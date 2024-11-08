#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "IMemoryAllocator.h"

class FlatMemoryAllocator : public IMemoryAllocator
{
public:
	enum Primary_Fit_Approach
	{
		FIRST_FIT
	};


	FlatMemoryAllocator(size_t maximum_size, Primary_Fit_Approach fit_approach);
	~FlatMemoryAllocator();

	void* allocate(size_t size, int process_id) override;
	void deallocate(void* ptr, size_t size) override;
	std::string visualize_memory() override;

private:
	size_t maximum_size;
	size_t allocated_size;
	std::vector<int> memory;
	std::vector<bool> allocation_map;
	Primary_Fit_Approach fit_approach;

	void initialize_memory();
	bool can_allocate_at(size_t index, size_t size);
	void allocate_at(size_t index, size_t size, int process_id);
	void deallocate_at(size_t index, size_t size);
};

