#include "PerfMonitor.h"
#include <iostream>

PerfMonitor::PerfMonitor(size_t sampleSize) : m_sampleSize(sampleSize < 1 ? 1 : sampleSize) {}

size_t PerfMonitor::add(std::string monitorName, Unit unit)
{
    m_stopwatchs.push_back({});
    m_names.push_back(monitorName);
    m_units.push_back(unit);
    m_sampleTotals.push_back({});
    m_sampleSets.push_back({});

    return m_names.size() - 1;
}

double PerfMonitor::sample(size_t monitorId, std::function<void()> op)
{
    Stopwatch::TimeUnit unit = static_cast<Stopwatch::TimeUnit>(m_units[monitorId]);
    unit = !unit ? static_cast<Stopwatch::TimeUnit>(0) : unit;
    Stopwatch sw = m_stopwatchs[monitorId];
    sw.reset();
    op();

    double sample{ sw.tick(unit) };

    auto& total = m_sampleTotals[monitorId];
    auto& set = m_sampleSets[monitorId];
    total += sample;

    if (set.size() == m_sampleSize) {
        double oldest = set.front();
        total -= oldest;
        set.pop();
    }
    set.push(sample);

    return total / set.size();
}

void PerfMonitor::printMonitors(size_t precision)
{
    std::string buffer{};
    std::cout << std::setprecision(3);
    buffer += "\033[1;1H\033[37m";
    for (size_t i{}; i < m_names.size(); ++i) {
        double value = m_sampleTotals[i] / m_sampleSets[i].size();
        int fps = static_cast<int>(1.0 / value);
        buffer += m_names[i] + ":\t";

        switch (m_units[i]) {
        case Second:
            buffer += std::format("{:.{}}", value, precision) + "s\n";
            break;
        case Milli:
            buffer += std::format("{:.{}}", value, precision) + "ms\n";
            break;
        case Micro:
            buffer += std::format("{:.{}}", value, precision) + "\xC2\xB5s\n";
            break;
        case Nano:
            buffer += std::format("{:.{}}", value, precision) + "ns\n";
            break;
        case FPS:
            buffer += std::to_string(static_cast<int>(1.0 / value)) + '\n';
            break;
        }
    }
    std::cout << buffer;
}
