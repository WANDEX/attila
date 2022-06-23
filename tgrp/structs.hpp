#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <atomic>  // atomic, fetch_add
#include <cstddef> // size_t
#include <set>
#include <string>
#include <vector>

namespace ss
{
    inline std::uint32_t getID() {
        static std::atomic<std::uint32_t> uid { 0 };
        return uid.fetch_add(1, std::memory_order_relaxed);
    }

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
        std::uint32_t id { ss::getID() };
        std::set<ss::task_t> subt_t {};
    };

    inline bool operator<(const ss::task_t &lhs, const ss::task_t &rhs) {
        return lhs.id < rhs.id;
    }

    using vtasks_t = std::vector<ss::task_t>;
    using stasks_t = std::set<ss::task_t>;

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
