#pragma once
#include <memory>
#include <queue>
#include "ConcurrentPtrQueue.h"
#include "Process.h"
#include "Worker.h"

typedef std::shared_ptr<Process> ProcessPtr;
class CPU : Worker {
public:
    enum Algorithm
    {
        FCFS,
        RR
    };

    CPU(int id, Algorithm algorithm);

private:
    int id;
    Algorithm algorithm;
    //std::shared_ptr<ConcurrentPtrQueue<Process>> queue;
    ProcessPtr active_process;

    virtual void loop();
};

extern std::shared_ptr<ConcurrentPtrQueue<Process>> queue;