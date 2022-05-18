#ifndef TGRP_H
#define TGRP_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <iomanip> // put_time

#include <cmath> // floor
#include <ctime> // difftime, mktime

#include <fmt/core.h>
#include <fmt/format.h> // fmt::join

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

inline std::vector<int> split_vi(const std::string &s, char delimiter) {
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

inline std::string last_week_file_name()
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout.imbue(std::locale("en_US.utf8"));
    std::stringstream buf;
    buf << std::put_time(&tm, "week-%V-%Y.txt");
    return buf.str();
}

#endif // TGRP_H
