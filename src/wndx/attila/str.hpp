#pragma once

#include "wndx/attila/aliases.hpp"

#include "structs.hpp"          // ss namespace with struct defs

#include <ctime>                // time_t
#include <regex>
#include <vector>

namespace wndx::str {

using namespace std;

inline static const char*  datef      { "%Y-%m-%d" }; // date format
inline static const string date_rs    { R"((\d{4}.\d\d.\d\d|\d\d.\d\d.\d{4}|\d\d.\d\d.\d\d))" };
inline static const string time_rs    { R"(.*(\d\d:\d\d).*(\d\d:\d\d))" };
inline static const regex  dts_re     { str::date_rs + str::time_rs }; // date + time span regex
inline static const regex  dts_txt_re { "(^.*" + str::time_rs + ") (.*$)" }; // + task text

string trim_right(sv_t s);
string trim_left(sv_t s);
string trim(sv_t s);

bool has_substr(sv_t s, sv_t substr);

string sane_getenv(sv_t envar);

vector<string> resplit(sv_t s, const regex &re);
vector<string> split_on_words(sv_t s);

string file_content(sv_t fpath);

string lines_between(const vector<string> &lines, sz_t beg_nl, sz_t end_nl);

sz_t          fnl_substr(string &s, sv_t substr, bool including_last);
bool remove_lines_before(string &s, sv_t substr, bool including_last);
bool remove_lines_after (string &s, sv_t substr, bool including_last);

string sec_to_tstr(const std::time_t &sec);
string tasks_to_mulstr(ss::vtasks_t &tasks);

} // namespace wndx::str
