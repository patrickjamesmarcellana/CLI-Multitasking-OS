#pragma once
#include <vector>
#include <memory>

#include "CPU.h"
#include "IMemoryAllocator.h"
#include "Process.h"

class Scheduler
{
public:
	typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;
	Scheduler(int cores, CPU::Algorithm algorithm, ProcessQueue process_queue, CPUClockSource& clock_source, std::shared_mutex& process_map_lock, long long int quantum_cycles,
		long long int delay_per_exec, IMemoryAllocator& memory_allocator);
	~Scheduler() = default;

	void runScheduler();
	int get_cores_used();
	int get_cores_available();
	int get_total_cores();
	int get_cpu_utilization();
	std::pair<size_t, size_t> get_idle_active_ticks();

private:
	int num_cores;
	CPU::Algorithm algorithm;
	std::vector<std::shared_ptr<CPU>> cpu_cores;
	std::vector<std::jthread> cpu_core_threads;
	std::shared_ptr<ConcurrentPtrQueue<Process>> process_queue;
	std::shared_mutex& process_map_lock;
	long long int quantum_cycles;
	long long int delay_per_exec;
	IMemoryAllocator& memory_allocator;
	CPUClockSource& clock_source;
};