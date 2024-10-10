#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "ConcurrentPtrQueue.h"
#include "Process.h"
#include "CPU.h"

class ProcessManager
{
public:
	typedef std::unordered_map<std::string, std::shared_ptr<Process>>& ProcessMap;
	typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;

	ProcessManager(ProcessMap process_map, ProcessQueue process_queue, long long int min_ins, long long int max_ins);
	~ProcessManager() = default;

	std::shared_ptr<Process> save_process(std::string process_name);
	std::shared_ptr<Process> get_process(std::string process_name);
	void add_process_to_queue(std::string process_name);
	long long int generate_random_total_lines();
	void generate_processes(int count);

private:
	ProcessMap process_map;
	ProcessQueue process_queue;
	long long int min_ins;
	long long int max_ins;
};

