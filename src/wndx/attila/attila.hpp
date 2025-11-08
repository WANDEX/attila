#pragma once

#include "aliases.hpp"

#include "structs.hpp"          // ss namespace with struct defs

#include <vector>

namespace wndx {

std::vector<int> split_vi(str_v s, ch_t delimiter);
int item_index(vec_str_t const &v, str_v item);
vec_str_t vslice(vec_str_t const &v, int start, int end);

// ss::hm_t calculate_time_spent(str_v d_fr, str_v d_to, str_v t_fr, str_v t_to);
ss::hm_t calculate_time_spent(str_t d_fr, str_t d_to, str_t t_fr, str_t t_to);

ss::hm_t time_spent(str_v s);
std::pair<str_t, str_t> dts_and_task(str_v s);
vec_str_t projects_of_task(str_v s);

ss::vtasks_t parse_tasks(str_v s);
ss::vtasks_t parse_tasks_parallel(str_v s);

str_t concat_span(str_v fr, str_v to);
str_t concat_week_files(vec_str_t &fpaths, str_v fr, str_v to);
vec_str_t dates_of_week(str_v date_str);
str_t filter_find(str_v s, str_v reinput);

auto get_all_files_recursive(fs::path const &path);
auto find_week_files(str_v pmatch) -> vec_str_t;
auto find_week_files_in_span(str_v fr, str_v to);

str_t week_file_name(str_v date_str);
str_t find_week_file_by_date(str_v date_str);
str_t find_last_week_file();

bool remove_lines_after_date (str_t &s, str_v date_str);
bool remove_lines_before_date(str_t &s, str_v date_str);

} // namespace wndx
