#include "Worker.h"
#include <thread>

Worker::Worker() : worker_thread([this]() {
    while (enabled) {
        loop();
    }}) {
}

Worker::~Worker() {
    enabled = false;
    worker_thread.detach(); // wait for thread to exit
}

void Worker::setup() {

}