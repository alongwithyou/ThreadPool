#include "ThreadPool.hh"
ThreadPool::ThreadPool(uint32_t numthreads) : m_nthreads(numthreads), stop_workers(false),
                                              m_nrunning(0) {

        for (uint32_t i=0; i<numthreads;i++) {
                m_workers.emplace_back(&ThreadPool::Worker, this);
        }
}
ThreadPool::~ThreadPool() {

        stop_workers = true;
        m_condition.notify_all();
        JoinAll();
}
// TODO: add futures and promises for timing purposes
void ThreadPool::Enqueue(function<void()> task) {
        {
                unique_lock<mutex> lock(m_taskmutex);
                m_tasks.push(task);
        }
        m_condition.notify_one();
}
void ThreadPool::Worker() {
        function<void()> work;
        while(true) {
                {
                        unique_lock<mutex> lock(m_taskmutex);
                        //cout << "Worker waiting..." << endl;
                        while(!stop_workers && m_tasks.empty()) {
                                m_condition.wait(lock);
                        }
                        if (stop_workers) { return; }
                        //cout << "Booting up..." << endl;
                        work = m_tasks.front();
                        m_tasks.pop();
                }
                work();
        }
}
void ThreadPool::JoinAll() {
        for (auto& worker : m_workers) { worker.join(); }
}
