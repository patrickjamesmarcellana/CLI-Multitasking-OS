#pragma once
#include <memory>
#include <queue>
#include "ConcurrentPtrQueue.h"
#include "Process.h"
#include "Worker.h"

typedef std::shared_ptr<Process> ProcessPtr;
class VirtualCPU : Worker{
public:
    VirtualCPU(int id);
private:
    int id;
    //std::shared_ptr<ConcurrentPtrQueue<Process>> queue;
    ProcessPtr active_process;

    virtual void loop();
};

extern std::shared_ptr<ConcurrentPtrQueue<Process>> queue;