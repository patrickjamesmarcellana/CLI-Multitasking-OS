#pragma once
#include <memory>
#include <queue>
#include <shared_mutex>
#include "CPUClockSource.h"
#include "CPUUsageTracker.h"
#include "ConcurrentPtrQueue.h"
#include "IMemoryAllocator.h"
#include "Process.h"
#include "Worker.h"


class CPU : public Worker {
public:
    typedef std::shared_ptr<Process> ProcessPtr;
    typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;

    enum Algorithm
    {
        FCFS,
        RR
    };

    CPU(int id, Algorithm algorithm, ProcessQueue process_queue, CPUSemaphores& semaphores, std::shared_mutex& process_map_lock, long long int quantum_cycles, long long int delay_per_exec,
        IMemoryAllocator& memory_allocator);
    ~CPU() = default;
    bool get_is_busy();
    void inc_cpu_counter();
    float get_cpu_usage();

    void handle_finished_processes();
    void handle_reception_of_process();
    void handle_execution_of_process();

    void* try_allocating_memory_for_new_process(std::shared_ptr<Process> process);
    void deallocate_memory_of_active_process();
    std::shared_ptr<Process> get_process_from_queue();

private:
    int id;
    Algorithm algorithm;
    ProcessPtr active_process;
    ProcessQueue process_queue;
    std::shared_mutex& process_map_lock;
    long long int quantum_cycles;
    long long int delay_per_exec;
    virtual void loop();
    bool is_busy = false;
    long long int process_cpu_counter = 0;
    long long int active_process_time_slice_expiry;
    IMemoryAllocator& memory_allocator;
    
    CPUTracker cpu_usage;
    CPUSemaphores& semaphores;
};