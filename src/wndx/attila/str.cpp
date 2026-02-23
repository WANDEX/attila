#include "str.hpp"

#include <fmt/core.h>

#include <algorithm>            // remove_if etc
#include <cstdlib>              // getenv
#include <fstream>
#include <sstream>              // ostringstream

namespace wndx::str {

using namespace std;

/**
 * trim whitespace characters from right (also removes blank lines)
 */
str_t trim_right(sv_t s)
{
    return std::regex_replace(s.data(), regex("\\s+$"), "");
}

/**
 * trim whitespace characters from left
 */
str_t trim_left(sv_t s)
{
    return std::regex_replace(s.data(), regex("^\\s+"), "");
}

/**
 * trim whitespace characters from left & right (also removes blank lines)
 */
str_t trim(sv_t s)
{
    return trim_left(trim_right(s));
}

bool has_substr(sv_t s, sv_t substr)
{
    return (s.find(substr) == str_t::npos) ? false : true;
}

/**
 * wrapper around std::getenv() to make it more safer
 *
 * details: https://www.delftstack.com/howto/cpp/cpp-get-environment-variables/
 */
str_t sane_getenv(sv_t envar)
{
    const char *tmp = getenv(envar.data());
    str_t env_var(tmp ? tmp : "");
    if (env_var.empty()) {
        WNDX_LOG(LL::ERRO, "'${}' ENV VAR not found or empty.\n", envar);
        exit(75); // XXX
    }
    // env variable value sanitization
    auto const pos{ env_var.find(' ') };
    if (pos != str_t::npos)
        env_var = env_var.substr(0, pos);
    return env_var;
}

/**
 * split str_t by regex
 */
vec_str_t resplit(sv_t s, regex const &re = regex{"\\s+"})
{
    regex_token_iterator iter(s.begin(), s.end(), re, -1);
    // sregex_token_iterator end;
    vec_str_t v({ iter, {} });
    auto isEmptyOrBlank = [](sv_t tmps) {
        return tmps.find_first_not_of(" \t") == str_t::npos;
    }; // remove blank str_t elements from the vector
    v.erase(remove_if(v.begin(), v.end(), isEmptyOrBlank), v.end());
    return v;
}

vec_str_t split_on_words(sv_t s)
{
    regex sep_regex("[[:punct:]]+", regex::extended);
    return resplit(s, sep_regex);
}

str_t file_content(sv_t fpath)
{
    ifstream rfile(fpath.data(), ios::in);
    str_t content((istreambuf_iterator<ch_t>(rfile)), (istreambuf_iterator<ch_t>()));
    return content;
}

/**
 * get slice of multiline str_t between line numbers
 */
str_t lines_between(vec_str_t const &lines, sz_t beg_nl, sz_t end_nl)
{
    if (end_nl == 0 || end_nl > lines.size())
        end_nl = lines.size();
    std::ostringstream buf;
    for (sz_t i = beg_nl; i < end_nl; i++)
        buf << lines[i] << '\n';
    return buf.str();
}

/**
 * find position of nearest newline in multiline str by substring
 * return str_t::npos if substr_t not found!
 */
size_t fnl_substr(str_t &s, sv_t substr, bool including_last=false)
{
    size_t pos;
    (including_last) ? pos = s.rfind(substr) : pos = s.find(substr);
    if (pos == str_t::npos)
        return str_t::npos; // substr not found!
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
 * return false if substr_t not found, true on success.
 */
bool remove_lines_before(str_t &s, sv_t substr, bool including_last=false)
{
    const size_t pos = fnl_substr(s, substr, including_last);
    if (pos == str_t::npos) return false; // substr not found
    s.replace(0, pos, "");
    return true;
}

/**
 * remove lines after line with substring
 * return false if substr_t not found, true on success.
 */
bool remove_lines_after(str_t &s, sv_t substr, bool including_last=true)
{
    const size_t pos = fnl_substr(s, substr, including_last);
    if (pos == str_t::npos) return false; // substr not found
    s.replace(pos, str_t::npos, "");
    return true;
}

str_t sec_to_tstr(std::time_t const &sec)
{
    return fmt::format("{:02}:{:02}", sec / 3600, sec % 3600 / 60);
}

str_t tasks_to_mulstr(ss::vtasks_t &tasks)
{
    std::ostringstream out;
    for (const auto &t : tasks) {
        out << t.dts << " <" << t.hm.time_spent << "> " << t.text << '\n';
    }
    return out.str();
}

} // namespace wndx::str
