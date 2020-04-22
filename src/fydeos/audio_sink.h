#ifndef ANBOX_FYDEROS_PLATFORM_AUDIO_SINK_H_
#define ANBOX_FYDEROS_PLATFORM_AUDIO_SINK_H_

#include "../anbox/audio/sink.h"
#include "../anbox/graphics/buffer_queue.h"

#include <thread>

namespace anbox {
namespace fydeos {

class AudioSink : public audio::Sink {
 public:
  AudioSink(){}
  ~AudioSink(){}

  void write_data(const std::vector<std::uint8_t> &data) override {
    DEBUG("AudioSink write_data %d", data.size());
  }

 private:
  // bool connect_audio();
  // void disconnect_audio();
  // void read_data(std::uint8_t *buffer, int size);

  // static void on_data_requested(void *user_data, std::uint8_t *buffer, int size);

  // std::mutex lock_;  
  // graphics::BufferQueue queue_;
  // graphics::Buffer read_buffer_;
  // size_t read_buffer_left_ = 0;
};

}
} // namespace platform

#endif
