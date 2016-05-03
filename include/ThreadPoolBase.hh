#ifndef THREADPOOLBASE_H
#define THREADPOOLBASE_H

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <utility>
#include <future>

using namespace std;

typedef std::function<void()> WorkType;

class ThreadPoolBase {
public:
        ThreadPoolBase() : m_stopWorkers(false) {}

        virtual void AddTask(function<void()> task) = 0;

        virtual void Worker() = 0;
        virtual void CleanUp() { m_stopWorkers = true; }
        void JoinAll();
        void Finish();

        template <typename T, typename... Params>
        void ParallelFor(uint32_t begin, uint32_t end, uint32_t n_tasks, T SerialFunction, Params&&... params) {

                n_tasks = (n_tasks >= m_workers.size()) ? n_tasks : m_workers.size();
                m_tasksRemaining = n_tasks;
                int chunk = (end - begin) / n_tasks;
                for (auto i = 0u; i < n_tasks; ++i) {
                        AddTask([=]{
                                        uint32_t threadstart = begin + i*chunk;
                                        uint32_t threadstop = (i == n_tasks - 1) ? end : threadstart + chunk;
                                        for (uint32_t it = threadstart; it < threadstop; ++it) {
                                                SerialFunction(it, params...);
                                        }
                                        m_tasksRemaining--;
                                });
                }
                Finish();
        }

protected:
        // threads and task queue
        vector<thread> m_workers;
        atomic<int> m_tasksRemaining;
        bool m_stopWorkers;
};

#endif /* end of include guard: THREADPOOL_H */
