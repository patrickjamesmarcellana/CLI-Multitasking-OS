#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size, Primary_Fit_Approach fit_approach) :
maximum_size(maximum_size),
allocated_size(0),
fit_approach(fit_approach),
processes_in_memory(0)
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

		return nullptr;
	}

	return nullptr;
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


	stream << "\n" << "----start---- = 0" << std::endl;
	stream << std::endl;
}


void FlatMemoryAllocator::visualize_memory()
{
	std::ofstream stream("memory-log.txt", std::ios::trunc);
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
	this->allocations[alloc.relocation] = alloc;
	this->allocated_size += size;
}

void FlatMemoryAllocator::deallocate_at(size_t index, size_t size)
{
	auto allocation = this->allocations.find(index);
	if (allocation != this->allocations.end()) {
		allocated_size -= allocation->second.size;
		this->allocations.erase(index);
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
