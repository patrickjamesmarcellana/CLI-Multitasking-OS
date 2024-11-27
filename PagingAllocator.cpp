#include "PagingAllocator.h"
#include <iostream>
#include <iomanip>

// positive numbers only
#define ceil_div(a, b) ((a + b - 1) / b)

PagingAllocator::PagingAllocator(size_t max_memory_size, size_t page_size) : 
	max_memory_size(max_memory_size), 
	page_size(page_size),
	frame_count(max_memory_size / page_size)
{
	for (size_t i = 0; i < this->frame_count; i++) {
		this->free_frame_list.push(i);
	}
}

void* PagingAllocator::allocate(size_t size, std::string process_name)
{
	std::lock_guard lock(this->alloc_mutex);

	size_t pages_needed = ceil_div(size, this->page_size);
	if (this->free_frame_list.size() < pages_needed) {
		return nullptr;
	}
	
	void* handle = (void*)this->next_handle++;
	for (size_t i = 0; i < pages_needed; i++) {
		size_t next_frame = this->free_frame_list.front();
		this->free_frame_list.pop();

		this->proc_page_tbl_map[handle][i] = next_frame;
	}

	return handle;
}

void PagingAllocator::deallocate(void* ptr, size_t size)
{
	std::lock_guard lock(this->alloc_mutex);

	for (auto& [page_number, frame_number] : this->proc_page_tbl_map[ptr]) {
		this->free_frame_list.push(frame_number);
	}
	this->proc_page_tbl_map.erase(ptr);
}

void PagingAllocator::visualize_memory(int current_cycle)
{
	std::lock_guard lock(this->alloc_mutex);

	for (auto& [handle, page_table] : this->proc_page_tbl_map) {
		std::cout << "Handle id=" << handle << std::endl;
		std::cout << "Page no. | Frame no. " << std::endl;
		for (auto& [page_number, frame_number] : page_table) {
			
			std::cout << std::setw(8) << page_number << " | " << std::setw(9) << frame_number << std::endl;
		}
	}
}

void PagingAllocator::dec_processes_in_memory()
{
	this->processes_in_memory--;
}

void PagingAllocator::inc_processes_in_memory()
{
	this->processes_in_memory++;
}
