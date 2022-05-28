#ifndef TGRP_H
#define TGRP_H

#include <cmath>    // floor
#include <cstdlib>  // getenv
#include <ctime>    // difftime, mktime

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <locale>
#include <iomanip>  // put_time

#include <fmt/core.h>
#include <fmt/format.h> // fmt::join

namespace fs = std::filesystem;

struct Task {
    std::string dt;
    std::string text;
    std::string spent;
    std::vector<std::string> words;
    std::vector<std::string> tproj;
};

inline std::string sane_getenv(const std::string &env)
{   // details: https://www.delftstack.com/howto/cpp/cpp-get-environment-variables/
    const char *tmp = std::getenv(env.c_str());
    std::string env_var(tmp ? tmp : "");
    if (env_var.empty()) {
        std::cerr << "[ERROR] '$" << env << "' env var not found or empty." << '\n';
        exit(75); // XXX
    }
    // env variable value sanitization
    int pos = env_var.find(' ');
    if (pos != std::string::npos)
        env_var = env_var.substr(0, pos);
    return env_var;
}

inline std::vector<int> split_vi(const std::string &s, char delimiter)
{
    std::vector<int> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(std::stoi(token));
    }
    return tokens;
}

inline std::vector<std::string> resplit(const std::string &s, const std::regex &re = std::regex{"\\s+"})
{
    std::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> v = {iter, end};
    auto isEmptyOrBlank = [](const std::string &tmps) {
        return tmps.find_first_not_of(" \t") == std::string::npos;
    }; // remove blank string elements from the vector
    v.erase(std::remove_if(v.begin(), v.end(), isEmptyOrBlank), v.end());
    return v;
}

inline std::vector<std::string> split_on_words(const std::string &s)
{
    std::regex sep_regex("[ [:punct:]]+", std::regex::extended);
    return resplit(s, sep_regex);
}

inline std::string calculate_time_spent(const std::string &fr, const std::string &to)
{
    std::vector<int> fr_res = split_vi(fr, ':');
    std::vector<int> to_res = split_vi(to, ':');

#if 0
    fmt::print("fr:{},{}\n", fr_res[0], fr_res[1]);
    fmt::print("to:{},{}\n", to_res[0], to_res[1]);
#endif

    std::tm t1 = {};
    t1.tm_hour = fr_res[0];
    t1.tm_min  = fr_res[1];
    std::time_t beg = std::mktime(&t1);

    std::tm t2 = {};
    t2.tm_hour = to_res[0];
    t2.tm_min  = to_res[1];
    std::time_t end = std::mktime(&t2);

    int sec_diff = std::floor(std::difftime(end, beg));
    if (sec_diff == 0) return "00:00";
    // fix: 23:53 -> 00:07 expected time spent: (00:14)
    // recalculate if the task was ended the next day
    if (sec_diff < 1) {
        t2.tm_mday = t2.tm_mday + 1;
        end = std::mktime(&t2);
        sec_diff = std::floor(std::difftime(end, beg));
    }
    int h = sec_diff / 3600;
    int m = sec_diff % 3600 / 60;
    std::string spent = fmt::format("{:02}:{:02}", h, m);
    return spent;
}

inline std::string time_spent(const std::string &s)
{
    const std::regex r{R"((\d\d:\d\d).*(\d\d:\d\d))"}; // time span: from - to (hh:mm)
    std::smatch m;
    if(!std::regex_search(s, m, r)) {
        std::cerr << "time span was not found in the string:" << '\n' << s << '\n';
        exit(77); // XXX
    }
    return calculate_time_spent(m[1], m[2]);
}

inline std::string file_content(const std::string &file_path)
{
    std::ifstream rfile(file_path, std::ios::in);
    std::string content((std::istreambuf_iterator<char>(rfile)),
                        (std::istreambuf_iterator<char>()    ));
    return content;
}

inline void file_time_spent(const std::string &file_path)
{
    std::string line;
    std::istringstream fc(file_content(file_path));
    while (std::getline(fc, line)) {
        std::cout << time_spent(line) << std::endl;
    }
}

inline std::pair<std::string, std::string> dt_and_task(const std::string &s)
{
    const std::regex r{R"((^.*\d\d:\d\d.*\d\d:\d\d) (.*$))"}; // time span: from - to (hh:mm)
    std::smatch m;
    if(!std::regex_search(s, m, r)) {
        std::cerr << "time span was not found in the string:" << '\n' << s << '\n';
        exit(76); // XXX
    }
    return std::make_pair(m[1], m[2]);
}

inline std::vector<std::string> projects_of_task(const std::string &s)
{
    const std::regex projects{R"((\[.*\]))"};
    std::smatch m;
    if(!std::regex_search(s, m, projects))
        return {};
    const std::regex re{R"(([\[\]]))"};
    return resplit(m.str(), re); // [nvim][lsp] -> nvim lsp
}

inline std::vector<Task> file_tasks(const std::string &file_path)
{
    std::vector<Task> tasks;
    std::string line;
    std::istringstream fc(file_content(file_path));
    while (std::getline(fc, line)) {
        std::pair<std::string, std::string> dt_text = dt_and_task(line);
        std::string& dt = dt_text.first;
        std::string& text = dt_text.second;
        std::string spent = time_spent(dt);
        std::vector<std::string> words = split_on_words(text);
        std::vector<std::string> tproj = projects_of_task(text);
        Task task = {dt, text, spent, words, tproj};
        tasks.push_back(task);
    }
#if 0
    for (const auto &t : tasks) {
        std::cout << std::endl
            << t.dt << std::endl
            << t.text << std::endl
            << t.spent << std::endl;
        fmt::print("[{}]\n", fmt::join(t.words, ", "));
        if (!t.tproj.empty())
            fmt::print("> tproj: {}\n", fmt::join(t.tproj, ", "));
    }
#endif
    return tasks;
}

inline std::string tasks_to_mulstr(std::vector<Task> tasks)
{
    std::ostringstream out;
    for (const auto &t : tasks) {
        out << t.text << '\n';
    }
    return out.str();
}

inline std::vector<std::string> get_all_files_recursive(const fs::path &path)
{
    std::vector<std::string> fpaths;
    for (const auto& p : fs::recursive_directory_iterator(path)) {
        if (!fs::is_directory(p)) {
            fs::path path = p.path();
            fpaths.push_back(path.u8string());
        }
    }
    std::sort(fpaths.begin(), fpaths.end());
#if 0
    for (const auto &p : fpaths) {
        std::cout << p <<  std::endl;
    }
#endif
    return fpaths;
}

inline std::vector<std::string> find_week_files(const std::string &pmatch = "week-")
{
    std::string POMODORO_DIR = sane_getenv("POMODORO_DIR");
    std::vector<std::string> fpaths = get_all_files_recursive(POMODORO_DIR);
    std::vector<std::string>& v = fpaths; // reference for shortness
    auto match = [=](const std::string &tmps) {
        return tmps.find(pmatch) == std::string::npos;
    }; // remove all paths which does not include pattern match
    v.erase(std::remove_if(v.begin(), v.end(), match), v.end());
    if (v.empty())
        return {};
#if 0
    for (const auto &p : fpaths) {
        std::cout << p <<  std::endl;
    }
#endif
    return fpaths;
}

/*
    construct & return week file name by the date string
*/
inline std::string week_file_name(const std::string &date_str)
{
    std::tm tm1 = {}, tm2 = {};
    const char* wfmt = "week-%V-%Y.txt";
    const std::time_t now = std::time(nullptr);
    tm1 = *std::localtime(&now);
    std::ostringstream buf;
    if (date_str == "now") {
        buf << std::put_time(&tm1, wfmt);
        return buf.str();
    }
    std::istringstream ss(date_str);
    ss.imbue(std::locale("en_US.utf-8"));
    ss >> std::get_time(&tm2, "%Y-%m-%d");
    const std::time_t t = std::mktime(&tm2);
    // if date str > date now => current week fname
    if (std::difftime(now, t) < 0)
        buf << std::put_time(&tm1, wfmt);
    else
        buf << std::put_time(&tm2, wfmt);
    return buf.str();
}

/*
    vector slice by indexes, like the list slicing in python
*/
inline std::vector<std::string> vslice(const std::vector<std::string> &v, int start=0, int end=-1)
{
    int oldlen = v.size();
    int newlen;
    if (end == -1 or end >= oldlen) {
        newlen = oldlen - start;
    } else {
        newlen = end - start;
    }
    std::vector<std::string> nv(newlen);
    for (int i=0; i<newlen; i++) {
        nv[i] = v[start + i];
    }
    return nv;
}

inline int item_index(const std::vector<std::string> &v, const std::string &item)
{
    auto ret = std::find(v.begin(), v.end(), item);
    if (ret != v.end())
        return ret - v.begin();
    return -1; // return the last element index
}

inline std::string find_week_file_by_date(const std::string &date_str)
{
    const std::vector<std::string> found = find_week_files(week_file_name(date_str));
    if (found.empty()) { // find closest next found week file
        const std::vector<std::string> fpaths = find_week_files();
        const std::string fake_fname = week_file_name(date_str);
        std::vector<std::string> fnames;
        fnames.push_back(fake_fname); // add fake entry week fname
        for (const fs::path p : fpaths)
            fnames.push_back(p.filename());
        // fname example: week-05-2022.txt
        // substr(8, 4) = year; substr(5, 2) = week_num
        std::sort(fnames.begin(), fnames.end(),
            [](const std::string &a, const std::string &b) -> bool
        {
            return
            (
                (a.substr(8, 4) < b.substr(8, 4)) ||
                (a.substr(8, 4) == b.substr(8, 4) && a.substr(5, 2) < b.substr(5, 2))
            );
        });
#if 0
        fmt::print("{}\n", fmt::join(fnames, "\n"));
        std::cout << "fake_fname: " << fake_fname << '\n';
#endif
        // find index of the fake entry & return next week file
        int index = item_index(fnames, fake_fname);
        return fpaths[index];
    }
    return found[0];
}

inline std::string find_last_week_file()
{
    return find_week_file_by_date("now");
}

inline std::vector<std::string> find_week_files_in_span(const std::string &fr, const std::string &to)
{
    const std::string fr_fpath = find_week_file_by_date(fr);
    const std::string to_fpath = find_week_file_by_date(to);
    const std::vector<std::string> fpaths = find_week_files();
    int fr_index = item_index(fpaths, fr_fpath);
    int to_index = item_index(fpaths, to_fpath);
    std::vector<std::string> fpaths_span = vslice(fpaths, fr_index, to_index + 1); // +1 including
#if 0
    for (const auto &p : fpaths_span) {
        std::cout << p << std::endl;
    }
    std::cout << '\n';
    std::cout << fr_index << "\tfr fpath: " << fr_fpath << std::endl;
    std::cout << to_index << "\tto fpath: " << to_fpath << std::endl;
#endif
    return fpaths_span;
}

/*
    vector of all dates of the week found by date string
    (from first to the last day of the week)
*/
inline std::vector<std::string> dates_of_week(const std::string &date_str)
{
    std::tm tm = {};
    std::istringstream ss(date_str);
    ss.imbue(std::locale("en_US.utf-8"));
    ss >> std::get_time(&tm, "%Y-%m-%d");
    std::mktime(&tm); // essential in order to set proper tm_wday
    int first_wday = tm.tm_mday - tm.tm_wday; // first day of the week
    tm.tm_mday = first_wday;
    std::vector<std::string> wdates;
    std::ostringstream buf;
    for (int i = 0; i < 7; i++) {
        // FIXME: HACK: to make sunday last day of the week, not first.
        if (i == 0 && tm.tm_wday == 0) tm.tm_mday -= 7;
        tm.tm_mday += 1;
        std::mktime(&tm);
        buf.clear(); buf.str(""); // clean buffer before inserting new date
        buf << std::put_time(&tm, "%Y-%m-%d");
        wdates.push_back(buf.str());
    }
    return wdates;
}

/*
    find position of nearest newline in multiline str by substring
    return std::string::npos if substring not found!
*/
inline std::size_t fnl_substr(std::string &str, const std::string &substr, bool including_last=false)
{
    std::size_t pos;
    (including_last) ? pos = str.rfind(substr) : pos = str.find(substr);
    if (pos == std::string::npos)
        return std::string::npos; // substr not found!
    if (including_last) {
        // including LAST FOUND line with substring
        for (; pos < str.size(); pos++)
            if (str[pos] == '\n') break;
    } else {
        // excluding FIRST FOUND line with substring
        for (; pos > 0; pos--)
            if (str[pos] == '\n') break;
    }
    return pos;
}

/*
    remove lines before line with substring
    return false if substring not found.
*/
inline bool remove_lines_before(std::string &str, const std::string &substr, bool including_last=false)
{
    std::size_t pos = fnl_substr(str, substr, including_last);
    if (pos == std::string::npos) return false; // substr not found
    str.replace(0, pos + 1, "");
    return true;
}

/*
    remove lines after line with substring
    return false if substring not found.
*/
inline bool remove_lines_after(std::string &str, const std::string &substr, bool including_last=true)
{
    std::size_t pos = fnl_substr(str, substr, including_last);
    if (pos == std::string::npos) return false; // substr not found
    str.replace(pos, std::string::npos, "");
    return true;
}

/*
    Remove lines before line with date substring.
    Iterate over the dates of the week if the date substring is not found,
    to exclude all lines before the date anyway.
*/
inline bool remove_lines_before_date(std::string &str, const std::string &date_str)
{
    if (remove_lines_before(str, date_str, false))
        return true;

    const std::vector<std::string> dates = dates_of_week(date_str);
    int index = item_index(dates, date_str);

    for (int i = index; i >= 0; i--) {
        if (remove_lines_before(str, dates[i], true))
            return true;
    }
    return false;
}

/*
    Remove lines after line with date substring.
    Iterate over the dates of the week if the date substring is not found,
    to exclude all lines after the date anyway.
*/
inline bool remove_lines_after_date(std::string &str, const std::string &date_str)
{
    if (remove_lines_after(str, date_str, true))
        return true;

    const std::vector<std::string> dates = dates_of_week(date_str);
    int index = item_index(dates, date_str);

    for (int i = index; i < dates.size(); i++) {
        if (remove_lines_after(str, dates[i], false))
            return true;
    }
    return false;
}

/*
    concatenate week files removing lines before & after range of dates
*/
inline const std::string concat_week_files
(std::vector<std::string> &fpaths, const std::string &fr, const std::string &to)
{
    // if the date range matches one file
    if (fpaths.size() == 1) {
        std::string fcontent = file_content(fpaths[0]);
        remove_lines_before_date(fcontent, fr);
        remove_lines_after_date(fcontent, to);
        return fcontent;
    }
    std::string fcontent_first = file_content(fpaths[0]);
    std::string fcontent_last = file_content(fpaths.back());
    remove_lines_before_date(fcontent_first, fr);
    remove_lines_after_date(fcontent_last, to);
    // DOUBTS: TODO: should we check if files end with a newline?
    std::ostringstream buf;
    buf << fcontent_first;
    for (int i = 1; i < fpaths.size() - 1; i++)
        buf << file_content(fpaths[i]);
    buf << fcontent_last;
    return buf.str();
}

#endif // TGRP_H
