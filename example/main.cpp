#include "video.cpp"
#include "webrtc.h"
#include <memory>

constexpr static char kServer[] =
    "wss://signal.webrtc.tcode.ltd/base?auth=gowebrtc-signal";

int main() {

  webrtc::WebRTCApp app("example");
  app.setSignalServer(kServer);
  auto stream = std::make_shared<RandomVideoStream>(app.getFactory());
  app.setVideoStream(stream);
  app.start();
  return 0;
}