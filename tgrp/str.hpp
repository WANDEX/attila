#ifndef STR_HPP
#define STR_HPP

#include <regex>
#include <string>
#include <vector>

namespace str
{
    using namespace std;

    string trim_right(const string &s);
    string trim_left(const string &s);
    string trim(const string &s);

    string sane_getenv(const string &envar);

    vector<string> resplit(const string &s, const regex &re);
    vector<string> split_on_words(const string &s);

    string file_content(const string &fpath);

    const string lines_between(const vector<string> &lines, int beg_nl, int end_nl);

    size_t        fnl_substr(string &s, const string &substr, bool including_last);
    bool remove_lines_before(string &s, const string &substr, bool including_last);
    bool remove_lines_after (string &s, const string &substr, bool including_last);
}

#endif // STR_HPP
