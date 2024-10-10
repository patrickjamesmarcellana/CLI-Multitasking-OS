#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "ConcurrentPtrQueue.h"
#include "Process.h"

class ProcessManager
{
public:
	typedef std::unordered_map<std::string, std::shared_ptr<Process>>& ProcessMap;
	typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;

	ProcessManager(ProcessMap process_map, ProcessQueue process_queue, int min_ins, int max_ins);
	~ProcessManager() = default;

	std::shared_ptr<Process> save_process(std::string process_name);
	std::shared_ptr<Process> get_process(std::string process_name);
	void add_process_to_queue(std::string process_name);
	int generate_random_total_lines();

private:
	ProcessMap process_map;
	ProcessQueue process_queue;
	int min_ins;
	int max_ins;
};

