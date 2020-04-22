#ifndef ANBOX_WAYLAND_HELPER_H_
#define ANBOX_WAYLAND_HELPER_H_

#include <fcntl.h>

#include <aura-shell-client-protocol.h>
#include <remote-shell-unstable-v1-client-protocol.h>
#include <viewporter-client-protocol.h>
#include <fullscreen-shell-unstable-v1-client-protocol.h>
#include <linux-dmabuf-unstable-v1-client-protocol.h>
#include <linux-explicit-synchronization-unstable-v1-client-protocol.h>
#include <xdg-shell-unstable-v6-client-protocol.h>
// #include <linux-explicit-synchronization-unstable-v1-protocol.h>
#include <presentation-time-client-protocol.h>
#include <input-timestamps-unstable-v1-client-protocol.h>
#include <vsync-feedback-unstable-v1-client-protocol.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
// #include <wayland-client-protocol-core.h>

// #include <wayland-egl.h>
#include <memory>

#include <drm_fourcc.h>
#include <gbm.h>

// Default deleters template specialization forward decl.
#define DEFAULT_DELETER_FDECL(TypeName) \
  namespace std {                       \
  template <>                           \
  struct default_delete<TypeName> {     \
    void operator()(TypeName* ptr);     \
  };                                    \
  }

DEFAULT_DELETER_FDECL(wl_buffer)
DEFAULT_DELETER_FDECL(wl_callback)
DEFAULT_DELETER_FDECL(wl_compositor)
DEFAULT_DELETER_FDECL(wl_display)
DEFAULT_DELETER_FDECL(wl_event_queue)
DEFAULT_DELETER_FDECL(wl_pointer)
DEFAULT_DELETER_FDECL(wl_keyboard)
DEFAULT_DELETER_FDECL(wl_region)
DEFAULT_DELETER_FDECL(wl_registry)
DEFAULT_DELETER_FDECL(wl_seat)
DEFAULT_DELETER_FDECL(wl_shell)
DEFAULT_DELETER_FDECL(wl_shell_surface)
DEFAULT_DELETER_FDECL(wl_shm)
DEFAULT_DELETER_FDECL(wl_shm_pool)
DEFAULT_DELETER_FDECL(wl_subcompositor)
DEFAULT_DELETER_FDECL(wl_subsurface)
DEFAULT_DELETER_FDECL(wl_surface)
DEFAULT_DELETER_FDECL(wl_touch)
DEFAULT_DELETER_FDECL(wl_output)
DEFAULT_DELETER_FDECL(wp_presentation)
DEFAULT_DELETER_FDECL(struct wp_presentation_feedback)
DEFAULT_DELETER_FDECL(zaura_shell)
DEFAULT_DELETER_FDECL(zaura_surface)
DEFAULT_DELETER_FDECL(zaura_output)
DEFAULT_DELETER_FDECL(zwp_linux_buffer_release_v1)
DEFAULT_DELETER_FDECL(zwp_fullscreen_shell_v1)
DEFAULT_DELETER_FDECL(zwp_input_timestamps_manager_v1)
DEFAULT_DELETER_FDECL(zwp_input_timestamps_v1)
DEFAULT_DELETER_FDECL(zwp_linux_buffer_params_v1)
DEFAULT_DELETER_FDECL(zwp_linux_dmabuf_v1)
DEFAULT_DELETER_FDECL(zwp_linux_explicit_synchronization_v1)
DEFAULT_DELETER_FDECL(zwp_linux_surface_synchronization_v1)
DEFAULT_DELETER_FDECL(zcr_remote_shell_v1)
DEFAULT_DELETER_FDECL(zcr_remote_surface_v1)
DEFAULT_DELETER_FDECL(zcr_input_method_surface_v1)
DEFAULT_DELETER_FDECL(wp_viewporter)
DEFAULT_DELETER_FDECL(wp_viewport)
DEFAULT_DELETER_FDECL(zxdg_shell_v6)
DEFAULT_DELETER_FDECL(zxdg_surface_v6)
// DEFAULT_DELETER_FDECL(wl_egl_window)

#if defined(USE_GBM)
DEFAULT_DELETER_FDECL(gbm_bo)
DEFAULT_DELETER_FDECL(gbm_device)
#endif

namespace fydeos{

struct Globals {
  // Globals() = default;
  // ~Globals() = default;

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
  std::unique_ptr<zcr_remote_shell_v1> remote_shell;
  std::unique_ptr<wp_viewporter> viewporter;
  std::unique_ptr<zxdg_shell_v6> zxdg_shell;  
  
  // std::unique_ptr<zcr_vsync_feedback_v1> vsync_feedback;
};

}

#endif