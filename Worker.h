#pragma once
#include <chrono>

class Worker {
    public:
        Worker();
        ~Worker();
    protected:
        // setup function that inheriting class can implement
        virtual void setup();

        // loop function that inheriting class needs to implement
        virtual void loop() = 0;

        void sleep(std::chrono::duration<int64_t>);
        bool enabled = true;
};