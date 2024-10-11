#pragma once
#include <vector>
#include <memory>

#include "CPU.h"
#include "Process.h"

class Scheduler
{
public:
	typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;
	Scheduler(int cores, CPU::Algorithm algorithm, ProcessQueue process_queue, long long int delay_per_exec);
	~Scheduler() = default;

	void runScheduler();
	void stopScheduler();
	int get_cores_used();
	int get_cores_available();
	int get_total_cores();
	int get_cpu_utilization();

private:
	int num_cores;
	CPU::Algorithm algorithm;
	std::vector<std::shared_ptr<CPU>> cpu_cores;
	std::shared_ptr<ConcurrentPtrQueue<Process>> process_queue;
	long long int delay_per_exec;
};