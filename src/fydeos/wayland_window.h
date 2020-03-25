#ifndef ANBOX_WAYLAND_WINDOW_H_
#define ANBOX_WAYLAND_WINDOW_H_

#include <memory>
#include <string>
#include <vector>

#include "wayland_helper.h"

namespace anbox{

class WaylandWindow{
public:
  struct Globals {
    Globals();
    ~Globals();

    std::unique_ptr<wl_output> output;
    std::unique_ptr<wl_compositor> compositor;
    std::unique_ptr<wl_shm> shm;
    std::unique_ptr<wp_presentation> presentation;
    std::unique_ptr<zwp_linux_dmabuf_v1> linux_dmabuf;
    std::unique_ptr<wl_shell> shell;
    std::unique_ptr<wl_seat> seat;
    std::unique_ptr<wl_subcompositor> subcompositor;
    std::unique_ptr<wl_touch> touch;
    std::unique_ptr<zaura_shell> aura_shell;
    std::unique_ptr<zwp_fullscreen_shell_v1> fullscreen_shell;
    std::unique_ptr<zwp_input_timestamps_manager_v1> input_timestamps_manager;
    std::unique_ptr<zwp_linux_explicit_synchronization_v1> linux_explicit_synchronization;
    std::unique_ptr<zcr_vsync_feedback_v1> vsync_feedback;
  };

private:  
  std::unique_ptr<wl_display> display_;
  std::unique_ptr<wl_registry> registry_;
  std::unique_ptr<wl_surface> surface_;  
  std::unique_ptr<wl_shell_surface> shell_surface_;

public:
  WaylandWindow();
};

}
#endif