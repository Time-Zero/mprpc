#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>

// 异步日志阻塞队列
template <typename T>
class LockQueue
{
public:
    void Push(const T &data);
    T Pop();

private:
    std::queue<T> m_queue_;
    std::mutex m_mtx_;
    std::condition_variable m_cv_;
};

// 多个生产者线程
template <typename T>
inline void LockQueue<T>::Push(const T &data)
{
    std::lock_guard<std::mutex> lck(m_mtx_);
    m_queue_.push(data);
    m_cv_.notify_one();     // 只有一个写线程，所以只需要唤醒一个线程
}

// 只有一个消费者线程
template <typename T>
inline T LockQueue<T>::Pop()
{
    std::unique_lock<std::mutex> lck(m_mtx_);
    while(m_queue_.empty())
    {
        m_cv_.wait(lck);
    }

    T data = m_queue_.front();
    m_queue_.pop();
    return std::move(data);
}
