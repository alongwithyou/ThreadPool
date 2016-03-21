#include "ThreadPool.hh"

ThreadPool::ThreadPool(uint32_t numthreads) : m_nthreads(numthreads), m_stopWorkers(false) {
        for (uint32_t i=0; i<numthreads;i++) {
                m_workers.emplace_back(&ThreadPool::Worker, this);
        }
}
ThreadPool::~ThreadPool() {
        m_stopWorkers = true;
        m_taskQueue.join();
        JoinAll();
}

void ThreadPool::Worker() {
    while(true) {
        function<void()> work;
        try {
            work = m_taskQueue.pop();
            work();
        }
        catch (const ThreadsafeQueue<WorkType>::QueueFinished&)
        {
            return;
        }
    }
}
void ThreadPool::JoinAll() {
        for (auto& worker : m_workers) { worker.join(); }
}
