#include "perfinfo.h"

#include <ostream>
#include <omp.h>

#ifdef USE_PERFORMANCE_COUNTERS
PerfInfo::PerfInfo(int64_t taskNum, size_t workSize) : m_time(0), m_thread_num(-1), m_workSize(workSize), m_taskNumber(taskNum) {
}

PerfInfo::PerfInfo() {
}

PerfInfo PerfInfo::createAndStart(int64_t taskNum, size_t workSize) {
    PerfInfo p(taskNum, workSize);
    return p.start();
}

PerfInfo PerfInfo::create(int64_t taskNum, size_t workSize) {
    return PerfInfo(taskNum, workSize);
}

PerfInfo PerfInfo::start() {
    m_time = 0;
    m_thread_num = omp_in_parallel() ? omp_get_thread_num() : -1;
    m_timer.start();

    return *this;
}

void PerfInfo::resume() {
    m_thread_num = omp_in_parallel() ? omp_get_thread_num() : -1;
    m_timer.start();
}

PerfInfo PerfInfo::stop() {
    m_time += m_timer.elapsed();
    return *this;
}

void PerfInfo::setWorkSize(const size_t& value) {
    m_workSize = value;
}

quint64 PerfInfo::time() const {
    return m_time;
}

int PerfInfo::thread_num() const {
    return m_thread_num;
}

size_t PerfInfo::workSize() const {
    return m_workSize;
}

int64_t PerfInfo::taskNumber() const {
    return m_taskNumber;
}

std::ostream& operator<<(std::ostream& s, const PerfInfo& info) {
    if (info.taskNumber() > -1) {
        s << "Task " << info.taskNumber() << " - Thread: " << info.thread_num() << ", WorkSize: " << info.workSize() << ", Time: " << info.time() << " ms";
    } else if (info.thread_num() > -1) {
        s << "Thread: " << info.thread_num() << ", WorkSize: " << info.workSize() << ", Time: " << info.time() << " ms";
    } else if (info.workSize() > 0) {
        s << "WorkSize: " << info.workSize() << ", Time: " << info.time() << " ms";
    } else {
        s << "Time: " << info.time() << " ms";
    }

    return s;
}
#endif
