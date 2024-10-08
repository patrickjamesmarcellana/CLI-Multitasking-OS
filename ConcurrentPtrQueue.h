#pragma once
#include <memory>
#include <mutex>
#include <queue>

template <typename T>
class ConcurrentPtrQueue {
    public:
        void push(std::shared_ptr<T> item);
        std::shared_ptr<T> try_pop();
    private:
        std::mutex queue_mutex;
        std::queue<std::shared_ptr<T>> queue;
};

#include "ConcurrentPtrQueue.tpp"