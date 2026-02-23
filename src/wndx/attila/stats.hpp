#pragma once

#include "wndx/attila/aliases.hpp"

#include "structs.hpp"          // ss namespace with struct defs

namespace wndx::attila::ss {

stats_t       calculate_stats(const vtasks_t &vtt);
stats_human_t calculate_stats_human(const stats_t &stats_t);

std::pair<const vtasks_t, str_t>
merge_tasks(const vtasks_t &vtt, const str_t &mulstr);

sgroups_t auto_proj_groups(const vtasks_t &vtt);

} // namespace wndx::attila::ss
