#pragma once
#include <memory>
#include <queue>
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

    CPU(int id, Algorithm algorithm, ProcessQueue process_queue, long long int delay_per_exec);
    bool get_is_busy();

private:
    int id;
    Algorithm algorithm;
    ProcessPtr active_process;
    ProcessQueue process_queue;
    long long int delay_per_exec;
    virtual void loop();
    bool is_busy = false;
};