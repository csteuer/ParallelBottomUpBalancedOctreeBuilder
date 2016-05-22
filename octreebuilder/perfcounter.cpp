#include "perfcounter.h"

#include <iostream>

PerfCounter::PerfCounter()
    : m_start(std::chrono::high_resolution_clock::time_point::max()), m_sumDurations(std::chrono::high_resolution_clock::duration::zero()), m_stopped(true) {
}

void PerfCounter::start() {
    m_stopped = false;
    m_sumDurations = std::chrono::high_resolution_clock::duration::zero();
    m_start = std::chrono::high_resolution_clock::now();
}

std::chrono::high_resolution_clock::duration PerfCounter::stop() {
    if (!m_stopped) {
        m_sumDurations += std::chrono::high_resolution_clock::now() - m_start;
        m_stopped = true;
    }
    return m_sumDurations;
}

void PerfCounter::resume() {
    if (m_stopped) {
        m_stopped = false;
        m_start = std::chrono::high_resolution_clock::now();
    }
}

std::chrono::high_resolution_clock::duration PerfCounter::elapsedTime() const {
    if (m_stopped) {
        return m_sumDurations;
    }

    return m_sumDurations + std::chrono::high_resolution_clock::now() - m_start;
}

std::ostream& operator<<(std::ostream& os, const PerfCounter& pc) {
    std::chrono::high_resolution_clock::duration elapsed = pc.elapsedTime();

    os << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << " ms";
    return os;
}
