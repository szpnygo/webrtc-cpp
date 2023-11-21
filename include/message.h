#pragma once
#include "message.h"
#include <nlohmann/json.hpp>
#include <string>

namespace webrtc {

using json = nlohmann::json;

struct BindUser {
  std::string name;

  std::string dump() {
    return json{{"action", "BindUser"}, {"name", name}}.dump();
  };
};

struct OutgoingMessage {
  std::string recipient;

  std::string content;

  std::string dump() {
    return json{{"action", "OutgoingMessage"},
                {"recipient", recipient},
                {"content", content}}
        .dump();
  };
};

struct OfferMessage {
  std::string sdp;
  std::string type;

  std::string dump() {
    return json{{"action", "offer"}, {"sdp", sdp}, {"type", type}}.dump();
  };
};

struct AnswerMessage {
  std::string sdp;
  std::string type;

  std::string dump() {
    return json{{"action", "answer"}, {"sdp", sdp}, {"type", type}}.dump();
  };
};

struct CandidateMessage {
  std::string candidate;
  std::string mid;
  int sdpMLineIndex;

  std::string dump() {
    return json{{"action", "candidate"},
                {"candidate", candidate},
                {"sdpMid", mid},
                {"sdpMLineIndex", sdpMLineIndex}}
        .dump();
  };
};

} // namespace webrtc
