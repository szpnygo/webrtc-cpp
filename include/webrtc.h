#pragma once
#include "signal.h"
#include "spdlog/spdlog.h"
#include "webrtc_app.h"
#include <iostream>
#include <memory>
#include <mutex>

namespace webrtc {

using namespace libwebrtc;

class WebRTCApp {
public:
  WebRTCApp(const std::string &name);

  ~WebRTCApp();

  // set the signal server url
  // use the gowebrtc-signal as the signal server backend
  // https://github.com/szpnygo/gowebrtc-signal
  void setSignalServer(const std::string &url);

  void start();

  void stop();

  // signal server on connect success
  void signalServerOnConnectSuccess();

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
};
} // namespace webrtc
