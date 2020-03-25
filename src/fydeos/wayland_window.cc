#include "wayland_window.h"

// #include "base/syslog_logging.h"
// #include "base/at_exit.h"
// #include "base/command_line.h"
// #include "base/message_loop/message_loop.h"

// #include "components/exo/wayland/clients/client_base.h"


namespace anbox{

void RegistryHandler(void* data,
                     wl_registry* registry,
                     uint32_t id,
                     const char* interface,
                     uint32_t version) {
  WaylandWindow::Globals* globals = static_cast<WaylandWindow::Globals*>(data);  
   
  if (strcmp(interface, "wl_compositor") == 0) {    
    globals->compositor.reset(static_cast<wl_compositor*>(
        wl_registry_bind(registry, id, &wl_compositor_interface, 3)));
  } else if (strcmp(interface, "wl_shm") == 0) {
    globals->shm.reset(static_cast<wl_shm*>(
        wl_registry_bind(registry, id, &wl_shm_interface, 1)));
  } else if (strcmp(interface, "wl_shell") == 0) {
    globals->shell.reset(static_cast<wl_shell*>(
        wl_registry_bind(registry, id, &wl_shell_interface, 1)));
  } else if (strcmp(interface, "wl_seat") == 0) {
    globals->seat.reset(static_cast<wl_seat*>(
        wl_registry_bind(registry, id, &wl_seat_interface, 5)));
  } else if (strcmp(interface, "wp_presentation") == 0) {
    globals->presentation.reset(static_cast<wp_presentation*>(
        wl_registry_bind(registry, id, &wp_presentation_interface, 1)));
  } else if (strcmp(interface, "zaura_shell") == 0) {
    globals->aura_shell.reset(static_cast<zaura_shell*>(
        wl_registry_bind(registry, id, &zaura_shell_interface, 5)));
  } else if (strcmp(interface, "zwp_linux_dmabuf_v1") == 0) {
    globals->linux_dmabuf.reset(static_cast<zwp_linux_dmabuf_v1*>(
        wl_registry_bind(registry, id, &zwp_linux_dmabuf_v1_interface, 2)));
  } else if (strcmp(interface, "wl_subcompositor") == 0) {
    globals->subcompositor.reset(static_cast<wl_subcompositor*>(
        wl_registry_bind(registry, id, &wl_subcompositor_interface, 1)));
  } else if (strcmp(interface, "zwp_input_timestamps_manager_v1") == 0) {
    globals->input_timestamps_manager.reset(
        static_cast<zwp_input_timestamps_manager_v1*>(wl_registry_bind(
            registry, id, &zwp_input_timestamps_manager_v1_interface, 1)));
  } else if (strcmp(interface, "zwp_fullscreen_shell_v1") == 0) {
    globals->fullscreen_shell.reset(static_cast<zwp_fullscreen_shell_v1*>(
        wl_registry_bind(registry, id, &zwp_fullscreen_shell_v1_interface, 1)));
  } else if (strcmp(interface, "wl_output") == 0) {
    globals->output.reset(static_cast<wl_output*>(
        wl_registry_bind(registry, id, &wl_output_interface, 1)));
  } else if (strcmp(interface, "zwp_linux_explicit_synchronization_v1") == 0) {
    globals->linux_explicit_synchronization.reset(
        static_cast<zwp_linux_explicit_synchronization_v1*>(wl_registry_bind(
            registry, id, &zwp_linux_explicit_synchronization_v1_interface,
            1)));
  }  
}

WaylandWindow::WaylandWindow(){
  display_.reset(wl_display_connect(nullptr));
  registry_.reset(wl_display_get_registry(display_.get()));  
}

}