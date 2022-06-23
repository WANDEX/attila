#ifndef STR_HPP
#define STR_HPP

#include <cstddef> // size_t
#include <regex>
#include <string>
#include <vector>

#include "structs.hpp" // ss namespace with struct defs

namespace str
{
    using namespace std;

    string trim_right(const string &s);
    string trim_left(const string &s);
    string trim(const string &s);

    bool has_substr(const string &s, const string &substr);

    string sane_getenv(const string &envar);

    vector<string> resplit(const string &s, const regex &re);
    vector<string> split_on_words(const string &s);

    string file_content(const string &fpath);

    const string lines_between(const vector<string> &lines, int beg_nl, int end_nl);

    const size_t  fnl_substr(string &s, const string &substr, bool including_last);
    bool remove_lines_before(string &s, const string &substr, bool including_last);
    bool remove_lines_after (string &s, const string &substr, bool including_last);

    const string tasks_to_mulstr(ss::vtasks_t &tasks);
}

#endif // STR_HPP
