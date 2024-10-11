#include "Worker.h"
#include <thread>

Worker::Worker() : worker_thread([this]() {
    while (enabled) {
        loop();
    }}) {
}

Worker::~Worker() {
    enabled = false;
    worker_thread.join(); // wait for thread to exit
}

void Worker::setup() {

}