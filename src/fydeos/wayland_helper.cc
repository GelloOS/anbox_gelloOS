#include "wayland_helper.h"

#include <input-timestamps-unstable-v1-client-protocol.h>
#include <linux-dmabuf-unstable-v1-client-protocol.h>
#include <linux-explicit-synchronization-unstable-v1-client-protocol.h>
#include <presentation-time-client-protocol.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

// Convenient macro that is used to define default deleters for object
// types allowing them to be used with std::unique_ptr.
#define DEFAULT_DELETER(TypeName, DeleteFunction)            \
  namespace std {                                            \
  void default_delete<TypeName>::operator()(TypeName* ptr) { \
    DeleteFunction(ptr);                                     \
  }                                                          \
  }

DEFAULT_DELETER(wl_buffer, wl_buffer_destroy)
DEFAULT_DELETER(wl_callback, wl_callback_destroy)
DEFAULT_DELETER(wl_compositor, wl_compositor_destroy)
DEFAULT_DELETER(wl_display, wl_display_disconnect)
DEFAULT_DELETER(wl_event_queue, wl_event_queue_destroy)
DEFAULT_DELETER(wl_pointer, wl_pointer_destroy)
DEFAULT_DELETER(wl_keyboard, wl_keyboard_destroy)
DEFAULT_DELETER(wl_region, wl_region_destroy)
DEFAULT_DELETER(wl_registry, wl_registry_destroy)
DEFAULT_DELETER(wl_seat, wl_seat_destroy)
DEFAULT_DELETER(wl_shell, wl_shell_destroy)
DEFAULT_DELETER(wl_shell_surface, wl_shell_surface_destroy)
DEFAULT_DELETER(wl_shm, wl_shm_destroy)
DEFAULT_DELETER(wl_shm_pool, wl_shm_pool_destroy)
DEFAULT_DELETER(wl_subcompositor, wl_subcompositor_destroy)
DEFAULT_DELETER(wl_subsurface, wl_subsurface_destroy)
DEFAULT_DELETER(wl_surface, wl_surface_destroy)
DEFAULT_DELETER(wl_touch, wl_touch_destroy)
DEFAULT_DELETER(wl_output, wl_output_destroy)
DEFAULT_DELETER(wp_presentation, wp_presentation_destroy)
DEFAULT_DELETER(struct wp_presentation_feedback,
                wp_presentation_feedback_destroy)
DEFAULT_DELETER(zaura_shell, zaura_shell_destroy)
DEFAULT_DELETER(zaura_surface, zaura_surface_destroy)
DEFAULT_DELETER(zaura_output, zaura_output_destroy)
DEFAULT_DELETER(zwp_linux_buffer_release_v1,
                zwp_linux_buffer_release_v1_destroy)
DEFAULT_DELETER(zwp_fullscreen_shell_v1, zwp_fullscreen_shell_v1_destroy)
DEFAULT_DELETER(zwp_input_timestamps_manager_v1,
                zwp_input_timestamps_manager_v1_destroy)
DEFAULT_DELETER(zwp_input_timestamps_v1, zwp_input_timestamps_v1_destroy)
DEFAULT_DELETER(zwp_linux_buffer_params_v1, zwp_linux_buffer_params_v1_destroy)
DEFAULT_DELETER(zwp_linux_dmabuf_v1, zwp_linux_dmabuf_v1_destroy)
DEFAULT_DELETER(zwp_linux_explicit_synchronization_v1,
                zwp_linux_explicit_synchronization_v1_destroy)
DEFAULT_DELETER(zwp_linux_surface_synchronization_v1,
                zwp_linux_surface_synchronization_v1_destroy)
DEFAULT_DELETER(zcr_remote_shell_v1, zcr_remote_shell_v1_destroy)
DEFAULT_DELETER(zcr_remote_surface_v1, zcr_remote_surface_v1_destroy)
DEFAULT_DELETER(zcr_input_method_surface_v1, zcr_input_method_surface_v1_destroy)
DEFAULT_DELETER(wp_viewporter, wp_viewporter_destroy)
DEFAULT_DELETER(wp_viewport, wp_viewport_destroy)
DEFAULT_DELETER(zxdg_shell_v6, zxdg_shell_v6_destroy)
DEFAULT_DELETER(zxdg_surface_v6, zxdg_surface_v6_destroy)
// DEFAULT_DELETER(wl_egl_window, wl_egl_window_destroy)

#if defined(USE_GBM)
DEFAULT_DELETER(gbm_bo, gbm_bo_destroy)
DEFAULT_DELETER(gbm_device, gbm_device_destroy)
#endif