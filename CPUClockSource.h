#pragma once
#include <semaphore>
#include <thread>
#include <vector>
#include <iostream>

class CPUSemaphores {
public:
    void waitUntilCycleStart() {
        cpuCanStartSem.acquire();
    }
    void notifyDone() {
        cpuFinishedSem.release();
    }
private:
    friend class CPUClockSource;
    std::binary_semaphore cpuCanStartSem{1}, cpuFinishedSem{0};
};

class CPUClockSource {
public:
    CPUClockSource(int cpuCount) : cpuSemaphores(cpuCount), clockThread(&CPUClockSource::clockThreadFunc, this)  {};
    ~CPUClockSource() {
        clockThreadEnabled = false;
        clockThread.join();
    }
    CPUSemaphores& getSemaphores(int cpuId) {
        return cpuSemaphores[cpuId];
    }
private:
    void clockThreadFunc() {
        while(clockThreadEnabled) {
            // wait until all are done
            for(CPUSemaphores& semaphores : cpuSemaphores) {
                semaphores.cpuFinishedSem.acquire();
            }

            //std::cout << "CPU Tick #" + std::to_string(cpuTicks) + "\n";
            cpuTicks++;

            // notify that all can release
            for(CPUSemaphores& semaphores : cpuSemaphores) {
                semaphores.cpuCanStartSem.release();
            }
        }
    }
    int cpuTicks = 0;
    bool clockThreadEnabled = true;
    std::thread clockThread;
    std::vector<CPUSemaphores> cpuSemaphores;
};