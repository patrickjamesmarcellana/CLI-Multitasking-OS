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
	virtual void set_process_running_to_false(std::string process_name) = 0;
	virtual void set_process_running_to_true(std::string process_name) = 0;
	virtual bool is_process_in_backing_store(std::string process_name) = 0;
	virtual void delete_process_from_backing_store(std::string process_name) = 0;
	virtual size_t get_active_memory() = 0;
};