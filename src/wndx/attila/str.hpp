#pragma once

#include "wndx/attila/aliases.hpp"

#include "structs.hpp"          // ss namespace with struct defs

#include <ctime>                // time_t
#include <regex>
#include <vector>

namespace wndx::attila::str {

using namespace std;

inline static const char*  datef      { "%Y-%m-%d" }; // date format
inline static const str_t  date_rs    { R"((\d{4}.\d\d.\d\d|\d\d.\d\d.\d{4}|\d\d.\d\d.\d\d))" };
inline static const str_t  time_rs    { R"(.*(\d\d:\d\d).*(\d\d:\d\d))" };
inline static const regex  dts_re     { str::date_rs + str::time_rs }; // date + time span regex
inline static const regex  dts_txt_re { "(^.*" + str::time_rs + ") (.*$)" }; // + task text

str_t trim_right(sv_t s);
str_t trim_left(sv_t s);
str_t trim(sv_t s);

bool has_substr(sv_t s, sv_t substr);

str_t sane_getenv(sv_t envar);

vec_str_t resplit(sv_t s, regex const &re);
vec_str_t split_on_words(sv_t s);

str_t file_content(sv_t fpath);

str_t lines_between(vec_str_t const &lines, sz_t beg_nl, sz_t end_nl);

sz_t          fnl_substr(str_t &s, sv_t substr, bool including_last);
bool remove_lines_before(str_t &s, sv_t substr, bool including_last);
bool remove_lines_after (str_t &s, sv_t substr, bool including_last);

str_t sec_to_tstr(std::time_t const &sec);
str_t tasks_to_mulstr(ss::vtasks_t &tasks);

} // namespace wndx::attila::str
