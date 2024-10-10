#include "ProcessManager.h"

#include <random>

ProcessManager::ProcessManager(ProcessMap process_map, ProcessQueue process_queue, int min_ins, int max_ins) :
process_map(process_map),
process_queue(process_queue),
min_ins(min_ins),
max_ins(max_ins)
{
}

std::shared_ptr<Process> ProcessManager::save_process(std::string process_name)
{
	int total_lines = generate_random_total_lines();
	std::shared_ptr<Process> new_process = std::make_shared<Process>(this->process_map.size(), process_name, total_lines);
	this->process_map[process_name] = new_process;
	return new_process;
}

std::shared_ptr<Process> ProcessManager::get_process(std::string process_name)
{
	return this->process_map[process_name];
}

void ProcessManager::add_process_to_queue(std::string process_name)
{
	std::shared_ptr<Process> process = this->get_process(process_name);
	this->process_queue->push(process);
}



int ProcessManager::generate_random_total_lines()
{
	std::random_device rand;
	std::mt19937 gen(rand());
	std::uniform_int_distribution<> distr(this->min_ins, this->max_ins);

	return distr(gen);
}
