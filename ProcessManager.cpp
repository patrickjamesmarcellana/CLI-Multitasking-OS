#include "ProcessManager.h"

#include <thread>
#include <random>

ProcessManager::ProcessManager(ProcessMap process_map, ProcessQueue process_queue, long long int min_ins, long long int max_ins, long long int batch_process_freq) :
process_map(process_map),
process_queue(process_queue),
min_ins(min_ins),
max_ins(max_ins),
batch_process_freq(batch_process_freq)
{
}

std::shared_ptr<Process> ProcessManager::save_process(std::string process_name)
{
	std::unique_lock lock(process_map_lock);
	long long int total_lines = generate_random_total_lines();
	std::shared_ptr<Process> new_process = std::make_shared<Process>(this->process_map.size(), process_name, total_lines);
	this->process_map[process_name] = new_process;
	this->process_queue->push(new_process);
	return new_process;
}

std::shared_ptr<Process> ProcessManager::get_process(std::string process_name)
{
	std::shared_lock lock(process_map_lock);
	return this->process_map[process_name];
}

void ProcessManager::add_process_to_queue(std::string process_name)
{
	std::shared_ptr<Process> process = this->get_process(process_name);
	this->process_queue->push(process);
}

long long int ProcessManager::generate_random_total_lines()
{
	std::random_device rand;
	std::mt19937 gen(rand());
	std::uniform_int_distribution<long long> distr(this->min_ins, this->max_ins);

	return distr(gen);
}

bool ProcessManager::is_generating_processes()
{
	return this->process_generator != nullptr;
}

void ProcessManager::scheduler_test_thread()
{
	this->process_generator = std::make_unique<ProcessGenerator>(*this, batch_process_freq);
}

void ProcessManager::scheduler_test_thread_stop()
{
	this->process_generator = nullptr;
}