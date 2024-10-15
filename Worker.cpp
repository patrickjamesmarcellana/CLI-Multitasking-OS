#include "Worker.h"
#include <thread>

Worker::Worker() :
    workerThread([this]() {
        while (enabled) {
            loop();
        }
    }) {
}

Worker::~Worker() {
    enabled = false;
    workerThread.join(); // wait for thread to exit
}

void Worker::setup() {

}