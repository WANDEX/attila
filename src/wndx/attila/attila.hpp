#pragma once

#include "wndx/attila/aliases.hpp"

#include "structs.hpp" // ss namespace with struct defs

#include <vector>

namespace wndx::attila {

std::vector<int> split_vi(str_v s, ch_t delimiter);
sz_t             item_index(vec_str_t const& v, str_v item);
vec_str_t        vslice(vec_str_t const& v, sz_t start, sz_t end);

ss::hm_t calculate_time_spent(str_t d_fr, str_t d_to, str_t t_fr, str_t t_to);

ss::hm_t  time_spent(str_v s);
auto      dts_and_task(str_v s) -> std::pair<str_t, str_t>;
vec_str_t projects_of_task(str_v s);

ss::vtasks_t parse_tasks(str_v s);
ss::vtasks_t parse_tasks_parallel(str_v s);

str_t tasks_to_mulstr(ss::vtasks_t& tasks);

str_t     concat_span(str_v fr, str_v to);
str_t     concat_week_files(vec_str_t& fpaths, str_v fr, str_v to);
vec_str_t dates_of_week(str_v date_str);
str_t     filter_find(str_v s, str_v reinput);

auto get_all_files_recursive(fs::path const& path);
auto find_week_files(str_v pmatch) -> vec_str_t;
auto find_week_files_in_span(str_v fr, str_v to);

str_t week_file_name(str_v date_str);
str_t find_week_file_by_date(str_v date_str);
str_t find_last_week_file();

bool remove_lines_after_date(str_t& s, str_v date_str);
bool remove_lines_before_date(str_t& s, str_v date_str);


} // namespace wndx::attila
