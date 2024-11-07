#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size) : maximum_size(maximum_size), allocated_size(0)
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

}

void FlatMemoryAllocator::deallocate(void* ptr)
{
}

std::string FlatMemoryAllocator::visualize_memory()
{
}

void FlatMemoryAllocator::initialize_memory()
{
}

bool FlatMemoryAllocator::can_allocate_at(size_t index, size_t size) const
{
}

void FlatMemoryAllocator::allocate_at(size_t index, size_t size)
{
}

void FlatMemoryAllocator::deallocate_at(size_t index)
{
}
