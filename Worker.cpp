#include "Worker.h"
#include <thread>

Worker::Worker() {
    std::thread cpuThread([this]() {
        while (enabled) {
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