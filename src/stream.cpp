#include "stream.h"
#include "spdlog/spdlog.h"

namespace webrtc {
VideoStream::VideoStream(scoped_refptr<RTCPeerConnectionFactory> factory,
                         int fps)
    : _factory(factory), _fps(fps) {
  try {
    _stream = _factory->CreateStream("video_stream");
    _capturer = _factory->GetVideoDevice()->CreateCapturer();
  } catch (const std::exception &e) {
    std::cerr << "Failed to create video stream or capturer: " << e.what()
              << std::endl;
    throw;
  }

  auto constraints = RTCMediaConstraints::Create();
  auto desktopSource =
      _factory->CreateVideoSource(_capturer, "random_video_input", constraints);
  auto tracker = _factory->CreateVideoTrack(desktopSource, "tracker_video");
  _stream->AddTrack(tracker);
};

VideoStream::~VideoStream() {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _is_running = false;
  }
  _cv.notify_one();
  if (_timer_thread.joinable()) {
    _timer_thread.join();
  }
}

void VideoStream::start() {
  std::lock_guard<std::mutex> lock(_mutex);
  _should_pause = false;
  if (!_is_running) {
    _is_running = true;
    _timer_thread = std::thread(&VideoStream::generate, this);
  }
  _cv.notify_one();
  spdlog::info("VideoStream start");
}

void VideoStream::stop() {
  std::lock_guard<std::mutex> lock(_mutex);
  _should_pause = true;
  spdlog::info("VideoStream stop");
}

void VideoStream::generate() {
  const std::chrono::milliseconds frameDuration(1000 / _fps);
  while (true) {
    {
      std::unique_lock<std::mutex> lock(_mutex);
      _cv.wait(lock, [this] { return !_should_pause || !_is_running; });
      if (!_is_running)
        break;
    }

    if (_should_pause) {
      continue; // 如果暂停，则跳过生成帧的操作
    }

    auto frameStart = std::chrono::steady_clock::now();
    auto frame = generateFrame();
    _capturer->GenerateFrame(frame);
    auto frameEnd = std::chrono::steady_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        frameEnd - frameStart);

    if (frameTime < frameDuration) {
      std::this_thread::sleep_for(frameDuration - frameTime);
    }
  }
}

} // namespace webrtc