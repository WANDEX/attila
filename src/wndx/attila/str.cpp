#include "str.hpp"

#include <fmt/core.h>

#include <algorithm>            // remove_if etc
#include <cstdlib>              // getenv
#include <fstream>

namespace wndx::str {

using namespace std;

/**
 * trim whitespace characters from right (also removes blank lines)
 */
string trim_right(sv_t s)
{
    return std::regex_replace(s.data(), regex("\\s+$"), "");
}

/**
 * trim whitespace characters from left
 */
string trim_left(sv_t s)
{
    return std::regex_replace(s.data(), regex("^\\s+"), "");
}

/**
 * trim whitespace characters from left & right (also removes blank lines)
 */
string trim(sv_t s)
{
    return trim_left(trim_right(s));
}

bool has_substr(sv_t s, sv_t substr)
{
    return (s.find(substr) == string::npos) ? false : true;
}

/**
 * wrapper around std::getenv() to make it more safer
 *
 * details: https://www.delftstack.com/howto/cpp/cpp-get-environment-variables/
 */
string sane_getenv(sv_t envar)
{
    const char *tmp = getenv(envar.data());
    string env_var(tmp ? tmp : "");
    if (env_var.empty()) {
        WNDX_LOG(LL::ERRO, "'${}' ENV VAR not found or empty.\n", envar);
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
vector<string> resplit(sv_t s, const regex &re = regex{"\\s+"})
{
    regex_token_iterator iter(s.begin(), s.end(), re, -1);
    // sregex_token_iterator end;
    // vector<string> v = {iter, end};
    vector<string> v({ iter, {} });
    auto isEmptyOrBlank = [](sv_t tmps) {
        return tmps.find_first_not_of(" \t") == string::npos;
    }; // remove blank string elements from the vector
    v.erase(remove_if(v.begin(), v.end(), isEmptyOrBlank), v.end());
    return v;
}

vector<string> split_on_words(sv_t s)
{
    regex sep_regex("[ [:punct:]]+", regex::extended);
    return resplit(s, sep_regex);
}

string file_content(sv_t fpath)
{
    ifstream rfile(fpath.data(), ios::in);
    string  content((istreambuf_iterator<char>(rfile)),
                    (istreambuf_iterator<char>()    ));
    return content;
}

/**
 * get slice of multiline string between line numbers
 */
string lines_between(const vector<string> &lines, int beg_nl=0, int end_nl=-1)
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
 * return str_t::npos if substring not found!
 */
size_t fnl_substr(string &s, sv_t substr, bool including_last=false)
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
bool remove_lines_before(string &s, sv_t substr, bool including_last=false)
{
    const size_t pos = fnl_substr(s, substr, including_last);
    if (pos == string::npos) return false; // substr not found
    s.replace(0, pos, "");
    return true;
}

/**
 * remove lines after line with substring
 * return false if substring not found, true on success.
 */
bool remove_lines_after(string &s, sv_t substr, bool including_last=true)
{
    const size_t pos = fnl_substr(s, substr, including_last);
    if (pos == string::npos) return false; // substr not found
    s.replace(pos, string::npos, "");
    return true;
}

string sec_to_tstr(const std::time_t &sec)
{
    return fmt::format("{:02}:{:02}", sec / 3600, sec % 3600 / 60);
}

string tasks_to_mulstr(ss::vtasks_t &tasks)
{
    std::ostringstream out;
    for (const auto &t : tasks) {
        out << t.dts << " <" << t.hm.time_spent << "> " << t.text << '\n';
    }
    return out.str();
}

} // namespace wndx::str
