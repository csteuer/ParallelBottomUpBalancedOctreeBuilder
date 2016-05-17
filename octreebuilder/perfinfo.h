#pragma once

#include "logging.h"
#include "octreebuilder_api.h"

#include <iosfwd>

#ifdef USE_PERFORMANCE_COUNTERS
#include <QElapsedTimer>
class OCTREEBUILDER_API PerfInfo {
public:
    PerfInfo();

    static PerfInfo createAndStart(int64_t taskNum = -1, size_t workSize = 0);

    static PerfInfo create(int64_t taskNum = -1, size_t workSize = 0);

    PerfInfo start();

    void resume();

    PerfInfo stop();

    void setWorkSize(const size_t& value);

    quint64 time() const;

    int thread_num() const;

    size_t workSize() const;

    int64_t taskNumber() const;

protected:
    PerfInfo(int64_t taskNum, size_t workSize);

    quint64 m_time;
    int m_thread_num;
    size_t m_workSize;
    int64_t m_taskNumber;
    QElapsedTimer m_timer;
};

OCTREEBUILDER_API std::ostream& operator<<(std::ostream& s, const PerfInfo& info);

#define LOG_PERF(x) LOG_INFO(x);
#define FOR_LOG_PERF(prefix, x)      \
    for (const PerfInfo& info : x) { \
        LOG_INFO(prefix << info);    \
    }
#define PERF(x) x;
#define START_PERF(x) x.start();
#define STOP_PERF(x) x.stop();
#define RESUME_PERF(x) x.resume();

#define NEW_PERF_COUNTER_3(name, taskNum, workSize) PerfInfo name = PerfInfo::create(taskNum, workSize);
#define NEW_PERF_COUNTER_2(name, taskNum) PerfInfo name = PerfInfo::create(taskNum);
#define NEW_PERF_COUNTER(name) PerfInfo name = PerfInfo::create();

#define START_NEW_PERF_COUNTER_3(name, taskNum, workSize) PerfInfo name = PerfInfo::createAndStart(taskNum, workSize);
#define START_NEW_PERF_COUNTER_2(name, taskNum) PerfInfo name = PerfInfo::createAndStart(taskNum);
#define START_NEW_PERF_COUNTER(name) PerfInfo name = PerfInfo::createAndStart();

#else
#define LOG_PERF(x)
#define FOR_LOG_PERF(prefix, x)
#define PERF(x)
#define START_PERF(x)
#define STOP_PERF(x)
#define RESUME_PERF(x)

#define NEW_PERF_COUNTER_3(name, taskNum, workSize)
#define NEW_PERF_COUNTER_2(name, taskNum)
#define NEW_PERF_COUNTER(name)

#define START_NEW_PERF_COUNTER_3(name, taskNum, workSize)
#define START_NEW_PERF_COUNTER_2(name, taskNum)
#define START_NEW_PERF_COUNTER(name)
#endif
