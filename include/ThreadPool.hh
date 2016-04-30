#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <utility>
#include <future>

#include "concurrentqueue.h"

using namespace std;
using namespace moodycamel;

typedef std::function<void()> WorkType;

class ThreadPool {
public:
        ThreadPool(uint32_t numthreads);
        ~ThreadPool();

        void Enqueue(function<void()> task);
        void Worker();
        void JoinAll();
        void Finish();

        template <typename T, typename... Params>
        void ParallelFor(uint32_t begin, uint32_t end, T SerialFunction, Params&&... params) {

                int chunk = (end - begin) / m_nthreads;
                for (int i = 0; i < m_nthreads; ++i) {
                        m_promises.emplace_back();
                        int mypromise = m_promises.size() - 1;
                        m_taskQueue.enqueue([=]{
                                        uint32_t threadstart = begin + i*chunk;
                                        uint32_t threadstop = (i == m_nthreads - 1) ? end : threadstart + chunk;
                                        for (uint32_t it = threadstart; it < threadstop; ++it) {
                                                SerialFunction(it, params...);
                                        }
                                        m_promises[mypromise].set_value();
                                });
                }
                WorkType work;
                if (m_taskQueue.try_dequeue(work)) work(); // master thread is also a worker
                Finish();
        }
        template<typename InputIt, typename T>
        void ParallelMap(InputIt begin, InputIt end, InputIt outputBegin, const std::function<T(T)>& func)
        {
            int chunkSize = (end - begin) / m_nthreads;
            for (int i = 0; i < m_nthreads; i++) {
                m_promises.emplace_back();
                int mypromise = m_promises.size() - 1;
                m_taskQueue.enqueue([=]{
                    InputIt threadBegin = begin + i*chunkSize;
                    InputIt threadOutput = outputBegin + i*chunkSize;
                    InputIt threadEnd = (i == m_nthreads - 1) ? end : threadBegin + chunkSize;
                    while (threadBegin != threadEnd) {
                        *(threadOutput++) = func(*(threadBegin++));
                    }
                    m_promises[mypromise].set_value();
                });
            }
            if (m_taskQueue.try_dequeue(work)) work(); // master thread is also a worker
            Finish();
        }

private:
        // threads and task queue
        int m_nthreads;
        vector<thread> m_workers;
        vector<promise<void>> m_promises;
        bool m_stopWorkers;

        ConcurrentQueue<WorkType> m_taskQueue;
};

#endif /* end of include guard: THREADPOOL_H */
