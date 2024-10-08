#include <chrono>
#include <iostream>
#include <thread>
#include "VirtualCPU.h"
using namespace std::literals::chrono_literals;

// TODO: not global
std::shared_ptr<ConcurrentPtrQueue<Process>> queue = std::make_shared<ConcurrentPtrQueue<Process>>();

VirtualCPU::VirtualCPU(int id) : id(id) {
    std::thread cpuThread([this](){
        while(enabled) {
            loop();
        }
    });
    cpuThread.detach();
}

void VirtualCPU::loop() {
    if(!active_process || active_process->getCurrLine() >= active_process->getTotalLines()) {
        active_process = queue->try_pop();
    }

    if(active_process && active_process->getCurrLine() < active_process->getTotalLines()) {
        std::cout << "CPU " + std::to_string(id) + ": Hello world from " + active_process->getProcessName() << "\n";
        active_process->incCurrLine();
    }

    std::this_thread::sleep_for(1ms);
}

VirtualCPU::~VirtualCPU() {
    enabled = false;
}