#include <fstream>
#include <iostream>
#include <regex>

#include <cmath> // floor
#include <ctime> // difftime, mktime

#include <fmt/core.h>

std::vector<int> split_vi(const std::string &s, char delimiter) {
    std::vector<int> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(std::stoi(token));
    }
    return tokens;
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
    std::regex const r{R"((\d\d:\d\d).*(\d\d:\d\d))"}; // time span: from - to (hh:mm)
    std::smatch m;
    if(!std::regex_search(s, m, r)) {
        std::cerr << "time span was not found in the string:" << '\n' << s << '\n';
        exit(77); // XXX
    }
    return calculate_time_spent(m[1], m[2]);
}

int main()
{
    std::string s = "Saturday (2022-01-08) 12:01 → 14:00 cleaning sink pipes"; // XXX
    std::string spent = time_spent(s);
    std::cout << spent;
    return 0;
}
