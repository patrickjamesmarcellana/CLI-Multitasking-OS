#pragma once
#include <string>

class IMemoryAllocator
{
public:
	virtual void* allocate(size_t size, int process_id) = 0;
	virtual void deallocate(void* ptr, size_t size) = 0;
	virtual void visualize_memory() = 0;
};