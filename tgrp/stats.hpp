#ifndef STATS_HPP
#define STATS_HPP

#include "structs.hpp" // ss namespace with struct defs

const ss::stats_t       calculate_stats(const ss::vtasks_t &vtt);
const ss::stats_human_t calculate_stats_human(const ss::stats_t &stats_t);

std::pair<const ss::vtasks_t, const std::string>
    merge_tasks(const ss::vtasks_t &vtt, const std::string &mulstr);

#endif // STATS_HPP
