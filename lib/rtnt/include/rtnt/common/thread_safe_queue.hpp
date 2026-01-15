#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>

namespace rtnt {

template <typename T>
class ThreadSafeQueue final
{
public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    /**
     * @brief   Adds an element to the back of the queue.
     *
     * @param   value   The value to push to the queue
     */
    void push(T value)
    {
        {
            std::lock_guard lock(_mutex);
            _queue.push(std::move(value));
        }  // nested scope so that mutex is released without waiting to notify.
        _cvar.notify_one();
    }

    /**
     * @brief   Removes the first element of the queue.
     *
     * @return  An @code std::optional<T>@endcode containing
     *          the popped value, or @code std::nullopt@endcode
     *          if the queue is empty.
     */
    std::optional<T> pop()
    {
        std::lock_guard lock(_mutex);

        if (_queue.empty()) {
            return std::nullopt;
        }

        T value = std::move(_queue.front());

        _queue.pop();
        return value;
    }

    /**
     * @return  @code true@endcode if no items are stored in the queue.
     */
    bool empty() const
    {
        std::lock_guard lock(_mutex);
        return _queue.empty();
    }

    /**
     * @return  The number of elements in the queue.
     */
    std::size_t size() const
    {
        std::lock_guard lock(_mutex);
        return _queue.size();
    }

private:
    mutable std::mutex _mutex;
    std::condition_variable _cvar;
    std::queue<T> _queue;
};

}  // namespace rtnt
