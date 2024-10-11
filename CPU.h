#pragma once
#include <memory>
#include <queue>
#include <shared_mutex>
#include "ConcurrentPtrQueue.h"
#include "Process.h"
#include "Worker.h"


class CPU : Worker {
public:
    typedef std::shared_ptr<Process> ProcessPtr;
    typedef std::shared_ptr<ConcurrentPtrQueue<Process>>& ProcessQueue;
    typedef struct cpu_stats_t {
        long long int active_counter = 0;
        long long int total_counter = 0;
        void increment_active() {
            active_counter++;
        }
        void increment_total() {
            total_counter++;
        }
    } cpu_stats_t;
    enum Algorithm
    {
        FCFS,
        RR
    };

    CPU(int id, Algorithm algorithm, ProcessQueue process_queue, long long int delay_per_exec);
    ~CPU() = default;
    bool get_is_busy();
    void inc_cpu_counter();
    void inc_cpu_active_counter();
    cpu_stats_t get_cpu_stats();

private:
    int id;
    Algorithm algorithm;
    ProcessPtr active_process;
    ProcessQueue process_queue;
    long long int delay_per_exec;
    virtual void loop();
    bool is_busy = false;
    long long int process_cpu_counter = 0;
    std::shared_mutex lock_cpu_stats;
    cpu_stats_t cpu_stats = {};
};