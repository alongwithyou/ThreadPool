#include "ThreadPoolBase.hh"

void ThreadPoolBase::JoinAll() {
    for (auto& worker : m_workers) { worker.join(); }
}
void ThreadPoolBase::Finish() {
    while (m_tasksRemaining > 0) {}
}
