#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace bookmarker {

void AppendGetBookmark(userver::components::ComponentList &component_list);

} // namespace bookmarker
