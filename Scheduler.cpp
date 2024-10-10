#include "Scheduler.h"

#include "CPU.h"

Scheduler::Scheduler(int cores, CPU::Algorithm algorithm) : num_cores(cores), algorithm(algorithm)
{
}

void Scheduler::runScheduler()
{
	for(int cpu_core_id = 0; cpu_core_id < this->num_cores; cpu_core_id++)
	{
		this->cpu_cores.push_back(std::make_shared<CPU>(cpu_core_id, this->algorithm));
	}
}
