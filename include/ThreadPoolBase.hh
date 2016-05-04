#ifndef THREADPOOLBASE_H
#define THREADPOOLBASE_H

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <utility>
#include <future>
#include "move_on_copy.hh"

using namespace std;

typedef std::function<void()> WorkType;

class ThreadPoolBase {
public:
        ThreadPoolBase() : m_stopWorkers(false) {}

        virtual void AddTask(function<void()> task) = 0;

        virtual void Worker() = 0;
        virtual void CleanUp() { m_stopWorkers = true; }
        void JoinAll();

        template <typename T, typename... Params>
        void ParallelFor(uint32_t begin, uint32_t end, uint32_t n_tasks, T SerialFunction, Params&&... params) {

                n_tasks = (n_tasks >= m_workers.size()) ? n_tasks : m_workers.size();
                std::future<void> futures[n_tasks];
                int chunk = (end - begin) / n_tasks;
                for (auto i = 0u; i < n_tasks; ++i) {
                        std::promise<void> promise;
                        futures[i] = promise.get_future();
                        auto move_promise = make_move_on_copy(move(promise));
                        AddTask([=] () mutable {
                                        uint32_t threadstart = begin + i*chunk;
                                        uint32_t threadstop = (i == n_tasks - 1) ? end : threadstart + chunk;
                                        for (uint32_t it = threadstart; it < threadstop; ++it) {
                                                SerialFunction(it, params...);
                                        }
                                        move_promise.value().set_value();
                                });
                }
                for (auto i = 0u; i<n_tasks; i++) {
                        futures[i].get();
                }
        }

protected:
        // threads and task queue
        vector<thread> m_workers;
        bool m_stopWorkers;
};

#endif /* end of include guard: THREADPOOL_H */
