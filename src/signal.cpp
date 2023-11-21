#include "signal.h"
#include "ixwebsocket/IXNetSystem.h"
#include "spdlog/spdlog.h"
#include <string>

namespace webrtc {
SignalServer::SignalServer(const std::string &url) : _url(url) {
  ix::initNetSystem();
  _webSocket = std::make_shared<ix::WebSocket>();
}

SignalServer::~SignalServer() {}

bool SignalServer::send(const std::string &data, bool binary) {
  auto result = _webSocket->send(data, binary);
  return result.success;
}

void SignalServer::start() {
  _webSocket->setTLSOptions({});
  _webSocket->setUrl(_url);
  _webSocket->setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      onMessage(msg->str, msg->binary);
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      spdlog::info("Signal server connected");
      _onConnectSuccess();
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      spdlog::error("Signal server error {}", msg->errorInfo.reason);
    } else if (msg->type == ix::WebSocketMessageType::Close) {
      spdlog::info("Signal server closed");
    } else if (msg->type == ix::WebSocketMessageType::Ping) {

    } else if (msg->type == ix::WebSocketMessageType::Pong) {
    }
  });
  _webSocket->start();
}

void SignalServer::close() {
  _webSocket->disableAutomaticReconnection();
  _webSocket->close();
  ix::uninitNetSystem();
}

void SignalServer::stop() {
  _webSocket->stop();
  ix::uninitNetSystem();
}

void SignalServer::onMessage(const std::string &message, const bool binary) {
  json body = json::parse(message);
  std::string action = body["action"];
  if (action == "BindSuccess") {
    spdlog::info("bind success");
  } else if (action == "BindFail") {
    spdlog::warn("bind fail " + body["reason"].get<std::string>());
  } else if (action == "IncomingMessage") {
    json message = json::parse(body["content"].get<std::string>());
    onInComingMessage(body["sender"].get<std::string>(), message);
  }
}

void SignalServer::onInComingMessage(const std::string &sender,
                                     const json &message) {}

bool SignalServer::bindUser(const std::string name) {
  spdlog::info("bind user {}", name);
  BindUser user = {name};
  return send(user.dump());
}

} // namespace webrtc