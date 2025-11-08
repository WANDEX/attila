#include "attila.hpp"

#include "structs.hpp"          // ss  namespace with struct defs
#include "str.hpp"              // str namespace

#include <fmt/core.h>
#include <fmt/format.h>         // fmt::join

#include <algorithm>
#include <ctime>                // time_t, mktime, difftime
#include <future>               // async
#include <iomanip>              // put_time
#include <locale>
#include <regex>                // TODO: replace std::regex with RE2
#include <sstream>
#include <thread>               // hardware_concurrency

namespace wndx {

std::vector<int> split_vi(str_v s, ch_t delimiter)
{
    std::vector<int> tokens;
    str_t token;
    str_t const str(s.data(), s.size()); // XXX
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(std::stoi(token));
    }
    return tokens;
}

// ss::hm_t calculate_time_spent(str_v d_fr, str_v d_to, str_v t_fr, str_v t_to)
ss::hm_t calculate_time_spent(str_t d_fr, str_t d_to, str_t t_fr, str_t t_to)
{
    // hour:min from string
    std::vector<int> fr_res { split_vi(t_fr, ':') };
    std::vector<int> to_res { split_vi(t_to, ':') };

#if 0
    fmt::print("t_fr:{},{}\n", fr_res[0], fr_res[1]);
    fmt::print("t_to:{},{}\n", to_res[0], to_res[1]);
#endif

    std::tm t1 {};
    // str_t const str1(d_fr.data(), d_fr.size()); // XXX
    str_t const str1(d_fr.begin(), d_fr.end()); // XXX
    std::istringstream t1s(str1);
    t1s >> std::get_time(&t1, str::datef); // date from string
    t1.tm_hour = fr_res[0];
    t1.tm_min  = fr_res[1];
    std::time_t beg = std::mktime(&t1);    // sec since epoch

    std::tm t2 {};
    str_t const str2(d_fr.data(), d_fr.size()); // XXX
    std::istringstream t2s(str2);
    t2s >> std::get_time(&t2, str::datef);
    t2.tm_hour = to_res[0];
    t2.tm_min  = to_res[1];
    std::time_t end = std::mktime(&t2);

    std::time_t diff = std::difftime(end, beg);
    if (diff == 0) {
        return { t1, t2, beg, end, 0,
            d_fr, d_to, t_fr, t_to, "00:00"
        };
    }
    // fix: 23:53 -> 00:07 expected time spent: (00:14)
    // recalculate if the task was ended the next day
    if (diff < 1) {
        t2.tm_mday = t2.tm_mday + 1;
        end = std::mktime(&t2);
        diff = std::difftime(end, beg);
    }
    auto tmp{ str::sec_to_tstr(diff) }; // XXX
    return{ t1, t2, beg, end, diff, d_fr, d_to, t_fr, t_to, tmp };
}

ss::hm_t time_spent(str_v s)
{
    std::smatch m;
    str_t const str(s.data(), s.size()); // XXX
    if (!std::regex_search(str, m, str::dts_re)) {
        try { // XXX
            throw std::runtime_error("date and/or time span was not found in the string");
        } catch (std::runtime_error const& e) {
            WNDX_LOG(LL::WARN, "{}\n", e.what());
            throw;
        }
    }
    // return calculate_time_spent(m[1], m[1], m[2], m[3]);
    return calculate_time_spent(m[1], m[1], m[2], m[3]);
}

std::pair<str_t, str_t> dts_and_task(str_v s)
{
    std::smatch m;
    str_t const str(s.data(), s.size()); // XXX
    if (!std::regex_search(str, m, str::dts_txt_re)) {
        try { // XXX
            throw std::runtime_error("time span and/or task text was not found in the string");
        } catch (std::runtime_error const& e) {
            WNDX_LOG(LL::WARN, "{}\n", e.what());
            throw;
        }
    }
    return std::make_pair(m[1], m[4]);
}

vec_str_t projects_of_task(str_v s)
{
    std::regex const projects{R"((\[.*\]))"};
    std::smatch m;
    str_t const str(s.data(), s.size()); // XXX
    if (!std::regex_search(str, m, projects))
        return {};
    std::regex const re{R"(([\[\]]))"};
    return str::resplit(m.str(), re); // [nvim][lsp] -> nvim lsp
}

/**
 * parse/analyze multiline string of tasks
 */
ss::vtasks_t parse_tasks(str_v s)
{
    ss::vtasks_t tasks;
    str_t line;
    str_t const str(s.data(), s.size()); // XXX
    std::istringstream content(str);
    std::pair<str_t, str_t> dts_text {};
    ss::hm_t hm {};
    str_t skip_msg = "^ Skipping task due to previous exception with line: ";
    while (std::getline(content, line)) {
        try {
            dts_text = dts_and_task(line);
        } catch (std::runtime_error const& e) {
            WNDX_LOG(LL::INFO, "{}\n{}\n{}\n", e.what(), skip_msg, line);
            continue; // safely handle exception by skipping this task
        }
        auto const dts { dts_text.first  };
        auto const text{ dts_text.second };
        try {
            hm = time_spent(dts);
        } catch (std::runtime_error const& e) {
            WNDX_LOG(LL::INFO, "{}\n{}\n{}\n", e.what(), skip_msg, line);
            continue; // safely handle exception by skipping this task
        }
        auto const words{ str::split_on_words(text) };
        auto const tproj{    projects_of_task(text) };
        ss::task_t task{ dts, text, hm, words, tproj };
        tasks.push_back(task);
    }
#if 0
    for (const auto &t : tasks) {
        std::cout << std::endl
            << t.dts << std::endl
            << t.text << std::endl
            << t.hm.str << std::endl;
        fmt::print("[{}]\n", fmt::join(t.words, ", "));
        if (!t.tproj.empty())
            fmt::print("> tproj: {}\n", fmt::join(t.tproj, ", "));
    }
#endif
    return tasks;
}

/**
 * wrapper around parse_tasks() for parallel/async parsing/analyzing of multiline string
 */
ss::vtasks_t parse_tasks_parallel(str_v s)
{
    size_t nl = std::count(s.begin(), s.end(), '\n'); // new lines count
    size_t threads_total = std::thread::hardware_concurrency();
    if (threads_total < 2 || nl < 101) { // simple single threaded mode
        return parse_tasks(s);
    }
    size_t num_threads = threads_total - 1; // -1 thread is essential for the algorithm
    // fill the lines vector
    str_t line;
    vec_str_t lines;
    str_t const str(s.data(), s.size()); // XXX
    std::istringstream content(str);
    while (std::getline(content, line))
        lines.push_back(line);
    // lines per thread (-1 thread) & remainder
    size_t lpt = nl / num_threads;
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

auto get_all_files_recursive(fs::path const &path)
{
    vec_str_t fpaths;
    for (const auto& p : fs::recursive_directory_iterator(path)) {
        if (!fs::is_directory(p)) {
#if 1
            fs::path path = p.path();
            // fpaths.push_back(path.u8string());
            fpaths.push_back(path.string());
#else
            fpaths.push_back(p.path());
#endif
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

auto find_week_files(str_v pmatch = "week-") -> vec_str_t
{
    str_t POMODORO_DIR = str::sane_getenv("POMODORO_DIR");
#if 0
    auto fpaths = get_all_files_recursive(POMODORO_DIR);
    auto& v = fpaths; // reference for shortness
#else
    vec_str_t fpaths = get_all_files_recursive(POMODORO_DIR);
    vec_str_t& v = fpaths; // reference for shortness
#endif
    auto match = [=](const str_t &tmps) {
        return tmps.find(pmatch) == str_t::npos;
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

/**
 * construct & return week file name by the date string
 */
str_t week_file_name(str_v date_str)
{
    std::tm tm1 {}, tm2 {};
    const char* wfmt = "week-%V-%Y.txt";
    const std::time_t now = std::time(nullptr);
    tm1 = *std::localtime(&now);
    std::ostringstream buf;
    if (date_str == "now") {
        buf << std::put_time(&tm1, wfmt);
        return buf.str();
    }
    str_t const str(date_str.data(), date_str.size()); // XXX
    std::istringstream ss(str);
    ss.imbue(std::locale("en_US.utf-8"));
    ss >> std::get_time(&tm2, str::datef);
    const std::time_t t = std::mktime(&tm2);
    // if date str > date now => current week fname
    if (std::difftime(now, t) < 0)
        buf << std::put_time(&tm1, wfmt);
    else
        buf << std::put_time(&tm2, wfmt);
    return buf.str();
}

/**
 * vector slice by indexes, like the list slicing in python
 */
vec_str_t vslice(vec_str_t const &v, int start=0, int end=-1)
{
    int oldlen = v.size();
    int newlen;
    if (end == -1 or end >= oldlen) {
        newlen = oldlen - start;
    } else {
        newlen = end - start;
    }
    vec_str_t nv(newlen);
    for (int i=0; i<newlen; i++) {
        nv[i] = v[start + i];
    }
    return nv;
}

int item_index(vec_str_t const &v, str_v item)
{
    auto ret = std::find(v.begin(), v.end(), item);
    if (ret != v.end())
        return ret - v.begin();
    return -1; // return the last element index
}

str_t find_week_file_by_date(str_v date_str)
{
    const vec_str_t found = find_week_files(week_file_name(date_str));
    if (found.empty()) { // find closest next found week file
        const vec_str_t fpaths = find_week_files();
        const str_t fake_fname = week_file_name(date_str);
        vec_str_t fnames;
        fnames.push_back(fake_fname); // add fake entry week fname
        for (const fs::path p : fpaths)
            fnames.push_back(p.filename());
        // fname example: week-05-2022.txt
        // substr(8, 4) = year; substr(5, 2) = week_num
        std::sort(fnames.begin(), fnames.end(),
            [](str_v a, str_v b) -> bool
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

str_t find_last_week_file()
{
    return find_week_file_by_date("now");
}

auto find_week_files_in_span(str_v fr, str_v to)
{
    auto fr_fpath{ find_week_file_by_date(fr) };
    auto to_fpath{ find_week_file_by_date(to) };
    auto   fpaths{ find_week_files() };
    auto fr_index{ item_index(fpaths, fr_fpath) };
    auto to_index{ item_index(fpaths, to_fpath) };
    auto fpaths_span{  vslice(fpaths, fr_index, to_index + 1) }; // +1 including
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

/**
 * vector of all dates of the week found by date string
 * (from first to the last day of the week)
 */
vec_str_t dates_of_week(str_v date_str)
{
    std::tm tm = {};
    str_t const str(date_str.data(), date_str.size()); // XXX
    std::istringstream ss(str);
    ss.imbue(std::locale("en_US.utf-8"));
    ss >> std::get_time(&tm, str::datef);
    std::mktime(&tm); // essential in order to set proper tm_wday
    int first_wday = tm.tm_mday - tm.tm_wday; // first day of the week
    tm.tm_mday = first_wday;
    vec_str_t wdates;
    std::ostringstream buf;
    for (int i = 0; i < 7; i++) {
        // FIXME: HACK: to make sunday last day of the week, not first.
        if (i == 0 && tm.tm_wday == 0) tm.tm_mday -= 7;
        tm.tm_mday += 1;
        std::mktime(&tm);
        buf.clear(); buf.str(""); // clean buffer before inserting new date
        buf << std::put_time(&tm, str::datef);
        wdates.push_back(buf.str());
    }
    return wdates;
}

/**
 * Remove lines before line with date substring.
 * Iterate over the dates of the week if the date substring is not found,
 * to exclude all lines before the date anyway.
 */
bool remove_lines_before_date(str_t &s, str_v date_str)
{
    if (str::remove_lines_before(s, date_str, false))
        return true;

    const vec_str_t dates = dates_of_week(date_str);
    int index = item_index(dates, date_str);

    for (int i = index; i >= 0; i--) {
        if (str::remove_lines_before(s, dates[i], true))
            return true;
    }
    return false;
}

/**
 * Remove lines after line with date substring.
 * Iterate over the dates of the week if the date substring is not found,
 * to exclude all lines after the date anyway.
 */
bool remove_lines_after_date(str_t &s, str_v date_str)
{
    if (str::remove_lines_after(s, date_str, true))
        return true;

    const vec_str_t dates = dates_of_week(date_str);
    int index = item_index(dates, date_str);

    for (int i = index; i < dates.size(); i++) {
        if (str::remove_lines_after(s, dates[i], false))
            return true;
    }
    return false;
}

/**
 * concatenate week files removing lines before & after range of dates
 */
str_t concat_week_files(vec_str_t &fpaths, str_v fr, str_v to)
{
    // if the date range matches one file
    if (fpaths.size() == 1) {
        str_t fcontent = str::file_content(fpaths[0]);
        remove_lines_before_date(fcontent, fr);
        remove_lines_after_date(fcontent, to);
        return str::trim(fcontent);
    }
    str_t fcontent_first = str::file_content(fpaths[0]);
    str_t fcontent_last  = str::file_content(fpaths.back());
    remove_lines_before_date(fcontent_first, fr);
    remove_lines_after_date(fcontent_last, to);
    std::ostringstream buf;
    buf << fcontent_first;
    for (int i = 1; i < fpaths.size() - 1; i++)
        buf << str::file_content(fpaths[i]);
    buf << fcontent_last;
    return str::trim(buf.str());
}

str_t concat_span(str_v fr, str_v to)
{
    // FIXME: find_week_files_in_span
    vec_str_t fpaths = find_week_files_in_span(fr, to);
    str_t content = concat_week_files(fpaths, fr, to);
    return content;
}

/**
 * filter multiline string by lines containing matching pattern
 */
str_t filter_find(str_v s, str_v reinput)
{
    std::regex const re(reinput.data(), reinput.length(), std::regex::ECMAScript|std::regex::icase);
    std::smatch m;
    str_t line;
    str_t const str(s.data(), s.size()); // XXX
    std::istringstream iss(str);
    std::ostringstream oss;
    while (std::getline(iss, line)) {
        if (std::regex_search(line, m, re))
            oss << line << '\n';
    }
    return oss.str();
}

} // namespace wndx
