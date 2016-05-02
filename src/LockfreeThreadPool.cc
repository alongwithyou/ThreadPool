#include "LockfreeThreadPool.hh"

LockfreeThreadPool::LockfreeThreadPool(uint32_t numthreads) : m_nthreads(numthreads), m_stopWorkers(false) {
    for (uint32_t i=0; i<numthreads;i++) {
        m_workers.emplace_back(&LockfreeThreadPool::Worker, this);
    }
}
LockfreeThreadPool::~LockfreeThreadPool() {
    m_stopWorkers = true;
    JoinAll();
}

void LockfreeThreadPool::Worker() {
    while(true) {
        function<void()> work;
        if (m_taskQueue.try_dequeue(work)) work();
        if (m_stopWorkers) return;
    }
}
void LockfreeThreadPool::JoinAll() {
    for (auto& worker : m_workers) { worker.join(); }
}
void LockfreeThreadPool::Finish() {
    for (auto& promise : m_promises) promise.get_future().get();
    m_promises.clear();
}
