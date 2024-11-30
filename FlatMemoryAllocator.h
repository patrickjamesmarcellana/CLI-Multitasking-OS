#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>
#include <mutex>
#include <filesystem>

#include "IMemoryAllocator.h"
#include <set>
#include <map>

#include "BackingStore.h"

class FlatMemoryAllocator : public IMemoryAllocator
{
public:
	enum Primary_Fit_Approach
	{
		FIRST_FIT
	};
	FlatMemoryAllocator(size_t maximum_size, Primary_Fit_Approach fit_approach, BackingStore& backing_store);
	~FlatMemoryAllocator();

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
	class FlatMemoryAllocInfo {
	public:
		std::string name;
		int age;
		bool running;
		size_t relocation; // phy addr of virtual memory location 0
		size_t size;
		bool operator<(FlatMemoryAllocInfo& other) {
			return this->relocation < other.relocation;
		}
	};
	std::map<size_t, FlatMemoryAllocInfo> allocations;

	int current_age = 0;
	size_t maximum_size;
	size_t allocated_size;
	std::vector<int> memory;
	std::map<size_t, bool> allocation_map;
	Primary_Fit_Approach fit_approach;
	int processes_in_memory;
	BackingStore& backing_store;

	void initialize_memory();
	bool can_allocate_at(size_t index, size_t size);
	void allocate_at(size_t index, size_t size, std::string& process_name);
	void deallocate_at(size_t index, size_t size);

	void dump_memory_state_to_stream(std::ostream& stream);
	std::string format_time(const std::chrono::time_point<std::chrono::system_clock>& time_executed);
	size_t compute_total_external_fragmentation();

	std::mutex mtx;
};

