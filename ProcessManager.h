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

	ProcessManager(ProcessMap process_map, std::shared_mutex& process_map_lock, ProcessQueue process_queue, CPUClockSource& clock_source, long long int min_ins, long long int max_ins, long long int batch_process_freq,
				   long long int min_mem_per_proc, long long int max_mem_per_proc);

	std::shared_ptr<Process> save_process(std::string process_name);
	std::shared_ptr<Process> get_process(std::string process_name);
	void add_process_to_queue(std::string process_name);
	long long int generate_random_total_lines();
	bool is_generating_processes();
	void scheduler_test_thread();
	void scheduler_test_thread_stop();
	void update_configuration(long long int min_ins, long long int max_ins, long long int batch_process_freq, long long int min_mem_per_proc, long long int max_mem_per_proc);
	long long int generate_random_memory_req();

private:
	class ProcessGenerator : public Worker {
	public:
		ProcessGenerator(ProcessManager &pm, long long int& freq) : Worker(), freq(freq), pm(pm) {}
	protected:
		virtual void loop() {
			pm.process_manager_semaphores.waitUntilCycleStart();
			if(cpuCycle++ % freq == 0) {
				//std::cout << "dump\n";
				pm.save_process("process_" + std::to_string(pm.process_map.size()));
			}
			pm.process_manager_semaphores.notifyDone();
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
	long long int min_mem_per_proc;
	long long int max_mem_per_proc;

	// prevent both changes to the pointers stored in the process map and changes to the individual process
	// used when printing out process map
	std::shared_mutex& entire_process_map_lock;

	// prevent changes to the pointers stored in the process map, but allow changes to an indivudal process
	std::shared_mutex fine_grain_process_map_rw_lock;

	CPUSemaphores& process_manager_semaphores;
};

