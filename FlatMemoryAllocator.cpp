#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size, Primary_Fit_Approach fit_approach) : maximum_size(maximum_size), allocated_size(0), fit_approach(fit_approach)
{
	memory.reserve(maximum_size);
	initialize_memory();
}

FlatMemoryAllocator::~FlatMemoryAllocator()
{
	memory.clear();
}

void* FlatMemoryAllocator::allocate(size_t size)
{
	if(fit_approach == FIRST_FIT)
	{
		for(size_t index = 0; index < maximum_size - size + 1; ++index)
		{
			if(!allocation_map[index] && can_allocate_at(index, size))
			{
				allocate_at(index, size);
				return &memory[index];
			}
		}

		return nullptr;
	}
}

void FlatMemoryAllocator::deallocate(void* ptr)
{
	size_t index = static_cast<char*>(ptr) - &memory[0];
	if(allocation_map[index])
	{
		deallocate_at(index);
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
	std::fill(memory.begin(), memory.end(), '.');
	std::fill(allocation_map.begin(), allocation_map.end(), false);
}

bool FlatMemoryAllocator::can_allocate_at(size_t index, size_t size) const
{
	return (index + size <= maximum_size);
}

void FlatMemoryAllocator::allocate_at(size_t index, size_t size)
{
	std::fill(allocation_map.begin + index, allocation_map.begin + index + size, true);
	allocated_size += size;
}

void FlatMemoryAllocator::deallocate_at(size_t index)
{
	allocation_map[index] = false;
}
