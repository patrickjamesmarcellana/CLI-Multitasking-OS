#pragma once
#include <vector>
#include <memory>

#include "CPU.h"
#include "Process.h"

class Scheduler
{
public:

	Scheduler(int cores, CPU::Algorithm algorithm);
	~Scheduler() = default;

	void runScheduler();

private:
	int num_cores;
	CPU::Algorithm algorithm;
	std::vector<std::shared_ptr<CPU>> cpu_cores;
};