#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../../models/user.hpp"

namespace bookmarker {

namespace {

class LoginUser final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-login-user";

  LoginUser(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {
    auto email_opt = request.GetFormDataArg("email");
    auto password_opt = request.GetFormDataArg("password");

    if (email_opt.value.empty() || password_opt.value.empty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error",
                                             "missing required parameters"));
    }

    auto email = email_opt.value;
    auto password = userver::crypto::hash::Sha256(password_opt.value);

    auto userResult = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT * FROM bookmarker.users "
        "WHERE email = $1 ",
        email);

    if (userResult.IsEmpty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kNotFound);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject(
              "error", "user with this email is not registered"));
    }

    auto user =
        userResult.AsSingleRow<TUser>(userver::storages::postgres::kRowTag);
    if (password != user.password) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kNotFound);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error", "wrong password"));
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO bookmarker.auth_sessions(user_id) VALUES($1) "
        "ON CONFLICT (user_id) DO UPDATE "
        "SET id = uuid_generate_v4() "
        "RETURNING id ",
        user.id);

    if (result.IsEmpty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(
          userver::server::http::HttpStatus::kInternalServerError);
      return userver::formats::json::ToString(
          userver::formats::json::MakeObject("error",
                                             "failed to create session"));
    }

    userver::formats::json::ValueBuilder response;
    response["id"] = result.AsSingleRow<std::string>();

    return userver::formats::json::ToString(response.ExtractValue());
  }

private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace

void AppendLoginUser(userver::components::ComponentList &component_list) {
  component_list.Append<LoginUser>();
}

} // namespace bookmarker
