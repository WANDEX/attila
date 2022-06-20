#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <cstddef> // size_t
#include <string>
#include <vector>

namespace ss
{
    struct task_t {
        std::string dt;
        std::string text;
        std::string spent;
        std::vector<std::string> words;
        std::vector<std::string> tproj;
    };

    using vtasks_t = std::vector<ss::task_t>;

    struct stats_t {
        const std::size_t nrecords;
        // TODO: max/min/avg/total
    };
}

#endif // STRUCTS_HPP
