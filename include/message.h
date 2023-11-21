#pragma once
#include <nlohmann/json.hpp>

namespace webrtc {

using json = nlohmann::json;

struct BindUser {
  std::string name;

  json toJSON() const { return {{"action", "BindUser"}, {"name", name}}; };

  std::string dump() { return toJSON().dump(); };
};
} // namespace webrtc
