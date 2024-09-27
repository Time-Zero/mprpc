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
    T& Pop();

private:
    std::queue<T> m_queue_;
    std::mutex m_mtx_;
    std::condition_variable m_cv_;
};

template <typename T>
inline void LockQueue<T>::Push(const T &data)
{
}

template <typename T>
inline T &LockQueue<T>::Pop()
{
    // TODO: 在此处插入 return 语句
}
