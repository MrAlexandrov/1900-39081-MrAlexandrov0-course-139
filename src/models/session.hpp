#pragma once

#include <string>

namespace bookmarker {

struct TSession {
  std::string id;
  std::string user_id;
};

} // namespace bookmarker