#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/utils/assert.hpp>

#include "../../../models/session.hpp"

namespace bookmarker {

struct TFilters {
  enum class ESortOrder {
    ID = 0,
    TITLE = 1,
    URL = 2,
    CREATED_TS = 3
  } order_by = ESortOrder::ID;

  size_t limit = 10;
  std::optional<std::string> tag;

  static std::optional<TFilters>
  Parse(const userver::server::http::HttpRequest &request);
};

struct DbQuery {
  std::string query;
  userver::storages::postgres::ParameterStore params;
};

DbQuery BuildDbRequest(const TSession &session, const TFilters &filters);

void AppendGetBookmarks(userver::components::ComponentList &component_list);

} // namespace bookmarker
