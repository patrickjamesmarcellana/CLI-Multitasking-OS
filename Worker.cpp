#include "Worker.h"
#include <thread>

Worker::Worker() {
    thread_semaphore.acquire();
    std::thread cpuThread([this]() {
        while (enabled) {
            loop();
        }
        thread_semaphore.release();
        });
    cpuThread.detach();
}

Worker::~Worker() {
    enabled = false;
    thread_semaphore.acquire(); // wait for thread to release semaphore
}

void Worker::setup() {

}