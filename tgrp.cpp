#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>

#include <cmath> // floor
#include <ctime> // difftime, mktime

#include <fmt/core.h>
#include <fmt/format.h> // fmt::join

std::vector<int> split_vi(const std::string &s, char delimiter) {
    std::vector<int> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(std::stoi(token));
    }
    return tokens;
}

std::vector<std::string> resplit(const std::string &s, const std::regex &re = std::regex{"\\s+"})
{
    std::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
    std::sregex_token_iterator end;
    return {iter, end};
}

std::vector<std::string> split_on_words(const std::string &s)
{
    std::regex sep_regex("[ [:punct:]]+", std::regex::extended);
    std::vector<std::string> words = resplit(s, sep_regex);
    return words;
}

std::string calculate_time_spent(const std::string &fr, const std::string &to)
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

std::string time_spent(const std::string &s)
{
    const std::regex r{R"((\d\d:\d\d).*(\d\d:\d\d))"}; // time span: from - to (hh:mm)
    std::smatch m;
    if(!std::regex_search(s, m, r)) {
        std::cerr << "time span was not found in the string:" << '\n' << s << '\n';
        exit(77); // XXX
    }
    return calculate_time_spent(m[1], m[2]);
}

std::string file_content(const std::string &file_path)
{
    std::ifstream rfile(file_path, std::ios::in);
    std::string content((std::istreambuf_iterator<char>(rfile)),
                        (std::istreambuf_iterator<char>()    ));
    return content;
}

void file_time_spent(const std::string &file_path)
{
    std::string line;
    std::istringstream fc(file_content(file_path));
    while (std::getline(fc, line)) {
        std::cout << time_spent(line) << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::string file_path = "week-01-2022.txt";
    file_time_spent(file_path);
    return 0;
}
