#ifndef LOCKFREETHREADPOOL_H
#define LOCKFREETHREADPOOL_H

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <utility>
#include <future>

#include "ThreadPoolBase.hh"
#include "concurrentqueue.h"

using namespace std;
using namespace moodycamel;

typedef std::function<void()> WorkType;

class LockfreeThreadPool : public ThreadPoolBase {
public:

        LockfreeThreadPool(uint32_t num_threads)
        {
            for (uint32_t i=0; i<num_threads;i++) {
                m_workers.emplace_back(&LockfreeThreadPool::Worker, this);
            }
        }

        ~LockfreeThreadPool() {
            CleanUp();
            JoinAll();
        }

        void AddTask(WorkType task) override
        {
            m_taskQueue.enqueue(task);
        }

        void Worker() override
        {
            while(true) {
                function<void()> work;
                if (m_taskQueue.try_dequeue(work)) work();
                if (m_stopWorkers) return;
            }
        }

private:
        ConcurrentQueue<WorkType> m_taskQueue;
};

#endif /* end of include guard: THREADPOOL_H */
