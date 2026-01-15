#pragma once
#include <mutex>
#include <queue>

namespace utils {

template <typename T>
class ConcurrentQueue
{
public:
    bool pop(T& a)
    {
        std::lock_guard lock(_mutex);
        if (_queue.empty()) {
            return false;
        }
        a = _queue.front();
        _queue.pop();
        return true;
    }

    void push(const T& value)
    {
        std::lock_guard lock(_mutex);
        _queue.push(value);
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
};

}  // namespace utils
