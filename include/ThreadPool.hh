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
#include <atomic>
#include <future>

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
                        m_promises.emplace_back();
                        int mypromise = m_promises.size() - 1;
                        Enqueue([=]{
                                        int threadstart = begin + i*chunk;
                                        int threadstop = (i == m_nthreads - 1) ? end : threadstart + chunk;
                                        for (int it = threadstart; it < threadstop; ++it) {
                                                ClassFunction::SerialFunction(it);
                                        }
                                        m_promises[mypromise].set_value();
                                });
                }
                Finish();
        }
        template <typename T, typename... Params>
        void ParallelFor(uint32_t begin, uint32_t end, T SerialFunction, Params&&... params) {

                int chunk = (end - begin) / m_nthreads;
                for (int i = 0; i < m_nthreads; ++i) {
                        m_promises.emplace_back();
                        int mypromise = m_promises.size() - 1;
                        Enqueue([=]{
                                        uint32_t threadstart = begin + i*chunk;
                                        uint32_t threadstop = (i == m_nthreads - 1) ? end : threadstart + chunk;
                                        for (uint32_t it = threadstart; it < threadstop; ++it) {
                                                SerialFunction(it, params...);
                                        }
                                        m_promises[mypromise].set_value();
                                });
                }
                Finish();
        }

        void Finish() {
                for (auto& promise : m_promises) promise.get_future().get();
                m_promises.clear();
        }

private:
        // threads and task queue
        int m_nthreads;
        vector<thread> m_workers;
        queue<function<void()>> m_tasks;
        vector<promise<void>> m_promises;

        // thread synchronization members
        bool stop_workers;
        condition_variable m_condition;
        mutable mutex m_taskmutex;
        atomic<int> m_nrunning;

};

// Macro to define a static class function which can be called via ThreadPool::ParallelFor<T>
#define SERIAL_OPERATION(name, function_kernal) class name { public: static void SerialFunction(const int& i) { function_kernal; } };
