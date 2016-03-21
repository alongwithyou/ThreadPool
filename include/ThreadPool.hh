#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <chrono>
#include <functional>
#include <vector>

#include "ThreadsafeQueue.hh"

using namespace std;

typedef std::function<void()> WorkType;

class ThreadPool {
public:
        ThreadPool(uint32_t numthreads);
        ~ThreadPool();

        void Worker();
        void JoinAll();

        template <typename ClassFunction>
        void ParallelFor(uint32_t begin, uint32_t end) {

                int chunk = (end - begin) / m_nthreads;
                for (int i = 0; i < m_nthreads; ++i) {
                        m_taskQueue.push([=]{
                                int threadstart = begin + i*chunk;
                                int threadstop = (i == m_nthreads - 1) ? end : threadstart + chunk;
                                for (int it = threadstart; it < threadstop; ++it) {
                                        ClassFunction::Serial(it);
                                }
                        });
                }
        }

        void MapRange(const function<void(int)>& func, const size_t begin, const size_t end);

private:
        // threads and task queue
        int m_nthreads;
        vector<thread> m_workers;
        bool m_stopWorkers;

        ThreadsafeQueue<WorkType> m_taskQueue;
};

// Macro to define a static class function which can be called via ThreadPool::ParallelFor<T>
#define SERIAL_OPERATION(name, function_kernel) class name { public: static void Serial(const int& i) { function_kernel; } };

#endif /* end of include guard: THREADPOOL_H */
