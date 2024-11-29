#pragma once
#include <queue>
#include <unordered_map>
#include "IMemoryAllocator.h"
#include <mutex>
#include "BackingStore.h"
class PagingAllocator : public IMemoryAllocator
{
public:
	PagingAllocator(size_t max_memory_size, size_t page_size, BackingStore backing_store);
	void* allocate(size_t size, std::string process_name) override;
	void deallocate(void* ptr, size_t size) override;
	void visualize_memory(int current_cycle) override;
	void dec_processes_in_memory() override;
	void inc_processes_in_memory() override;
	size_t get_active_memory() override;
	void set_process_running_to_false(std::string process_name) override;
	void set_process_running_to_true(std::string process_name) override;
	bool is_process_in_backing_store(std::string process_name) override;
	void delete_process_from_backing_store(std::string process_name) override;
private:
	class page_table {
	public:
		std::unordered_map<size_t, size_t> table;
		bool is_running = true;
		int age;
	};
	size_t max_memory_size, page_size, frame_count;
	std::atomic<size_t> processes_in_memory = 0;
	size_t age = 0;

	BackingStore backing_store;
	std::recursive_mutex alloc_mutex;
	std::queue<size_t> free_frame_list;
	std::unordered_map<void*, page_table> proc_page_tbl_map; // map handle to its page table
	std::unordered_map<std::string, void*> proc_handle_map;
	std::unordered_map<void*, std::string> handle_proc_map;
	size_t next_handle = 1; // 0 is nullptr
	
};