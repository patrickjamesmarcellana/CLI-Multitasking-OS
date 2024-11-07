#pragma once
#include <unordered_map>
#include <vector>

#include "IMemoryAllocator.h"

class FlatMemoryAllocator : public IMemoryAllocator
{
public:
	FlatMemoryAllocator(size_t maximum_size);
	~FlatMemoryAllocator();

	void* allocate(size_t size) override;
	void deallocate(void* ptr) override;
	std::string visualize_memory() override;

private:
	size_t maximum_size;
	size_t allocated_size;
	std::vector<char> memory;
	std::unordered_map<size_t, bool> allocation_map;

	void initialize_memory();
	bool can_allocate_at(size_t index, size_t size) const;
	void allocate_at(size_t index, size_t size);
	void deallocate_at(size_t index);
};

