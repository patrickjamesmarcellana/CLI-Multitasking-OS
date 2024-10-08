#pragma once
#include <memory>
#include <queue>
#include "ConcurrentPtrQueue.h"
#include "Process.h"

typedef std::shared_ptr<Process> ProcessPtr;
class VirtualCPU {
public:
    VirtualCPU(int id);
    ~VirtualCPU();
private:
    int id;
    bool enabled = true;
    //std::shared_ptr<ConcurrentPtrQueue<Process>> queue;
    ProcessPtr active_process;

    void loop();
};

extern std::shared_ptr<ConcurrentPtrQueue<Process>> queue;