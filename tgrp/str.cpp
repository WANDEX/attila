#include <cstddef>   // size_t
#include <cstdlib>   // getenv

#include <fstream>
#include <iostream>
#include <sstream>

#include <regex>
#include <string>
#include <vector>
#include <algorithm> // remove_if etc

#include "str.hpp"

using namespace std;

/**
 * trim whitespace characters from right (also removes blank lines)
 */
string str::trim_right(const string &s)
{
    return regex_replace(s, regex("\\s+$"), "");
}

/**
 * trim whitespace characters from left
 */
string str::trim_left(const string &s)
{
    return regex_replace(s, regex("^\\s+"), "");
}

/**
 * trim whitespace characters from left & right (also removes blank lines)
 */
string str::trim(const string &s)
{
    return str::trim_left(str::trim_right(s));
}

bool str::has_substr(const string &s, const string &substr)
{
    return (s.find(substr) == string::npos) ? false : true;
}

/**
 * wrapper around std::getenv() to make it more safer
 *
 * details: https://www.delftstack.com/howto/cpp/cpp-get-environment-variables/
 */
string str::sane_getenv(const string &envar)
{
    const char *tmp = getenv(envar.c_str());
    string env_var(tmp ? tmp : "");
    if (env_var.empty()) {
        cerr << "[ERROR] '$" << envar << "' env var not found or empty." << '\n';
        exit(75); // XXX
    }
    // env variable value sanitization
    int pos = env_var.find(' ');
    if (pos != string::npos)
        env_var = env_var.substr(0, pos);
    return env_var;
}

/**
 * split string by regex
 */
vector<string> str::resplit(const string &s, const regex &re = regex{"\\s+"})
{
    sregex_token_iterator iter(s.begin(), s.end(), re, -1);
    sregex_token_iterator end;
    vector<string> v = {iter, end};
    auto isEmptyOrBlank = [](const string &tmps) {
        return tmps.find_first_not_of(" \t") == string::npos;
    }; // remove blank string elements from the vector
    v.erase(remove_if(v.begin(), v.end(), isEmptyOrBlank), v.end());
    return v;
}

vector<string> str::split_on_words(const string &s)
{
    regex sep_regex("[ [:punct:]]+", regex::extended);
    return str::resplit(s, sep_regex);
}

string str::file_content(const string &fpath)
{
    ifstream rfile(fpath, ios::in);
    string  content((istreambuf_iterator<char>(rfile)),
                    (istreambuf_iterator<char>()    ));
    return content;
}

/**
 * get slice of multiline string between line numbers
 */
const string str::lines_between(const vector<string> &lines, int beg_nl=0, int end_nl=-1)
{
    if (beg_nl < 0)
        beg_nl = 0;
    if (end_nl == -1 || end_nl > lines.size())
        end_nl = lines.size();
    ostringstream buf;
    for (int i = beg_nl; i < end_nl; i++)
        buf << lines[i] << '\n';
    return buf.str();
}

/**
 * find position of nearest newline in multiline str by substring
 * return std::string::npos if substring not found!
 */
const size_t str::fnl_substr(string &s, const string &substr, bool including_last=false)
{
    size_t pos;
    (including_last) ? pos = s.rfind(substr) : pos = s.find(substr);
    if (pos == string::npos)
        return string::npos; // substr not found!
    if (including_last) {
        // including LAST FOUND line with substring
        for (; pos < s.size(); pos++)
            if (s[pos] == '\n') break;
    } else {
        // excluding FIRST FOUND line with substring
        for (; pos > 0; pos--)
            if (s[pos] == '\n') break;
    }
    return pos;
}

/**
 * remove lines before line with substring
 * return false if substring not found, true on success.
 */
bool str::remove_lines_before(string &s, const string &substr, bool including_last=false)
{
    const size_t pos = str::fnl_substr(s, substr, including_last);
    if (pos == string::npos) return false; // substr not found
    s.replace(0, pos, "");
    return true;
}

/**
 * remove lines after line with substring
 * return false if substring not found, true on success.
 */
bool str::remove_lines_after(string &s, const string &substr, bool including_last=true)
{
    const size_t pos = str::fnl_substr(s, substr, including_last);
    if (pos == string::npos) return false; // substr not found
    s.replace(pos, string::npos, "");
    return true;
}

const string str::tasks_to_mulstr(ss::vtasks_t &tasks)
{
    std::ostringstream out;
    for (const auto &t : tasks) {
        out << t.dt << " <" << t.hm_t.str << "> " << t.text << '\n';
    }
    return out.str();
}
