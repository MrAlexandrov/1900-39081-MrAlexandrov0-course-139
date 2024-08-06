#pragma once


#include <userver/components/component_list.hpp>

#include "../../../models/session.hpp"

namespace bookmarker {

void AppendGetBookmarks(userver::components::ComponentList &component_list);

} // namespace bookmarker
