#pragma once
#include <queue>
#include <unordered_map>
#include "IMemoryAllocator.h"
#include <mutex>
class PagingAllocator : public IMemoryAllocator
{
public:
	PagingAllocator(size_t max_memory_size, size_t page_size);
	void* allocate(size_t size, std::string process_name) override;
	void deallocate(void* ptr, size_t size) override;
	void visualize_memory(int current_cycle) override;
	void dec_processes_in_memory() override;
	void inc_processes_in_memory() override;
private:
	using page_table = std::unordered_map<size_t, size_t>;
	size_t max_memory_size, page_size, frame_count;
	std::atomic<size_t> processes_in_memory = 0;

	std::mutex alloc_mutex;
	std::queue<size_t> free_frame_list;
	std::unordered_map<void*, page_table> proc_page_tbl_map; // map handle to its page table
	size_t next_handle = 1; // 0 is nullptr
	
};