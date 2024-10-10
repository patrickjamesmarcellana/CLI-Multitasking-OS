#include <thread>
#include "Worker.h"

Worker::Worker() {
    std::thread cpuThread([this](){
        while(enabled) {
            loop();
        }
    });
    cpuThread.detach();
}

Worker::~Worker() {
    enabled = false;
}

void Worker::setup() {

}

void Worker::sleep(std::chrono::duration<int64_t> duration) {
    std::this_thread::sleep_for(duration);
}