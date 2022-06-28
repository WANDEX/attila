#include "stats.hpp"
#include "structs.hpp" // ss  namespace with struct defs
#include "str.hpp"     // str namespace

#include <sstream>   // ostringstream

#include <algorithm> // erase/remove
#include <cstddef>   // size_t
#include <ctime>     // time_t
#include <set>
#include <string>
#include <vector>

#include <fmt/core.h>

const ss::stats_t calculate_stats(const ss::vtasks_t &vtt)
{
    std::size_t sum {0}; // total spent on all tasks in seconds
    std::size_t avg {0};
    std::size_t max {0};
    std::size_t min { static_cast<std::size_t>(vtt.at(0).hm_t.diff) };
    const std::size_t nrecords { vtt.size() };

    std::size_t sec {0}; // total spent on task in seconds
    for (const auto &t : vtt) {
        sec = t.hm_t.diff;
        sum += sec;
        if (max < sec)
            max = sec;
        if (min > sec)
            min = sec;
    }
    avg = sum / nrecords;
    return { avg, max, min, sum, nrecords };
}

const ss::stats_human_t calculate_stats_human(const ss::stats_t &t)
{
    // convert size_t seconds into HH:MM spent time string
    auto hm = [&](const std::size_t sec) -> const std::string {
        return fmt::format("{:02}:{:02}", sec / 3600, sec % 3600 / 60);
    };
    return { hm(t.avg), hm(t.max), hm(t.min), hm(t.sum), t.nrecords };
}

std::pair<const ss::vtasks_t, const std::string>
    merge_tasks(const ss::vtasks_t &vtt, const std::string &mulstr)
{
    ss::vtasks_t v {vtt};
    // remove vector elements which text is not in multiline string
    // NOTE: (in case multiline string was filtered by the regex)
    for (ss::vtasks_t::iterator it = v.begin(); it != v.end(); ++it) {
        if (!str::has_substr(mulstr, it->text))
            v.erase(it);
    }

    for (std::size_t i = 0; i < v.size(); ++i) {
        bool already_exist = false;
        ss::stasks_t subt_t {};
        for (std::size_t j = i; j < v.size(); ++j) {
            bool same_text = (v[i].text == v[j].text) ? true : false;
            if (!already_exist && same_text) {
                // do not remove first found -> this will be the main task of sub tasks
                already_exist = true;
                subt_t.insert(v[j]);
                continue;
            }
            if (same_text) {
                // NOTE: set -> since we do not want to insert the same thing more than once
                subt_t.insert(v[j]);
                v.erase(v.begin() + j--); // remove by index & decrement index afterwards
            }
        }
        v.at(i).subt_t.insert(subt_t.begin(), subt_t.end()); // put set of sub tasks as child's
    }

    // sum time spent of all sub-tasks & set new parameters of the main task
    // compose string with text indicating merged tasks into one main task
    for (auto &main_task: v) {
        if (main_task.subt_t.size() < 2) {
            continue; // skip -> this task does not have sub-tasks
        }

        std::time_t sec {0};
        for (const auto &sub_task: main_task.subt_t) {
            sec += sub_task.hm_t.diff;
        }

        const auto last = main_task.subt_t.rbegin();
        // update hm_t struct values
        main_task.hm_t.tm_end     = last->hm_t.tm_end;
        main_task.hm_t.end        = last->hm_t.end;
        main_task.hm_t.diff       = sec;
        main_task.hm_t.date_to    = last->hm_t.date_to;
        main_task.hm_t.time_to    = last->hm_t.time_to;
        main_task.hm_t.time_spent = str::sec_to_tstr(sec);

        // if first & last sub-task date differ -> only date strings without time: fr -> to
        std::ostringstream out;
        if (main_task.hm_t.date_fr == main_task.hm_t.date_to) {
            out << "*M  (" << main_task.hm_t.date_fr << ") "
                << main_task.hm_t.time_fr << " > " << main_task.hm_t.time_to;
        } else {
            out << "*M  (" << main_task.hm_t.date_fr << " >> " << main_task.hm_t.date_to << ")";
        }
        main_task.dts = out.str();
    }

    return std::make_pair(v, str::tasks_to_mulstr(v));
}
