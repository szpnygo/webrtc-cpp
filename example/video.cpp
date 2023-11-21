#include "stream.h"

using namespace webrtc;

class RandomVideoStream : public VideoStream {
public:
  RandomVideoStream(scoped_refptr<RTCPeerConnectionFactory> factory,
                    int fps = 30)
      : VideoStream(factory, fps) {}

  scoped_refptr<RTCVideoFrame> generateFrame() override {
    std::vector<uint8_t> data_y(width * height);
    std::vector<uint8_t> data_u(width * height / 4);
    std::vector<uint8_t> data_v(width * height / 4);

    // Fill Y, U and V buffers with random data.
    for (int i = 0; i < width * height; ++i) {
      data_y[i] = std::rand() % 256;
      if (i < width * height / 4) {
        data_u[i] = std::rand() % 256;
        data_v[i] = std::rand() % 256;
      }
    }

    // Add a growing white rectangle in the Y channel.
    int rect_size = frame_count % width;
    for (int y = 0; y < rect_size; ++y) {
      for (int x = 0; x < rect_size; ++x) {
        data_y[y * width + x] = 255;
      }
    }

    frame_count++;

    // Create RTCVideoFrame with random data.
    return RTCVideoFrame::Create(width, height, data_y.data(), width,
                                 data_u.data(), width / 2, data_v.data(),
                                 width / 2);
  }

private:
  int width = 640;
  int height = 480;
  int frame_count = 0;
};