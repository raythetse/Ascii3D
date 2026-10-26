#pragma once
#include <functional>
#include <queue>
#include <string>
#include <vector>
#include "Stopwatch.h"

// Allows basic tracking of lamda execution times for performance monitoring
class PerfMonitor
{
public:
    enum TimeUnit {
        FPS = -1,
        Second,
        Milli,
        Micro,
        Nano,
    };
private:
    std::vector<Stopwatch> m_stopwatchs;
    std::vector<std::string> m_names;
    std::vector<TimeUnit> m_units;
    std::vector<double> m_sampleTotals;
    std::vector<std::queue<double>> m_sampleSets;
    size_t m_sampleSize;
public:
    PerfMonitor(size_t sampleSize = 100);
    size_t add(std::string monitorName, TimeUnit unit = Milli);
    double sample(size_t monitorId, std::function<void()> op);
    void printMonitors(size_t precision = 3);
};

