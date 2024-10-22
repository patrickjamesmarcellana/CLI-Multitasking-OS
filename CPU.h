#pragma once
#include <memory>
#include <queue>
#include <shared_mutex>
#include "CPUClockSource.h"
#include "CPUUsageTracker.h"
#include "ConcurrentPtrQueue.h"
#include "Process.h"
#include "Worker.h"


class CPU : Worker {
public:
    typedef std::shared_ptr<Process> ProcessPtr;
    typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;

    enum Algorithm
    {
        FCFS,
        RR
    };

    CPU(int id, Algorithm algorithm, ProcessQueue process_queue, long long int delay_per_exec, CPUSemaphores& semaphores);
    ~CPU() = default;
    bool get_is_busy();
    void inc_cpu_counter();
    void inc_cpu_active_counter();
    float get_cpu_usage();

private:
    int id;
    Algorithm algorithm;
    ProcessPtr active_process;
    ProcessQueue process_queue;
    long long int delay_per_exec;
    virtual void loop();
    bool is_busy = false;
    long long int process_cpu_counter = 0;
    
    CPUTracker cpu_usage;

    CPUSemaphores& semaphores;
};