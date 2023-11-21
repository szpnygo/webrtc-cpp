#include "webrtc.h"
#include "libwebrtc.h"

namespace webrtc {

WebRTCApp::WebRTCApp(const std::string &name) : _name(name) {
  LibWebRTC::Initialize();
  _factory = LibWebRTC::CreateRTCPeerConnectionFactory();
}

WebRTCApp::~WebRTCApp() {}

void WebRTCApp::setSignalServer(const std::string &url) {
  _signalServer = std::make_shared<SignalServer>(url);
}

void WebRTCApp::start() {
  _signalServer->onConnectSuccess([this] { signalServerOnConnectSuccess(); });
  _signalServer->onNewConnectionRequest(
      [this](const std::string &name) { onNewConnectionRequest(name); });
  _signalServer->onAnswerMessage(
      [this](const std::string &name, const AnswerMessage &message) {
        if (_connections.find(name) != _connections.end()) {
          _connections[name]->setAnswer(message.type, message.sdp);
        }
      });
  _signalServer->onCandidateMessage(
      [this](const std::string &name, const CandidateMessage &message) {
        if (_connections.find(name) != _connections.end()) {
          _connections[name]->addCandidate(message.candidate, message.mid,
                                           message.sdpMLineIndex);
        }
      });

  _signalServer->start();

  spdlog::info("WebRTCApp start");

  std::unique_lock<std::mutex> lock(_mutex);
  _cv.wait(lock, [this] { return _isStop; });
}

void WebRTCApp::signalServerOnConnectSuccess() {
  _signalServer->bindUser(_name);
}

void WebRTCApp::stop() {
  std::unique_lock<std::mutex> lock(_mutex);
  _isStop = true;
  _cv.notify_all();
}

scoped_refptr<RTCPeerConnection> WebRTCApp::createPeerConnection() {
  RTCConfiguration config;
  config.ice_servers[0] =
      IceServer{"stun:coturn.tcodestudio.com:31000", "", ""};
  auto constraints = RTCMediaConstraints::Create();
  auto pc = _factory->Create(config, constraints);

  auto stream = _stream->getStream();

  auto codecs = _factory->GetRtpSenderCapabilities(RTCMediaType::VIDEO);
  auto codecsVector = codecs->codecs().std_vector();
  std::vector<std::string> order = {"video/AV1"};
  std::stable_sort(codecsVector.begin(), codecsVector.end(),
                   [&order](const auto &a, const auto &b) {
                     auto pos_a = std::distance(
                         order.begin(), std::find(order.begin(), order.end(),
                                                  a->mime_type().std_string()));
                     auto pos_b = std::distance(
                         order.begin(), std::find(order.begin(), order.end(),
                                                  b->mime_type().std_string()));
                     return pos_a < pos_b;
                   });

  std::vector<string> std_vec = {stream->id()};
  for (const auto &item : stream->video_tracks().std_vector()) {
    auto sender = pc->AddTrack(item, std_vec);
    if (sender) {
      for (const auto &transceiver : pc->transceivers().std_vector()) {
        if (transceiver->sender()->id().std_string() ==
            sender->id().std_string()) {
          transceiver->SetCodecPreferences(codecsVector);
        }
      }
    }
  }

  return pc;
}

void WebRTCApp::onNewConnectionRequest(const std::string &name) {
  // check if the connection already exists
  if (_connections.find(name) != _connections.end()) {
    spdlog::info("Connection already exists {}", name);
    return;
  }
  auto conn =
      std::make_shared<Connection>(name, _signalServer, createPeerConnection());
  addConnection(name, conn);
  conn->setOnConnected([this, name]() {
    spdlog::info("onConnected {}", name);
    if (_stream && !_stream->isPlaying()) {
      _stream->start();
    }
  });
  conn->setOnDisconnected([this, name]() {
    spdlog::info("onDisconnected {}", name);
    removeConnection(name);
  });
  conn->start();
}

void WebRTCApp::addConnection(const std::string &name,
                              std::shared_ptr<Connection> connection) {
  if (_connections.find(name) != _connections.end()) {
    _connections[name]->close();
    _factory->Delete(connection->getPeerConnection());
  }
  _connections[name] = connection;
}

void WebRTCApp::removeConnection(const std::string &name) {
  if (_connections.find(name) != _connections.end()) {
    _factory->Delete(_connections[name]->getPeerConnection());
  }
  _connections.erase(name);
  if (_connections.size() == 0 && _stream && _stream->isPlaying()) {
    _stream->stop();
  }
}

} // namespace webrtc