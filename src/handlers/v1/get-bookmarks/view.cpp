#include "view.hpp"

#include <fmt/format.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/parameter_store.hpp>
#include <userver/utils/assert.hpp>

#include "../../../models/bookmark.hpp"
#include "../../lib/auth.hpp"

namespace bookmarker {

std::optional<TFilters>
TFilters::Parse(const userver::server::http::HttpRequest &request) {
  TFilters result;

  if (request.HasArg("limit")) {
    long long temp = std::stol(request.GetArg("limit").c_str());
    if (temp < 0)
      return std::nullopt;
    result.limit = static_cast<size_t>(temp);
  }

  if (request.HasArg("tag")) {
    result.tag = request.GetArg("tag");
  }

  if (request.HasArg("order_by")) {
    static std::unordered_map<std::string, TFilters::ESortOrder> mappings{
        {"id", TFilters::ESortOrder::ID},
        {"title", TFilters::ESortOrder::TITLE},
        {"url", TFilters::ESortOrder::URL},
        {"created_ts", TFilters::ESortOrder::CREATED_TS},
    };
    auto order_by = request.GetArg("order_by");
    auto it = mappings.find(order_by);
    if (it == mappings.end()) { // Сортировка по несуществующему параметру
      return std::nullopt;
    }
    result.order_by = it->second;
  }

  return result;
}

DbQuery BuildDbRequest(const TSession &session, const TFilters &filters) {
  std::ostringstream ss;
  userver::storages::postgres::ParameterStore params;
  ss << "SELECT * FROM bookmarker.bookmarks ";
  ss << "WHERE owner_id = $" << params.Size() + 1 << " ";
  params.PushBack(session.user_id);
  if (filters.tag.has_value()) {
    ss << "AND tag = $" << params.Size() + 1 << " ";
    params.PushBack(filters.tag.value());
  }
  ss << "ORDER BY ";
  switch (filters.order_by) {
  case TFilters::ESortOrder::ID:
    ss << "id ";
    break;
  case TFilters::ESortOrder::TITLE:
    ss << "title ";
    break;
  case TFilters::ESortOrder::URL:
    ss << "url ";
    break;
  case TFilters::ESortOrder::CREATED_TS:
    ss << "created_ts ";
    break;
  }
  ss << "LIMIT " << filters.limit;

  return {ss.str(), std::move(params)};
}

class GetBookmarks final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-v1-get-bookmarks";

  GetBookmarks(const userver::components::ComponentConfig &config,
               const userver::components::ComponentContext &component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {
    auto session_opt = GetSessionInfo(pg_cluster_, request);
    if (!session_opt.has_value()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error",
                                             "user is not authorized"));
    }

    auto session = session_opt.value();

    auto filters_opt = TFilters::Parse(request);
    if (!filters_opt.has_value()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error", "wrong query"));
    }

    auto filters = filters_opt.value();

    DbQuery param_query = BuildDbRequest(session, filters);
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave, param_query.query,
        param_query.params);

    userver::formats::json::ValueBuilder response;
    response["items"].Resize(0);
    for (auto row :
         result.AsSetOf<TBookmark>(userver::storages::postgres::kRowTag)) {
      response["items"].PushBack(row);
    }

    return userver::formats::json::ToString(response.ExtractValue());
  }

private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

void AppendGetBookmarks(userver::components::ComponentList &component_list) {
  component_list.Append<GetBookmarks>();
}

} // namespace bookmarker
