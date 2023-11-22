#pragma once
#include "ixwebsocket/IXWebSocket.h"
#include "message.h"
#include "signal.h"
#include <functional>
#include <memory>
#include <string>

namespace webrtc {

typedef std::function<void()> OnConnectSuccess;
typedef std::function<void(const std::string)> OnNewConnectionRequest;
typedef std::function<void(const std::string, const AnswerMessage)>
    OnAnswerMessage;
typedef std::function<void(const std::string, const CandidateMessage)>
    OnCandidateMessage;

class ISignalServer {
public:
  virtual void sendOffer(const std::string sender, const std::string type,
                         const std::string sdp) = 0;

  virtual void sendCandidate(const std::string sender,
                             const std::string candidate, const std::string mid,
                             const int sdpMLineIndex) = 0;
};

class SignalServer : public ISignalServer {
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

  void onNewConnectionRequest(OnNewConnectionRequest onNewConnectionRequest) {
    _onNewConnectionRequest = onNewConnectionRequest;
  }

  void onAnswerMessage(OnAnswerMessage onAnswerMessage) {
    _onAnswerMessage = onAnswerMessage;
  }

  void onCandidateMessage(OnCandidateMessage onCandidateMessage) {
    _onCandidateMessage = onCandidateMessage;
  }

  void sendOffer(const std::string sender, const std::string type,
                 const std::string sdp) override;

  void sendCandidate(const std::string sender, const std::string candidate,
                     const std::string mid, const int sdpMLineIndex) override;

private:
  OnConnectSuccess _onConnectSuccess;
  OnNewConnectionRequest _onNewConnectionRequest;
  OnAnswerMessage _onAnswerMessage;
  OnCandidateMessage _onCandidateMessage;

  std::string _url;
  std::shared_ptr<ix::WebSocket> _webSocket;

  void onMessage(const std::string &message, const bool binary);

  void onInComingMessage(const std::string &sender, const json &message);
};
} // namespace webrtc
