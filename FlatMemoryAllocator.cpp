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

void* FlatMemoryAllocator::allocate(size_t size, int process_id)
{
	if(this->fit_approach == FIRST_FIT)
	{
		for(size_t index = 0; index < this->maximum_size - size + 1; ++index)
		{
			if(!allocation_map[index] && can_allocate_at(index, size))
			{
				allocate_at(index, size, process_id);
				return &memory[index];
			}
		}

		return nullptr;
	}

	return nullptr;
}

void FlatMemoryAllocator::deallocate(void* ptr, size_t size)
{
	size_t index = static_cast<int*>(ptr) - &memory[0];
	if(allocation_map[index])
	{
		deallocate_at(index, size);
	}
}

void FlatMemoryAllocator::dump_memory_state_to_stream(std::ostream& stream) {
	stream << "Timestamp: " << this->format_time(std::chrono::system_clock::now()) << std::endl;
	stream << "Number of processes in memory: " << this->processes_in_memory << std::endl;
	stream << "Total External Fragmentation: " << this->compute_total_external_fragmentation() << std::endl;
	stream << "\n" << "----end---- = " << this->maximum_size << std::endl;



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
	this->processes_in_memory--;
}

void FlatMemoryAllocator::inc_processes_in_memory()
{
	this->processes_in_memory++;
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

void FlatMemoryAllocator::allocate_at(size_t index, size_t size, int process_id)
{
	for(int i = index; i < index + size; i++)
	{
		if(i < maximum_size)
		{
			this->allocation_map[i] = true;
			this->memory[i] = process_id;
		}
	}
	this->allocated_size += size;
}

void FlatMemoryAllocator::deallocate_at(size_t index, size_t size)
{
	for (int i = index; i < index + size; i++)
	{
		if(i < this->maximum_size)
		{
			this->allocation_map[i] = false;
			this->memory[i] = -1;
		}
	}
	allocated_size -= size;
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
	size_t external_fragmentation = 0;
	for(int i = 0; i < maximum_size; i++)
	{
		if(this->allocation_map[i] == false)
		{
			external_fragmentation++;
		}
	}

	return external_fragmentation;
}
