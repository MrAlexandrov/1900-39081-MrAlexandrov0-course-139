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

#include "../../lib/auth.hpp"
#include "../../../models/bookmark.hpp"
#include "../../../models/filter.hpp"
#include "../../../models/session.hpp"

namespace bookmarker {

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
