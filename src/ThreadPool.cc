#include "ThreadPool.hh"

ThreadPool::ThreadPool(uint32_t numthreads) : m_nthreads(numthreads), m_stopWorkers(false) {
    for (uint32_t i=0; i<numthreads-1;i++) { // -1 b/c main thread is also a worker
        m_workers.emplace_back(&ThreadPool::Worker, this);
    }
}
ThreadPool::~ThreadPool() {
    m_stopWorkers = true;
    JoinAll();
}

void ThreadPool::Worker() {
    while(true) {
        function<void()> work;
        if (m_taskQueue.try_dequeue(work)) work();
        if (m_stopWorkers) return;
    }
}
void ThreadPool::JoinAll() {
    for (auto& worker : m_workers) { worker.join(); }
}
void ThreadPool::Finish() {
    for (auto& promise : m_promises) promise.get_future().get();
    m_promises.clear();
}
