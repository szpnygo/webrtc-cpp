#include "webrtc.h"

constexpr static char kServer[] =
    "wss://signal.webrtc.tcode.ltd/base?auth=gowebrtc-signal";

int main() {
  webrtc::WebRTCApp app("test");
  app.setSignalServer(kServer);
  app.start();
  return 0;
}