#include <chrono>
#include <iostream>

#include "VirtualCPU.h"
using namespace std::literals::chrono_literals;

// TODO: not global
std::shared_ptr<ConcurrentPtrQueue<Process>> queue = std::make_shared<ConcurrentPtrQueue<Process>>();

VirtualCPU::VirtualCPU(int id) : Worker(), id(id) {

}

void VirtualCPU::loop() {
    if(!active_process || active_process->getCurrLine() >= active_process->getTotalLines()) {
        active_process = queue->try_pop();
    }

    if(active_process && active_process->getCurrLine() < active_process->getTotalLines()) {
        std::cout << "CPU " + std::to_string(id) + ": Hello world from " + active_process->getProcessName() + "\n";
        active_process->incCurrLine();
    }

    sleep(1ms);
}