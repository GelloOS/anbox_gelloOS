#ifndef ANBOX_WAYLAND_PLATFORM_H_
#define ANBOX_WAYLAND_PLATFORM_H_

#include <map>
#include <thread>

#include "../anbox/platform/base_platform.h"
#include "../anbox/input/manager.h"
#include "../anbox/wm/manager.h"

#include "wayland_helper.h"
#include "wayland_window.h"

namespace anbox{

class WaylandPlatform: 
  public platform::BasePlatform,
  public AnboxInput{  
public:
  

private:
  std::shared_ptr<wl_display> display_;
  // std::unique_ptr<wl_display> display_;  
  std::unique_ptr<wl_registry> registry_;
  std::unique_ptr<wl_event_queue> queue_;
  std::unique_ptr<wl_callback> callback_;

  // std::shared_ptr<input::Manager> input_manager_;
  std::map<int32_t, std::weak_ptr<wm::Window>> windows_;
  std::shared_ptr<wm::Manager> window_manager_;
  std::shared_ptr<Renderer> renderer_;

  int32_t next_window_id_ = 0;

  ::fydeos::Globals globals_;
  std::thread message_thread_;

public:
  WaylandPlatform(const std::shared_ptr<input::Manager> &input_manager);
  virtual ~WaylandPlatform();
  std::shared_ptr<wm::Window> create_window(const anbox::wm::Task::Id &task, const anbox::graphics::Rect &frame, const std::string &title) override;

  void set_clipboard_data(const platform::BasePlatform::ClipboardData &data) override;
  platform::BasePlatform::ClipboardData get_clipboard_data() override;

  std::shared_ptr<audio::Sink> create_audio_sink() override;
  std::shared_ptr<audio::Source> create_audio_source() override { return nullptr; }

  void set_renderer(const std::shared_ptr<Renderer> &renderer) override;
  void set_window_manager(const std::shared_ptr<wm::Manager> &window_manager) override;

  bool supports_multi_window() const override;  

/*
public:
  void window_deleted(const anbox::wm::Task::Id &task) override {
    // window_manager_->remove_task(id);
  }
  void window_wants_focus(const std::int32_t &id) override {}
  void window_moved(const std::int32_t &id, const std::int32_t &x, const std::int32_t &y) override {}
  void window_resized(const std::int32_t &id, const std::int32_t &x, const std::int32_t &y) override {}
*/

public:
  wl_display* getDisplay(){ return display_.get(); }

private:
  void messageLoop();
};  
  
}

#endif