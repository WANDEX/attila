#pragma once

#include "wndx/attila/aliases.hpp"

#include "structs.hpp" // ss namespace with struct defs

namespace wndx::attila::ss {

stats_t       calculate_stats(vtasks_t const& vtt);
stats_human_t calculate_stats_human(stats_t const& stats_t);

std::pair<vtasks_t const, str_t> merge_tasks(vtasks_t const& vtt,
                                             str_t const&    mulstr);

sgroups_t auto_proj_groups(vtasks_t const& vtt);

} // namespace wndx::attila::ss
