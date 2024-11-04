#pragma once
#include <chrono>
#include <thread>

class Worker {
public:
    Worker() = default;
    ~Worker() = default;
    void operator()(std::stop_token stopToken) {
        setup();
        while (!stopToken.stop_requested()) {
            loop();
        }
    };
protected:
    // setup function that inheriting class can implement
    virtual void setup() {

    };

    // loop function that inheriting class needs to implement
    virtual void loop() = 0;

    template<typename Rep, typename Period> void sleep(std::chrono::duration<Rep, Period> duration) {
        std::this_thread::sleep_for(duration);
    };
};