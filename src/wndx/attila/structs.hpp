#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <atomic>  // atomic, fetch_add
#include <cstddef> // size_t
#include <ctime>   // time_t
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
        std::tm  tm_beg;
        std::tm  tm_end;
        std::time_t beg;
        std::time_t end;
        std::time_t diff;
        std::string date_fr;
        std::string date_to;
        std::string time_fr;
        std::string time_to;
        std::string time_spent;
    };

    struct task_t {
        std::string dts;
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

    struct group_t {
        // std::string color; // TODO: generate group unique hex color, can be overridden by the user
        // TODO: words are manually added by the user in the UI group container
        std::set<std::string> words {}; // auto-associate task to the group by unique word
        std::set<ss::task_t> tasks_t {};
        std::string gname { "group_" + std::to_string(gid) }; // TODO: can be overridden by the user
        std::uint32_t gid { ss::getID() };
    };

    inline bool operator<(const ss::group_t &lhs, const ss::group_t &rhs) {
        return lhs.gid < rhs.gid;
    }

    using sgroups_t = std::set<ss::group_t>;
}

#endif // STRUCTS_HPP
