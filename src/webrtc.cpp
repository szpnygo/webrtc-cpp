#include "webrtc.h"
#include <corecrt_math.h>

namespace webrtc {

WebRTCApp::WebRTCApp(const std::string &name) : _name(name) {
  LibWebRTC::Initialize();
}

WebRTCApp::~WebRTCApp() {}

void WebRTCApp::setSignalServer(const std::string &url) {
  _signalServer = std::make_shared<SignalServer>(url);
}

void WebRTCApp::start() {
  _signalServer->onConnectSuccess([this] { signalServerOnConnectSuccess(); });
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
} // namespace webrtc