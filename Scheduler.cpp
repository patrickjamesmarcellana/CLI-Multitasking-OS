#include "Scheduler.h"

#include "CPU.h"

Scheduler::Scheduler(int cores, CPU::Algorithm algorithm, ProcessQueue process_queue, std::shared_mutex& process_map_lock, long long int quantum_cycles, long long int delay_per_exec) :
num_cores(cores),
algorithm(algorithm),
process_queue(process_queue),
process_map_lock(process_map_lock),
quantum_cycles(quantum_cycles),
delay_per_exec(delay_per_exec)
{
}

void Scheduler::runScheduler()
{
	for(int cpu_core_id = 0; cpu_core_id < this->num_cores; cpu_core_id++)
	{
		auto cpu = std::make_shared<CPU>(cpu_core_id, this->algorithm, this->process_queue, this->process_map_lock, this->quantum_cycles, this->delay_per_exec);
		this->cpu_cores.push_back(cpu);

		// cannot directly pass CPU object as jthread wants to copy it
		// https://stackoverflow.com/questions/10503258/what-is-the-rationale-behind-stdbind-and-stdthread-always-copying-arguments
		//
		// can also be solved using std::ref, but it turns the smart pointer into a raw pointer (possibly unsafe)
		this->cpu_core_threads.push_back(std::jthread([cpu](std::stop_token stopToken) {
			(*cpu)(stopToken);
		})); 
	}
}

int Scheduler::get_cores_available()
{
	int cores_available = 0;
	for(const auto& cpu : this->cpu_cores)
	{
		if (!cpu->get_is_busy())
		{
			cores_available++;
		}
	}

	return cores_available;
}

int Scheduler::get_cores_used()
{
	return this->num_cores - this->get_cores_available();
}

int Scheduler::get_total_cores()
{
	return this->num_cores;
}

int Scheduler::get_cpu_utilization()
{
	return this->get_cores_used() / this->get_cores_available() * 100;
}
