#pragma once
#include <semaphore>
#include <thread>
#include <vector>
#include <iostream>

class CPUSemaphores {
public:
    CPUSemaphores(bool initialState) : enabled(initialState) {};
    CPUSemaphores() : enabled(true) {};
    void waitUntilCycleStart() {
        if(enabled) cpuCanStartSem.acquire();
    }
    void notifyDone() {
        if (enabled) cpuFinishedSem.release();
    }
    bool enabled;
private:
    friend class CPUClockSource;
    std::binary_semaphore cpuCanStartSem{ 1 }, cpuFinishedSem{ 0 };
};

class CPUClockSource {
public:
    // TODO: not hardcoded cpu limit
    CPUClockSource() : cpuSemaphores(8), processManagerSemaphores(false /* disabled initially */), clockThread(&CPUClockSource::clockThreadFunc, this) {};
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
    void togglePMThread(bool enabled) {
        std::lock_guard loopLock(lock);

        this->processManagerSemaphores.enabled = enabled;
        if (enabled) {
            // reset to (cpuCanStartSem = 1, cpuFinishedSem = 0)
            while (this->processManagerSemaphores.cpuCanStartSem.try_acquire());
            this->processManagerSemaphores.cpuCanStartSem.release();

            while (this->processManagerSemaphores.cpuFinishedSem.try_acquire());
        }
        else {
            // HACK: process generator can get stuck waiting for this semaphore if it was already waiting before the semaphore was disabled
            // after getting unstuck, it should not get stuck again as the semaphore is now disabled
            processManagerSemaphores.cpuCanStartSem.release();
        }
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
                if (this->processManagerSemaphores.enabled) {
                    processManagerSemaphores.cpuFinishedSem.acquire();
                }
            }

            //std::cout << "CPU Tick #" + std::to_string(cpuTicks) + "\n";
            cpuTicks++;

            // notify that all can release
            for (int i = 0; i < cpuCount; i++) {
                cpuSemaphores[i].cpuCanStartSem.release();
            }
            if (this->processManagerSemaphores.enabled) {
                processManagerSemaphores.cpuCanStartSem.release();
            }
        }
    }
    int cpuCount = 0;
    int cpuTicks = 0;
    std::thread clockThread;
    std::vector<CPUSemaphores> cpuSemaphores;
    CPUSemaphores processManagerSemaphores;
    std::mutex lock;
};