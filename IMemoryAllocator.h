#pragma once
#include <string>

class IMemoryAllocator
{
public:
	virtual void* allocate(size_t size, std::string process_name) = 0;
	virtual void deallocate(void* ptr, size_t size) = 0;
	virtual void visualize_memory(int current_cycle) = 0;
	virtual void inc_processes_in_memory() = 0;
	virtual void dec_processes_in_memory() = 0;
};