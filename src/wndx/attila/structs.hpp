#pragma once

#include "aliases.hpp"

#include <atomic>               // atomic, fetch_add
#include <ctime>                // std::time_t
#include <set>
#include <vector>

namespace wndx::ss {

inline s32 getID() {
    static std::atomic<s32> uid{ 0 };
    return uid.fetch_add(1, std::memory_order_relaxed);
}

struct hm_t {
    std::tm  tm_beg;
    std::tm  tm_end;
    std::time_t beg;
    std::time_t end;
    std::time_t diff;
    str_t date_fr;
    str_t date_to;
    str_t time_fr;
    str_t time_to;
    str_t time_spent;
};

struct task_t {
    str_t dts;
    str_t text;
    hm_t  hm;
    vec_str_t words;
    vec_str_t tproj;
    s32   id{ getID() };
    std::set<task_t> subt_t {};
#if 1
    auto operator<=>(const task_t &rhs) const {
        return id <=> rhs.id;
    }
#else
    bool operator<(const task_t &lhs, const task_t &rhs) {
        return lhs.id < rhs.id;
    }
#endif
};

struct stats_t {
    sz_t avg;
    sz_t max;
    sz_t min;
    sz_t sum;
    sz_t nrecords;
};

struct stats_human_t {
    str_t avg;
    str_t max;
    str_t min;
    str_t sum;
    sz_t nrecords;
};

struct group_t {
    // str_t color; // TODO: generate group unique hex color, can be overridden by the user
    // TODO: words are manually added by the user in the UI group container
    std::set<str_t>  words   {}; // auto-associate task to the group by unique word
    std::set<task_t> tasks_t {};
    str_t gname{ "group_" + std::to_string(gid) }; // TODO: can be overridden by the user
    s32   gid{ getID() };
#if 1
    auto operator<=>(const group_t &rhs) const {
        return gid <=> rhs.gid;
    }
#else
    bool operator<(const group_t &lhs, const group_t &rhs) {
        return lhs.id < rhs.id;
    }
#endif
};

using vtasks_t  = std::vector<task_t>;
using stasks_t  = std::set<task_t>;
using sgroups_t = std::set<group_t>;

} // namespace wndx::ss

