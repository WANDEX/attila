#include "stats.hpp"

#include "str.hpp"              // str namespace
#include "structs.hpp"          // ss  namespace with struct defs

#include <fmt/core.h>

#include <sstream>              // ostringstream
#include <algorithm>            // erase/remove
#include <cstddef>              // size_t
#include <ctime>                // time_t
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace wndx::ss {

stats_t calculate_stats(const vtasks_t &vtt)
{
    std::size_t sum {0}; // total spent on all tasks in seconds
    std::size_t avg {0};
    std::size_t max {0};
    std::size_t min { static_cast<std::size_t>(vtt.at(0).hm.diff) };
    const std::size_t nrecords { vtt.size() };

    std::size_t sec {0}; // total spent on task in seconds
    for (const auto &t : vtt) {
        sec = static_cast<std::size_t>(t.hm.diff);
        sum += sec;
        if (max < sec)
            max = sec;
        if (min > sec)
            min = sec;
    }
    avg = sum / nrecords;
    return { avg, max, min, sum, nrecords };
}

stats_human_t calculate_stats_human(const stats_t &t)
{
    // convert size_t seconds into HH:MM spent time string
    auto hm = [&](const std::size_t sec) -> str_t {
        return fmt::format("{:02}:{:02}", sec / 3600, sec % 3600 / 60);
    };
    return { hm(t.avg), hm(t.max), hm(t.min), hm(t.sum), t.nrecords };
}

std::pair<const vtasks_t, str_t>
merge_tasks(const vtasks_t &vtt, const str_t &mulstr)
{
    vtasks_t v {vtt};
    // remove vector elements which text is not in multiline string
    // NOTE: (in case multiline string was filtered by the regex)
    for (vtasks_t::iterator it = v.begin(); it != v.end(); ++it) {
        if (!str::has_substr(mulstr, it->text))
            v.erase(it);
    }

    for (std::size_t i = 0; i < v.size(); ++i) {
        bool already_exist = false;
        stasks_t subt_t {};
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
                v.erase(v.begin() + static_cast<long>(j--)); // remove by index & decrement index afterwards
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
            sec += sub_task.hm.diff;
        }

        const auto last = main_task.subt_t.rbegin();
        // update hm_t struct values
        main_task.hm.tm_end     = last->hm.tm_end;
        main_task.hm.end        = last->hm.end;
        main_task.hm.diff       = sec;
        main_task.hm.date_to    = last->hm.date_to;
        main_task.hm.time_to    = last->hm.time_to;
        main_task.hm.time_spent = str::sec_to_tstr(sec);

        // if first & last sub-task date differ -> only date strings without time: fr -> to
        std::ostringstream out;
        if (main_task.hm.date_fr == main_task.hm.date_to) {
            out << "*M  (" << main_task.hm.date_fr << ") "
                << main_task.hm.time_fr << " > " << main_task.hm.time_to;
        } else {
            out << "*M  (" << main_task.hm.date_fr << " >> " << main_task.hm.date_to << ")";
        }
        main_task.dts = out.str();
    }

    return std::make_pair(v, str::tasks_to_mulstr(v));
}

/**
 * auto create and populate groups by the tasks with unique project name
 */
sgroups_t auto_proj_groups(const vtasks_t &vtt)
{
    sgroups_t groups {};
    std::set<str_t> utproj_names {}; // unique task project names
    for (auto &task: vtt) {
        if (task.tproj.size() < 2) {
            continue; // skip -> task without task project(s)
        }

        // TODO for each tproj element -> in case there are many

        str_t const project_name { task.tproj[0] }; // if more than one -> first task project

        // auto-associate tasks with the same project name to the same auto group
        // do not create new group if group with project name already exist in groups -> simply insert task to the group
        if (utproj_names.find(project_name) != utproj_names.end()) { // exist -> reuse already present group
            bool found = false;
            for (auto g: groups) { // find group to add task to
                if (g.gname == project_name) {
                    found = true;
                    g.tasks_t.insert(g.tasks_t.end(), task);
                    break;
                }
            }
            if (!found) {
                try { // XXX
                    throw std::runtime_error("Finished iterating over groups -> project_name, was not found!");
                } catch (std::runtime_error const& e) {
                    WNDX_LOG(LL::ERRO, "{}\n", e.what());
                    throw;
                }
            }
        } else { // not exist -> create & insert new group to the groups
            group_t group_t {}; // initialize with gid generation
            group_t.gname = project_name;
            group_t.words.insert(project_name);
            group_t.tasks_t.insert(task);
            groups.insert(group_t);
            utproj_names.insert(project_name); // since this is a set duplicates wont be added
        }
    }

    return groups;
}

} // namespace wndx::ss
