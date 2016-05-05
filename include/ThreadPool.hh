#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <utility>
#include <future>

#include "ThreadPoolBase.hh"

#include "ThreadsafeQueue.hh"

using namespace std;

typedef std::function<void()> WorkType;

class ThreadPool : public ThreadPoolBase {
public:

        ThreadPool(uint32_t num_threads)
        {
            for (uint32_t i=0; i<num_threads;i++) {
                m_workers.emplace_back(&ThreadPool::Worker, this);
            }
        }

        ~ThreadPool() {
            CleanUp();
            JoinAll();
        }

        void AddTask(WorkType task) override
        {
            m_taskQueue.push(task);
        }

        void CleanUp() override
        {
            m_stopWorkers = true;
            m_taskQueue.join();
        }

        void Worker() override {
            while(true) {
                WorkType work;
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

private:
        ThreadsafeQueue<WorkType> m_taskQueue;
};

#endif /* end of include guard: THREADPOOL_H */
