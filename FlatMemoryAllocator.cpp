#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size, Primary_Fit_Approach fit_approach) :
maximum_size(maximum_size),
allocated_size(0),
fit_approach(fit_approach)
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

std::string FlatMemoryAllocator::visualize_memory()
{
	// custom ASCII printout to a text file
	// Timestamp: (MM/DD/yyyy HH:MM:SS:AM/PM)
	// Number of processes in memory: X
	// Total external fragmentation in KB: X

	// ----end---- = upper limit
	//
	// 16384
	// P1
	// 12288
	//
	// 8192
	// P9
	// 4096
	//
	// ----start---- = 0
	return std::string(memory.begin(), memory.end());
}

void FlatMemoryAllocator::initialize_memory()
{
	memory = std::vector<int>(maximum_size, -1); // -1 means unused
	for(size_t index = 0; index < maximum_size; ++index)
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
		this->allocation_map[i] = true;
		this->memory[i] = process_id;
	}
	this->allocated_size += size;
}

void FlatMemoryAllocator::deallocate_at(size_t index, size_t size)
{
	for (int i = index; i < index + size; i++)
	{
		this->allocation_map[i] = false;
		this->memory[i] = -1;
	}
	allocated_size -= size;
}
