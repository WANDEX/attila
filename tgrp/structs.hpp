#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <cstddef> // size_t
#include <string>
#include <vector>

namespace ss
{

    struct hm_t {
        int h;
        int m;
        std::size_t in_sec;
        std::string str;
    };

    struct task_t {
        std::string dt;
        std::string text;
        ss::hm_t    hm_t;
        std::vector<std::string> words;
        std::vector<std::string> tproj;
    };

    using vtasks_t = std::vector<ss::task_t>;

    struct stats_t {
        const std::size_t avg;
        const std::size_t max;
        const std::size_t min;
        const std::size_t sum;
        const std::size_t nrecords;
    };

    struct stats_human_t {
        const std::string avg;
        const std::string max;
        const std::string min;
        const std::string sum;
        const std::size_t nrecords;
    };
}

#endif // STRUCTS_HPP
