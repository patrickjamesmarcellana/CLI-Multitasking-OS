#include <memory>
#include <mutex>
#include <queue>
#include "ConcurrentPtrQueue.h"
#include "Process.h"

template <typename T>
void ConcurrentPtrQueue<T>::push(std::shared_ptr<T> item) {
    std::unique_lock lock(queue_mutex);
    return queue.push(item);
}

template <typename T>
std::shared_ptr<T> ConcurrentPtrQueue<T>::try_pop() {
    std::unique_lock lock(queue_mutex);
    if(queue.size() > 0) {
        std::shared_ptr<T> ret = queue.front();
        queue.pop();
        return ret;
    }
    return std::shared_ptr<T>(nullptr);
}
