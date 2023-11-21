#pragma once
#include "connection.h"
#include "signal.h"
#include "spdlog/spdlog.h"
#include "stream.h"
#include "webrtc_app.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>

namespace webrtc {

using namespace libwebrtc;

class WebRTCApp {
public:
  WebRTCApp(const std::string &name);

  ~WebRTCApp();

  void setVideoStream(std::shared_ptr<VideoStream> stream) {
    _stream = stream;
  };

  // set the signal server url
  // use the gowebrtc-signal as the signal server backend
  // https://github.com/szpnygo/gowebrtc-signal
  void setSignalServer(const std::string &url);

  void start();

  void stop();

  scoped_refptr<RTCPeerConnectionFactory> getFactory() { return _factory; }

protected:
private:
  // the name of the user
  std::string _name;
  // the signal server
  std::shared_ptr<SignalServer> _signalServer;

  // control the server stop
  std::mutex _mutex;
  std::condition_variable _cv;
  bool _isStop = false;

  // the connections, key is the name of the client
  std::unordered_map<std::string, std::shared_ptr<Connection>> _connections;
  // the peer connection factory
  scoped_refptr<RTCPeerConnectionFactory> _factory;

  std::shared_ptr<VideoStream> _stream;

  // signal server on connect success
  void signalServerOnConnectSuccess();

  // signal server on new connection request
  // name is the name of the client
  void onNewConnectionRequest(const std::string &name);

  // create a new connection for the client
  void createConnection(const std::string &name);

  // common function to create a peer connection
  scoped_refptr<RTCPeerConnection> createPeerConnection();

  void addConnection(const std::string &name,
                     std::shared_ptr<Connection> connection);

  void removeConnection(const std::string &name);
};
} // namespace webrtc
