#pragma once
#include <string>

class IMemoryAllocator
{
public:
	virtual void* allocate(size_t size) = 0;
	virtual void deallocate(void* ptr, size_t size) = 0;
	virtual std::string visualize_memory() = 0;
};