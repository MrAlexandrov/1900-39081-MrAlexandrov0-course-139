#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../models/bookmark.hpp"
#include "../../lib/auth.hpp"

namespace bookmarker {

namespace {

class AddBookmark final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-v1-add-bookmark";

  AddBookmark(const userver::components::ComponentConfig &config,
              const userver::components::ComponentContext &component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {
    auto session = GetSessionInfo(pg_cluster_, request);
    if (!session) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error",
                                             "user is not authorized"));
    }

    auto request_body =
        userver::formats::json::FromString(request.RequestBody());
    auto url = request_body["url"].As<std::optional<std::string>>();
    auto title = request_body["title"].As<std::optional<std::string>>();
    auto tag = request_body["tag"].As<std::optional<std::string>>();

    if (!url.has_value() || !title.has_value()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error",
                                             "invalid input parameters"));
    }

    auto existing_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT * FROM bookmarker.bookmarks "
        "WHERE owner_id = $1 AND url = $2",
        session->user_id, url.value());

    if (!existing_result.IsEmpty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kConflict);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject(
              "error", "bookmark with the same URL already exists"));
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "WITH ins AS ( "
        "INSERT INTO bookmarker.bookmarks(owner_id, url, title, tag) "
        "VALUES($1, $2, $3, $4) "
        "ON CONFLICT DO NOTHING "
        "RETURNING * "
        ") "
        "SELECT * FROM bookmarker.bookmarks WHERE url = $1 "
        "UNION ALL "
        "SELECT * FROM ins",
        session->user_id, url.value(), title.value(), tag);

    if (result.IsEmpty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(
          userver::server::http::HttpStatus::kInternalServerError);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error",
                                             "failed to add bookmark"));
    }

    auto bookmark =
        result.AsSingleRow<TBookmark>(userver::storages::postgres::kRowTag);
    return userver::formats::json::ToString(
        userver::formats::json::ValueBuilder{bookmark}.ExtractValue());
  }

private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace

void AppendAddBookmark(userver::components::ComponentList &component_list) {
  component_list.Append<AddBookmark>();
}

} // namespace bookmarker
