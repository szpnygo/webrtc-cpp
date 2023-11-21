#pragma once
#include "webrtc_app.h"
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace webrtc {

using namespace libwebrtc;

class VideoStream {
public:
  VideoStream(scoped_refptr<RTCPeerConnectionFactory> factory, int fps = 30);

  ~VideoStream();

  void start();

  void stop();

  virtual scoped_refptr<RTCVideoFrame> generateFrame() = 0;

  scoped_refptr<RTCMediaStream> getStream() { return _stream; }

  bool isPlaying() { return _is_running && !_should_pause; }

private:
  scoped_refptr<RTCMediaStream> _stream;
  scoped_refptr<RTCVideoCapturer> _capturer;
  scoped_refptr<RTCPeerConnectionFactory> _factory;

  std::thread _timer_thread;
  int _fps;
  std::mutex _mutex;
  std::condition_variable _cv;
  bool _is_running = false;
  bool _should_pause = true;

  void generate();
};
} // namespace webrtc