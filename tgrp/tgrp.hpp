#ifndef TGRP_HPP
#define TGRP_HPP

#include <filesystem>
#include <regex>
#include <string>
#include <vector>

#include "structs.hpp" // ss namespace with struct defs

std::vector<int> split_vi(const std::string &s, char delimiter);
int item_index(const std::vector<std::string> &v, const std::string &item);
std::vector<std::string> vslice(const std::vector<std::string> &v, int start, int end);

const ss::hm_t calculate_time_spent(const std::string &fr, const std::string &to);
const ss::hm_t time_spent(const std::string &s);
std::pair<std::string, std::string> dt_and_task(const std::string &s);
std::vector<std::string> projects_of_task(const std::string &s);

ss::vtasks_t parse_tasks(const std::string &s);
ss::vtasks_t parse_tasks_parallel(const std::string &s);

std::string concat_span(const std::string &fr, const std::string &to);
const std::string concat_week_files(std::vector<std::string> &fpaths,
                                    const std::string &fr, const std::string &to);
std::vector<std::string> dates_of_week(const std::string &date_str);
std::string filter_find(const std::string &s, const std::string &reinput);

std::vector<std::string> get_all_files_recursive(const std::filesystem::path &path);
std::vector<std::string> find_week_files(const std::string &pmatch);
std::vector<std::string> find_week_files_in_span(const std::string &fr, const std::string &to);

std::string week_file_name(const std::string &date_str);
std::string find_week_file_by_date(const std::string &date_str);
std::string find_last_week_file();

bool remove_lines_after_date (std::string &s, const std::string &date_str);
bool remove_lines_before_date(std::string &s, const std::string &date_str);

#endif // TGRP_HPP
