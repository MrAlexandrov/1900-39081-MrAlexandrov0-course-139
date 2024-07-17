#pragma once

#include <userver/utils/regex.hpp>

#include <string_view>

namespace bookmarker {

static constexpr std::string_view email_string_pattern =
    R"(^\w+@[a-zA-Z]+\.[a-zA-Z]{2,6}$)";

static userver::utils::regex email_pattern{email_string_pattern};

// Хотя бы 8 символов
static constexpr std::string_view password_string_pattern =
    R"(^.{8,}$)";

static userver::utils::regex password_pattern{password_string_pattern};

} // namespace bookmarker