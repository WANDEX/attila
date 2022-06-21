#include <cmath>    // floor
#include <ctime>    // difftime, mktime

#include <fstream>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <future>   // async
#include <thread>   // hardware_concurrency

#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <locale>
#include <iomanip>  // put_time

#include <fmt/core.h>
#include <fmt/format.h> // fmt::join

#include "tgrp.hpp"

#include "structs.hpp"  // ss  namespace with struct defs
#include "str.hpp"      // str namespace

namespace fs = std::filesystem;

std::vector<int> split_vi(const std::string &s, char delimiter)
{
    std::vector<int> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(std::stoi(token));
    }
    return tokens;
}

const ss::hm_t calculate_time_spent(const std::string &fr, const std::string &to)
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
    if (sec_diff == 0) return {0, 0, 0, "00:00"};
    // fix: 23:53 -> 00:07 expected time spent: (00:14)
    // recalculate if the task was ended the next day
    if (sec_diff < 1) {
        t2.tm_mday = t2.tm_mday + 1;
        end = std::mktime(&t2);
        sec_diff = std::floor(std::difftime(end, beg));
    }
    int h = sec_diff / 3600;
    int m = sec_diff % 3600 / 60;
    std::string str = fmt::format("{:02}:{:02}", h, m);
    return {h, m, static_cast<size_t>(sec_diff), str};
}

const ss::hm_t time_spent(const std::string &s)
{
    const std::regex r{R"((\d\d:\d\d).*(\d\d:\d\d))"}; // time span: from - to (hh:mm)
    std::smatch m;
    if(!std::regex_search(s, m, r)) {
        std::cerr << "time span was not found in the string:" << '\n' << s << '\n';
        exit(77); // XXX
    }
    return calculate_time_spent(m[1], m[2]);
}

std::pair<std::string, std::string> dt_and_task(const std::string &s)
{
    const std::regex r{R"((^.*\d\d:\d\d.*\d\d:\d\d) (.*$))"}; // time span: from - to (hh:mm)
    std::smatch m;
    if(!std::regex_search(s, m, r)) {
        std::cerr << "time span was not found in the string:" << '\n' << s << '\n';
        exit(76); // XXX
    }
    return std::make_pair(m[1], m[2]);
}

std::vector<std::string> projects_of_task(const std::string &s)
{
    const std::regex projects{R"((\[.*\]))"};
    std::smatch m;
    if(!std::regex_search(s, m, projects))
        return {};
    const std::regex re{R"(([\[\]]))"};
    return str::resplit(m.str(), re); // [nvim][lsp] -> nvim lsp
}

/*
    parse/analyze multiline string of tasks
*/
ss::vtasks_t parse_tasks(const std::string &s)
{
    ss::vtasks_t tasks;
    std::string line;
    std::istringstream content(s);
    while (std::getline(content, line)) {
        std::pair<std::string, std::string> dt_text = dt_and_task(line);
        std::string& dt   = dt_text.first;
        std::string& text = dt_text.second;
        ss::hm_t hm_t = time_spent(dt);
        std::vector<std::string> words = str::split_on_words(text);
        std::vector<std::string> tproj = projects_of_task(text);
        ss::task_t task = {dt, text, hm_t, words, tproj};
        tasks.push_back(task);
    }
#if 0
    for (const auto &t : tasks) {
        std::cout << std::endl
            << t.dt << std::endl
            << t.text << std::endl
            << t.hm_t.str << std::endl;
        fmt::print("[{}]\n", fmt::join(t.words, ", "));
        if (!t.tproj.empty())
            fmt::print("> tproj: {}\n", fmt::join(t.tproj, ", "));
    }
#endif
    return tasks;
}

/*
    wrapper around parse_tasks() for parallel/async parsing/analyzing of multiline string
*/
ss::vtasks_t parse_tasks_parallel(const std::string &s)
{
    size_t nl = std::count(s.begin(), s.end(), '\n'); // new lines count
    size_t threads_total = std::thread::hardware_concurrency();
    if (threads_total < 2 || nl < 101) { // simple single threaded mode
        return parse_tasks(s);
    }
    size_t num_threads = threads_total - 1; // -1 thread is essential for the algorithm
    // fill the lines vector
    std::string line;
    std::vector<std::string> lines;
    std::istringstream content(s);
    while (std::getline(content, line))
        lines.push_back(line);
    // lines per thread (-1 thread) & remainder
    size_t lpt = std::floor(nl / num_threads);
    double lpt_remainder = nl % num_threads;
    // lambda function for feeding the tasks analyzer
    // with equally distributed chunks-lines of one large text
    auto parse_tasks_lines = [&](size_t i, bool to_the_end=false) -> ss::vtasks_t {
        if (to_the_end)
            return parse_tasks(str::lines_between(lines, lpt*i, -1));
        else
            return parse_tasks(str::lines_between(lines, lpt*i, lpt*(i+1)));
    };
    // vector of futures which will contain vector of task structs
    std::vector<std::future<ss::vtasks_t>> futures;
    for (size_t i = 0; i < num_threads; i++) {
        futures.insert(futures.begin() + i,
                std::async(std::launch::async, parse_tasks_lines, i));
    }
    // if has remainder -> process leftover lines on additional (last thread)
    if (lpt_remainder != 0) {
        futures.insert(futures.begin() + num_threads,
                std::async(std::launch::async, parse_tasks_lines, num_threads, true));
    }
    // extend vector with tasks_t vectors got from vector of futures
    ss::vtasks_t vtt;
    for(auto &e : futures) {
        ss::vtasks_t tmp_vec = e.get();
        vtt.insert(vtt.end(), tmp_vec.begin(), tmp_vec.end());
    }
    return vtt;
}

std::string tasks_to_mulstr(ss::vtasks_t tasks)
{
    std::ostringstream out;
    for (const auto &t : tasks) {
        out << t.dt << " <" << t.hm_t.str << "> " << t.text << '\n';
    }
    return out.str();
}

std::vector<std::string> get_all_files_recursive(const fs::path &path)
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

std::vector<std::string> find_week_files(const std::string &pmatch = "week-")
{
    std::string POMODORO_DIR = str::sane_getenv("POMODORO_DIR");
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
std::string week_file_name(const std::string &date_str)
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
std::vector<std::string> vslice(const std::vector<std::string> &v, int start=0, int end=-1)
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

int item_index(const std::vector<std::string> &v, const std::string &item)
{
    auto ret = std::find(v.begin(), v.end(), item);
    if (ret != v.end())
        return ret - v.begin();
    return -1; // return the last element index
}

std::string find_week_file_by_date(const std::string &date_str)
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

std::string find_last_week_file()
{
    return find_week_file_by_date("now");
}

std::vector<std::string> find_week_files_in_span(const std::string &fr, const std::string &to)
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
std::vector<std::string> dates_of_week(const std::string &date_str)
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
    Remove lines before line with date substring.
    Iterate over the dates of the week if the date substring is not found,
    to exclude all lines before the date anyway.
*/
bool remove_lines_before_date(std::string &s, const std::string &date_str)
{
    if (str::remove_lines_before(s, date_str, false))
        return true;

    const std::vector<std::string> dates = dates_of_week(date_str);
    int index = item_index(dates, date_str);

    for (int i = index; i >= 0; i--) {
        if (str::remove_lines_before(s, dates[i], true))
            return true;
    }
    return false;
}

/*
    Remove lines after line with date substring.
    Iterate over the dates of the week if the date substring is not found,
    to exclude all lines after the date anyway.
*/
bool remove_lines_after_date(std::string &s, const std::string &date_str)
{
    if (str::remove_lines_after(s, date_str, true))
        return true;

    const std::vector<std::string> dates = dates_of_week(date_str);
    int index = item_index(dates, date_str);

    for (int i = index; i < dates.size(); i++) {
        if (str::remove_lines_after(s, dates[i], false))
            return true;
    }
    return false;
}

/*
    concatenate week files removing lines before & after range of dates
*/
const std::string concat_week_files(std::vector<std::string> &fpaths,
                                    const std::string &fr, const std::string &to)
{
    // if the date range matches one file
    if (fpaths.size() == 1) {
        std::string fcontent = str::file_content(fpaths[0]);
        remove_lines_before_date(fcontent, fr);
        remove_lines_after_date(fcontent, to);
        return str::trim(fcontent);
    }
    std::string fcontent_first = str::file_content(fpaths[0]);
    std::string fcontent_last  = str::file_content(fpaths.back());
    remove_lines_before_date(fcontent_first, fr);
    remove_lines_after_date(fcontent_last, to);
    std::ostringstream buf;
    buf << fcontent_first;
    for (int i = 1; i < fpaths.size() - 1; i++)
        buf << str::file_content(fpaths[i]);
    buf << fcontent_last;
    return str::trim(buf.str());
}

std::string concat_span(const std::string &fr, const std::string &to)
{
    std::vector<std::string> fpaths = find_week_files_in_span(fr, to);
    std::string content = concat_week_files(fpaths, fr, to);
    return content;
}

/*
    filter multiline string by lines containing matching pattern
*/
std::string filter_find(const std::string &s, const std::string &reinput)
{
    const std::regex re(reinput, std::regex::ECMAScript|std::regex::icase);
    std::smatch m;
    std::string line;
    std::istringstream iss(s);
    std::ostringstream oss;
    while (std::getline(iss, line)) {
        if(std::regex_search(line, m, re))
            oss << line << '\n';
    }
    return oss.str();
}
