#pragma once

#include "wndx/sane/aliases.hpp"
#include "wndx/sane/log.hpp"

#include <string_view>
#include <vector>

namespace wndx {

#if 1
using ch_t = char;
using str_t = std::string;
using str_v = std::string_view;
#else
using ch_t = char8_t;
using str_t = std::u8string;
using str_v = std::u8string_view;
#endif
using vec_str_t = std::vector<str_t>;
using vec_s32_t = std::vector<s32>;

} // namespace wndx
