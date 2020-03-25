#ifndef ANBOX_WAYLAND_HELPER_H_
#define ANBOX_WAYLAND_HELPER_H_

#include <aura-shell-client-protocol.h>
#include <fcntl.h>
#include <fullscreen-shell-unstable-v1-client-protocol.h>
#include <linux-dmabuf-unstable-v1-client-protocol.h>
#include <linux-explicit-synchronization-unstable-v1-client-protocol.h>
// #include <linux-explicit-synchronization-unstable-v1-protocol.h>
#include <presentation-time-client-protocol.h>
#include <input-timestamps-unstable-v1-client-protocol.h>
#include <vsync-feedback-unstable-v1-client-protocol.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>


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
DEFAULT_DELETER_FDECL(wl_pointer)
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

#if defined(USE_GBM)
DEFAULT_DELETER_FDECL(gbm_bo)
DEFAULT_DELETER_FDECL(gbm_device)
#endif

#endif