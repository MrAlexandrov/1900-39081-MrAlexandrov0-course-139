#include "filter.hpp"

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

} // namespace bookmarker