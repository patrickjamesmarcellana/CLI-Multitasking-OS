#include "Scheduler.h"

#include "CPU.h"

Scheduler::Scheduler(int cores, CPU::Algorithm algorithm, ProcessQueue process_queue, long long int quantum_cycles, long long int delay_per_exec) :
num_cores(cores),
algorithm(algorithm),
process_queue(process_queue),
quantum_cycles(quantum_cycles),
delay_per_exec(delay_per_exec)
{
}

void Scheduler::runScheduler()
{
	for(int cpu_core_id = 0; cpu_core_id < this->num_cores; cpu_core_id++)
	{
		this->cpu_cores.push_back(std::make_shared<CPU>(cpu_core_id, this->algorithm, this->process_queue, this->quantum_cycles, this->delay_per_exec));
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
	float cpu_utilization = 0.0;
	for(auto& cpu : this->cpu_cores) {
		if(cpu->get_is_busy())
		{
			cpu_utilization += 25.0f;
		}
	}
 	return cpu_utilization;
}
