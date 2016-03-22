#include <thread>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>

using namespace std;

class ThreadPool {
public:
        ThreadPool(uint32_t numthreads);
        ~ThreadPool();

        void Enqueue(function<void()> task);
        void Worker();
        void JoinAll();

        template <typename ClassFunction>
        void ParallelFor(uint32_t begin, uint32_t end) {

                int chunk = (end - begin) / m_nthreads;
                for (int i = 0; i < m_nthreads; ++i) {
                        Enqueue([=]{
                                        int threadstart = begin + i*chunk;
                                        int threadstop = (i == m_nthreads - 1) ? end : threadstart + chunk;
                                        for (int it = threadstart; it < threadstop; ++it) {
                                                ClassFunction::Serial(it);
                                        }
                                });
                }
        }
        template <typename T, typename... Params>
        void ParallelFor(uint32_t begin, uint32_t end, T func, Params&&... params) {

                int chunk = (end - begin) / m_nthreads;
                for (int i = 0; i < m_nthreads; ++i) {
                        Enqueue([=]{
                                        uint32_t threadstart = begin + i*chunk;
                                        uint32_t threadstop = (i == m_nthreads - 1) ? end : threadstart + chunk;
                                        for (uint32_t it = threadstart; it < threadstop; ++it) {
                                                func(it, params...);
                                        }
                                });
                }
        }

private:
        // threads and task queue
        int m_nthreads;
        vector<thread> m_workers;
        queue<function<void()>> m_tasks;

        // thread synchronization members
        bool stop_workers;
        condition_variable m_condition;
        mutable mutex m_taskmutex;

};

// Macro to define a static class function which can be called via ThreadPool::ParallelFor<T>
#define SERIAL_OPERATION(name, function_kernal) class name { public: static void Serial(const int& i) { function_kernal; } };
