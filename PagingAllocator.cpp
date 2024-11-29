#include "PagingAllocator.h"
#include <iostream>
#include <iomanip>
#include <filesystem>

// positive numbers only
#define ceil_div(a, b) ((a + b - 1) / b)

PagingAllocator::PagingAllocator(size_t max_memory_size, size_t page_size, BackingStore& backing_store) :
	max_memory_size(max_memory_size), 
	page_size(page_size),
	frame_count(max_memory_size / page_size),
	backing_store(backing_store)
{
	for (size_t i = 0; i < this->frame_count; i++) {
		this->free_frame_list.push(i);
	}
}

void* PagingAllocator::allocate(size_t size, std::string process_name)
{
	std::lock_guard lock(this->alloc_mutex);

	size_t pages_needed = ceil_div(size, this->page_size);
	while (this->free_frame_list.size() < pages_needed) {
		void* handle_to_evict = nullptr;
		size_t age_of_handle_to_evict;
		for (auto& [handle, info] : this->proc_page_tbl_map) {
			if (!info.is_running && (handle_to_evict == nullptr || info.age < age_of_handle_to_evict)) {
				handle_to_evict = handle;
				age_of_handle_to_evict = info.age;
			}
		}

		if (handle_to_evict != nullptr) {
			backing_store.storeProcess(handle_proc_map[handle_to_evict], proc_page_tbl_map[handle_to_evict].table.size() * this->page_size);
			deallocate(handle_to_evict, 0 /* ignored */);
		}
		else {
			return nullptr;
		}
	}
	
	void* handle = (void*)this->next_handle++;
	this->proc_page_tbl_map[handle].age = this->age++;

	for (size_t i = 0; i < pages_needed; i++) {
		size_t next_frame = this->free_frame_list.front();
		this->free_frame_list.pop();

		this->proc_page_tbl_map[handle].table[i] = next_frame;
	}

	this->handle_proc_map[handle] = process_name;
	this->proc_handle_map[process_name] = handle;
	return handle;
}

void PagingAllocator::deallocate(void* ptr, size_t size)
{
	std::lock_guard lock(this->alloc_mutex);

	for (auto& [page_number, frame_number] : this->proc_page_tbl_map[ptr].table) {
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
		for (auto& [page_number, frame_number] : page_table.table) {
			
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

size_t PagingAllocator::get_active_memory()
{
	std::lock_guard lock(this->alloc_mutex);

	size_t total = 0;
	for (auto& [handle, page_table] : this->proc_page_tbl_map) {
		total += page_table.table.size() * this->page_size;
	}
	return total;
}

void PagingAllocator::set_process_running_to_false(std::string process_name)
{
	std::lock_guard lock(this->alloc_mutex);
	void* handle = this->proc_handle_map[process_name];
	this->proc_page_tbl_map[handle].is_running = false;
}

void PagingAllocator::set_process_running_to_true(std::string process_name)
{
	std::lock_guard lock(this->alloc_mutex);
	void* handle = this->proc_handle_map[process_name];
	this->proc_page_tbl_map[handle].is_running = true;
}

bool PagingAllocator::is_process_in_backing_store(std::string process_name)
{
	const std::string filename = process_name + "_memory.txt";
	if (std::filesystem::exists(filename))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void PagingAllocator::delete_process_from_backing_store(std::string process_name)
{
	const std::string filename = process_name + "_memory.txt";

	try
	{
		std::filesystem::remove(filename);
		this->backing_store.page_out++;
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << '\n';
	}
}