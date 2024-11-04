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

	ProcessManager(ProcessMap process_map, std::shared_mutex& process_map_lock, ProcessQueue process_queue, long long int min_ins, long long int max_ins, long long int batch_process_freq);
	~ProcessManager() = default;

	std::shared_ptr<Process> save_process(std::string process_name);
	std::shared_ptr<Process> get_process(std::string process_name);
	void add_process_to_queue(std::string process_name);
	long long int generate_random_total_lines();
	bool is_generating_processes();
	void scheduler_test_thread();
	void scheduler_test_thread_stop();
	void update_configuration(long long int min_ins, long long int max_ins, long long int batch_process_freq);

private:
	class ProcessGenerator : public Worker {
	public:
		ProcessGenerator(ProcessManager &pm, long long int& freq) : Worker(), freq(freq), pm(pm) {}
	protected:
		virtual void loop() {
			if(cpuCycle++ % freq == 0) {
				pm.save_process("process_" + std::to_string(pm.process_map.size()));
			}
		}
	private:
			long long int cpuCycle = 0;
			long long int& freq;
			ProcessManager &pm;
	};

	ProcessMap process_map;
	ProcessQueue process_queue;
	std::unique_ptr<std::jthread> process_generator;
	long long int min_ins;
	long long int max_ins;
	long long int batch_process_freq;
	std::shared_mutex& process_map_lock;
};

