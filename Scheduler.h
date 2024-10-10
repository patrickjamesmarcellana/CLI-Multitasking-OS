#pragma once
#include <vector>
#include <memory>

#include "CPU.h"
#include "Process.h"

class Scheduler
{
public:
	typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;
	Scheduler(int cores, CPU::Algorithm algorithm, ProcessQueue process_queue);
	~Scheduler() = default;

	void runScheduler();

private:
	int num_cores;
	CPU::Algorithm algorithm;
	std::vector<std::shared_ptr<CPU>> cpu_cores;
	std::shared_ptr<ConcurrentPtrQueue<Process>> process_queue;
};