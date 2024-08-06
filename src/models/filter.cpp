#include "filter.hpp"

namespace bookmarker {

std::optional<TFilters> 
TFilters::Parse(const std::optional<std::string>& limit_str,
                const std::optional<std::string>& tag_str,
                const std::optional<std::string>& order_by_str) {
  TFilters result;

  if (limit_str) {
    try {
      long long temp = std::stol(limit_str.value());
      if (temp < 0) {
        return std::nullopt;
      }
      result.limit = static_cast<size_t>(temp);
    } catch (const std::exception&) {
      return std::nullopt;
    }
  }

  if (tag_str) {
    result.tag = tag_str;
  }

  if (order_by_str) {
    static const std::unordered_map<std::string, TFilters::ESortOrder> mappings{
      {"id", TFilters::ESortOrder::ID},
      {"title", TFilters::ESortOrder::TITLE},
      {"url", TFilters::ESortOrder::URL},
      {"created_ts", TFilters::ESortOrder::CREATED_TS},
    };
    auto it = mappings.find(order_by_str.value());
    if (it == mappings.end()) {
      return std::nullopt;
    }
    result.order_by = it->second;
  }

  return result;
}

std::optional<TFilters>
TFilters::Parse(const userver::server::http::HttpRequest &request) {
  std::optional<std::string> limit;
  if (request.HasArg("limit")) {
    limit = request.GetArg("limit");
  } else {
    limit = std::nullopt;
  }
  std::optional<std::string> tag;
  if (request.HasArg("tag")) {
    tag = request.GetArg("tag");
  } else {
    tag = std::nullopt;
  }
  std::optional<std::string> order_by;
  if (request.HasArg("order_by")) {
    order_by = request.GetArg("order_by");
  } else {
    order_by = std::nullopt;
  }
  return Parse(limit,
               tag,
               order_by
  );
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