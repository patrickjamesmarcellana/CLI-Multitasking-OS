#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size, Primary_Fit_Approach fit_approach, BackingStore backing_store) :
maximum_size(maximum_size),
allocated_size(0),
fit_approach(fit_approach),
processes_in_memory(0),
backing_store(backing_store)
{
	initialize_memory();
}

FlatMemoryAllocator::~FlatMemoryAllocator()
{
	memory.clear();
}

void* FlatMemoryAllocator::allocate(size_t size, std::string process_name)
{
	std::lock_guard<std::mutex> lock(mtx);
	if(this->fit_approach == FIRST_FIT)
	{
		size_t prev_end = 0; // it is the next index that is not reserved by the previous allocation
		for (auto& [_, allocation] : allocations) { // allocations are sorted by addresses
			size_t hole_size = allocation.relocation - prev_end;
			if (hole_size >= size) {
				allocate_at(prev_end, size, process_name);
				return &memory[prev_end];
			}
			prev_end = allocation.relocation + allocation.size;
		}

		// check remaining space to the right of allocations
		size_t hole_size = maximum_size - prev_end;
		if (hole_size >= size) {
			allocate_at(prev_end, size, process_name);
			return &memory[prev_end];
		}

		// get oldest allocated process and swap to the backing store
		size_t lower_new_free = 0;
		std::optional<std::pair<size_t, FlatMemoryAllocInfo>> oldest_process;
		for (auto& [position, alloc_info] : allocations) { // allocations are sorted by addresses
			if(!alloc_info.running &&
				(!oldest_process || alloc_info.age < oldest_process->second.age))
			{
				size_t lower = position;
				size_t upper = lower + alloc_info.size;

				// find first free space below lower
				
				for(long long int i = lower - 1; i >= 0; i--)
				{
					if(this->allocation_map[i])
					{
						lower_new_free = i + 1;
						break;
					}
				}

				// find first free space below lower
				size_t upper_new_free = upper - 1;
				for (size_t i = upper + 1; i < this->maximum_size; i++)
				{
					if (i < this->maximum_size && this->allocation_map[i])
					{
						upper_new_free = i - 1;
						break;
					}
				}

				// compute for the new available free space when the chosen oldest process is removed
				size_t new_memory_space_available = upper_new_free - lower_new_free + 1;

				if(new_memory_space_available >= size)
				{
					// new memory space available can fit new process
					oldest_process = { position, alloc_info };
				}
			}
		}

		if(oldest_process)
		{
			// store the oldest process in the backing store
			this->backing_store.storeProcess(oldest_process->second.name, 1);

			// deallocate oldest process memory from the main memory
			this->deallocate_at(oldest_process->first, oldest_process->second.size);

			// allocate the new process to the lowest free available space from the lower bound of the swapped process
			allocate_at(lower_new_free, size, process_name);
			return &memory[lower_new_free];
		}

		return nullptr;
	}

	return nullptr;
}

bool FlatMemoryAllocator::is_process_in_backing_store(std::string process_name)
{
	const std::string filename = process_name + "_memory.txt";
	if(std::filesystem::exists(filename))
	{
		return true;
	} else
	{
		return false;
	}
}

void FlatMemoryAllocator::delete_process_from_backing_store(std::string process_name)
{
	const std::string filename = process_name + "_memory.txt";

	try
	{
		std::filesystem::remove(filename);
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << '\n';
	}
}

void FlatMemoryAllocator::set_process_running_to_false(std::string process_name)
{
	for (auto& [position, alloc_info] : this->allocations) { // allocations are sorted by addresses
		if (alloc_info.running && alloc_info.name == process_name)
		{
			alloc_info.running = false;
		}
	}
}

void FlatMemoryAllocator::set_process_running_to_true(std::string process_name)
{
	for (auto& [position, alloc_info] : this->allocations) { // allocations are sorted by addresses
		if (!alloc_info.running && alloc_info.name == process_name)
		{
			alloc_info.running = true;
		}
	}
}


void FlatMemoryAllocator::deallocate(void* ptr, size_t size)
{
	std::lock_guard<std::mutex> lock(mtx);
	size_t index = static_cast<int*>(ptr) - &memory[0]; // sidenote: this is dangerous because the vector can change address at any given time
	deallocate_at(index, size);
}

void FlatMemoryAllocator::dump_memory_state_to_stream(std::ostream& stream) {
	std::lock_guard<std::mutex> lock(mtx);
	stream << "Timestamp: " << this->format_time(std::chrono::system_clock::now()) << std::endl;
	stream << "Number of processes in memory: " << this->processes_in_memory << std::endl;
	stream << "Total External Fragmentation: " << this->compute_total_external_fragmentation() << std::endl;
	stream << "\n" << "----end---- = " << this->maximum_size << std::endl << std::endl;

	for (auto process_it = this->allocations.crbegin(); process_it != this->allocations.crend(); ++process_it) {
		auto& alloc_info = process_it->second;
		stream << alloc_info.relocation + alloc_info.size << std::endl;
		stream << alloc_info.name << std::endl;
		stream << alloc_info.relocation << std::endl;
		stream << std::endl;
	}


	stream << "----start---- = 0" << std::endl;
	stream << std::endl;
}


void FlatMemoryAllocator::visualize_memory(int current_cycle)
{
	std::ofstream stream("memory_stamp_" + std::to_string(current_cycle) + ".txt", std::ios::trunc);
	this->dump_memory_state_to_stream(stream);
}

void FlatMemoryAllocator::dec_processes_in_memory()
{
	std::lock_guard<std::mutex> lock(mtx); // TODO: replace with atomic int
	this->processes_in_memory--;
}

void FlatMemoryAllocator::inc_processes_in_memory()
{
	std::lock_guard<std::mutex> lock(mtx);
	this->processes_in_memory++;
	//std::cout << std::to_string(this->processes_in_memory) + "\n";
}

size_t FlatMemoryAllocator::get_active_memory()
{
	return this->allocated_size;
}

void FlatMemoryAllocator::initialize_memory()
{
	memory = std::vector<int>(maximum_size, -1); // -1 means unused
	for(size_t index = 0; index < maximum_size; index++)
	{
		allocation_map[index] = false;
	}
}

bool FlatMemoryAllocator::can_allocate_at(size_t index, size_t size)
{
	return (index + size <= maximum_size);
}

void FlatMemoryAllocator::allocate_at(size_t index, size_t size, std::string& process_name)
{
	FlatMemoryAllocInfo alloc;
	alloc.relocation = index;
	alloc.size = size;
	alloc.name = process_name;
	alloc.running = true;
	alloc.age = current_age;
	this->allocations[alloc.relocation] = alloc;
	this->allocated_size += size;
	this->current_age++;

	for (size_t i = index; i < index + size; i++)
	{
		allocation_map[i] = true;
	}
}

void FlatMemoryAllocator::deallocate_at(size_t index, size_t size)
{
	auto allocation = this->allocations.find(index);
	if (allocation != this->allocations.end()) {
		allocated_size -= allocation->second.size;
		this->allocations.erase(index);

		for (size_t i = index; i < index + size; i++)
		{
			allocation_map[i] = false;
		}
	}
	else {
		std::cout << "WARN: Could not find allocation starting at index " << index << std::endl;
	}

}

std::string FlatMemoryAllocator::format_time(const std::chrono::time_point<std::chrono::system_clock>& time_executed) {
	std::time_t time_t_time_executed = std::chrono::system_clock::to_time_t(time_executed);
	std::tm local_time_executed;

	localtime_s(&local_time_executed, &time_t_time_executed);

	std::ostringstream oss;
	oss << "(" << std::put_time(&local_time_executed, "%m/%d/%Y %I:%M:%S%p") << ")";

	return oss.str();
}

size_t FlatMemoryAllocator::compute_total_external_fragmentation()
{
	return this->maximum_size - this->allocated_size;
}
