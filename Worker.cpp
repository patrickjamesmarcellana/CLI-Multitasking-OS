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
