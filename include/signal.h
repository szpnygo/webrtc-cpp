#pragma once
#include "ixwebsocket/IXWebSocket.h"
#include "message.h"
#include <functional>
#include <memory>
#include <string>

namespace webrtc {

typedef std::function<void()> OnConnectSuccess;

class SignalServer {
public:
  SignalServer(const std::string &url);
  ~SignalServer();

  void start();

  void close();

  void stop();

  bool send(const std::string &data, bool binary = false);

  ix::ReadyState getReadyState() const { return _webSocket->getReadyState(); };

  std::string getUrl() const { return _url; }

  bool bindUser(const std::string name);

  void onConnectSuccess(OnConnectSuccess onConnectSuccess) {
    _onConnectSuccess = onConnectSuccess;
  }

private:
  OnConnectSuccess _onConnectSuccess;

  std::string _url;
  std::shared_ptr<ix::WebSocket> _webSocket;

  void onMessage(const std::string &message, const bool binary);

  void onInComingMessage(const std::string &sender, const json &message);
};
} // namespace webrtc
