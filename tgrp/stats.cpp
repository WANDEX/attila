#include "stats.hpp"
#include "structs.hpp" // ss namespace with struct defs

#include <cstddef>   // size_t
#include <string>

#include <fmt/core.h>

const ss::stats_t calculate_stats(const ss::vtasks_t &vtt)
{
    std::size_t sum {0}; // total spent on all tasks in seconds
    std::size_t avg {0};
    std::size_t max {0};
    std::size_t min { vtt[0].hm_t.in_sec };
    const std::size_t nrecords { vtt.size() };

    std::size_t sec {0}; // total spent on task in seconds
    for (const auto &t : vtt) {
        sec = t.hm_t.in_sec;
        sum += sec;
        if (max < sec)
            max = sec;
        if (min > sec)
            min = sec;
    }
    avg = sum / nrecords;
    return { avg, max, min, sum, nrecords };
}

const ss::stats_human_t calculate_stats_human(const ss::stats_t &t)
{
    // convert size_t seconds into HH:MM spent time string
    auto hm = [&](const std::size_t sec) -> const std::string {
        return fmt::format("{:02}:{:02}", sec / 3600, sec % 3600 / 60);
    };
    return { hm(t.avg), hm(t.max), hm(t.min), hm(t.sum), t.nrecords };
}
