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

void ThreadPool::MapRange(const function<void(int)>& func, const size_t begin, const size_t end)
{
    int chunkSize = (end - begin) / m_nthreads;
    for (int i = 0; i < m_nthreads; i++) {
        m_taskQueue.push([=]{
            int threadstart = begin + i*chunkSize;
            int threadstop = (i == m_nthreads - 1) ? end : threadstart + chunkSize;
            for (int it = threadstart; it < threadstop; ++it) {
                func(it);
            }
        });
    }
}
