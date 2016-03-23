#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadsafeQueue
{
public:
    ThreadsafeQueue() : m_finished(false), m_maxSize(20) {}
    ThreadsafeQueue(const size_t maxSize) : m_finished(false), m_maxSize(maxSize) {}
    ThreadsafeQueue(const ThreadsafeQueue&) = delete;
    ThreadsafeQueue(ThreadsafeQueue&&) = delete;

    void push(const T& task)
    {
        std::unique_lock<std::mutex> lock(m_qmtx);
        m_cond.wait(lock, [this]{ return m_contents.size() < m_maxSize; });
        m_contents.push(task);
        m_cond.notify_all();
    }

    void push(T&& task)
    {
        std::unique_lock<std::mutex> lock(m_qmtx);
        m_cond.wait(lock, [this]{ return m_contents.size() < m_maxSize; });
        m_contents.push(std::move(task));
        m_cond.notify_all();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(m_qmtx);
        m_cond.wait(lock, [this]{ return !m_contents.empty() || m_finished; });
        if (m_finished) throw QueueFinished();
        T item = std::move(m_contents.front());
        m_contents.pop();
        m_cond.notify_all();  // I'm not sure if this can be called before the return...
        return item;
    }

    void join()
    {
        std::unique_lock<std::mutex> lock(m_qmtx);
        m_cond.wait(lock, [this]{ return m_contents.empty(); });
        m_finished = true;
        m_cond.notify_all();
    }

    class QueueFinished : public std::exception
    {
    public:
        virtual const char* what() const noexcept { return "Queue has been joined"; };
    };

private:
    std::queue<T> m_contents;
    std::mutex m_qmtx;
    std::condition_variable m_cond;
    bool m_finished;
    size_t m_maxSize;
};

#endif /* end of include guard: THREADSAFEQUEUE_H */
