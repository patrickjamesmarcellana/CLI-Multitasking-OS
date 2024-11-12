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
    bool enabled = false;
private:
    friend class CPUClockSource;
    std::binary_semaphore cpuCanStartSem{ 1 }, cpuFinishedSem{ 0 };
};

class CPUClockSource {
public:
    // TODO: not hardcoded cpu limit
    CPUClockSource() : cpuSemaphores(8), clockThread(&CPUClockSource::clockThreadFunc, this) {};
    ~CPUClockSource() {
        clockThreadEnabled = false;
        clockThread.join();
    }
    void setCount(int cpuCount) {
        std::lock_guard loopLock(lock);
        this->cpuCount = cpuCount;
    }
    void shutdown() {
        std::lock_guard loopLock(lock);

        // loop is not yet running, we can ensure that no semaphore is going to ever call cpuFinishedSem.acquire() after we return from this shutdown function
        // which causes a deadlock when the cpu thread is already stopped (never going to finish again)
        shutdownMode = true;
    }
    CPUSemaphores& getSemaphores(int cpuId) {
        return cpuSemaphores[cpuId];
    }
    CPUSemaphores& getProcessManagerSemaphores() {
        return processManagerSemaphores;
    }
    bool shutdownMode = false;
    bool clockThreadEnabled = true;
private:
    void clockThreadFunc() {
        while (clockThreadEnabled) {
            std::lock_guard loopLock(lock);

            if (!shutdownMode) { // when shutting down, dont wait (this will cause a desync during the shutdown process)
                // wait until all are done
                for (int i = 0; i < cpuCount; i++) {
                    cpuSemaphores[i].cpuFinishedSem.acquire();
                }
                if (processManagerSemaphores.enabled) {
                    processManagerSemaphores.cpuFinishedSem.acquire();
                }
            }

            //std::cout << "CPU Tick #" + std::to_string(cpuTicks) + "\n";
            cpuTicks++;

            // notify that all can release
            for (int i = 0; i < cpuCount; i++) {
                cpuSemaphores[i].cpuCanStartSem.release();
            }
            processManagerSemaphores.cpuCanStartSem.release();
        }
    }
    int cpuCount = 0;
    int cpuTicks = 0;
    std::thread clockThread;
    std::vector<CPUSemaphores> cpuSemaphores;
    CPUSemaphores processManagerSemaphores;
    std::mutex lock;
};