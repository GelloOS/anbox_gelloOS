#ifndef ANBOX_WAYLAND_WINDOW_H_
#define ANBOX_WAYLAND_WINDOW_H_

#include <memory>
#include <string>
#include <vector>
#include <thread>

#include <unistd.h>
#include <xf86drm.h>

// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>

#include "renderer.h"
#include "wayland_helper.h"

// #include "ui/gfx/geometry/size.h"
// #include "base/strings/stringprintf.h"
// #include "base/files/scoped_file.h"
// #include "base/time/time.h"
// #include "base/containers/circular_deque.h"

#include "../anbox/logger.h"
#include "../anbox/wm/window.h"
#include "../anbox/graphics/renderer.h"
// #include "external/android-emugl/host/include/OpenGLESDispatch/EGLDispatch.h"

namespace anbox{

class WaylandWindow: 
  public wm::Window,
  public fydeos::WaylandRenderer{
public:  
  struct Buffer {  
    std::unique_ptr<wl_buffer> buffer;
    bool busy = false;

    std::unique_ptr<gbm_bo> bo;
    EGLImageKHR egl_image;
    // std::unique_ptr<ScopedEglImage> egl_image;
    // std::unique_ptr<ScopedEglSync> egl_sync;
    // std::unique_ptr<ScopedTexture> texture;

    std::unique_ptr<zwp_linux_buffer_params_v1> params;
    // std::unique_ptr<base::SharedMemory> shared_memory;
    // std::unique_ptr<wl_shm_pool> shm_pool;    

    fydeos::Buffer_Ext ext;
  };

public:  
  std::shared_ptr<wl_display> display_;
  // std::unique_ptr<wl_display> display_;
  std::unique_ptr<wl_registry> registry_;
  std::unique_ptr<wl_surface> surface_;  
  std::unique_ptr<wl_pointer> pointer_;
  // std::unique_ptr<zxdg_surface_v6> xdg_surface_;
  // std::unique_ptr<wl_egl_window> window_;
  // std::unique_ptr<wl_shell_surface> shell_surface_;
  std::unique_ptr<zcr_remote_surface_v1> remote_shell_surface_;
  std::thread message_thread_;
  anbox::wm::Task::Id task_;

  // std::shared_ptr<anbox::graphics::Renderer> renderer_;
  // std::shared_ptr<Renderer> renderer_;

  const ::fydeos::Globals &globals_;
  
  int drm_fd_ = -1;
  std::unique_ptr<gbm_device> device_;

  bool transparent_background_ = false;
  int width_ = 1024;
  int height_ = 768;
  size_t num_buffers_ = 2;
  int scale_ = 1;
  int transform_ = WL_OUTPUT_TRANSFORM_NORMAL;
  int32_t drm_format_ = DRM_FORMAT_ARGB8888;
  int32_t bo_usage_ = GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING | GBM_BO_USE_TEXTURING;

  std::vector<std::unique_ptr<Buffer>> buffers_;
  bool y_invert_ = true;

public:
  WaylandWindow(
    const ::fydeos::Globals &globals,
    const std::shared_ptr<wl_display> &display,    
    const std::shared_ptr<Renderer> &renderer,
    const anbox::wm::Task::Id &task, 
    const anbox::graphics::Rect &frame, 
    const std::string &title);
  
  virtual ~WaylandWindow(){
    __asm__("int3");
  }

  bool init();

public:
  EGLNativeWindowType native_handle() const override {
    // return (EGLNativeWindowType)window_.get();
    // return (EGLNativeWindowType)dynamic_cast<fydeos::WaylandRenderer*>((WaylandWindow*)this);

    // DEBUG("native_handle: %llX %llX", 
    //   dynamic_cast<fydeos::WaylandRenderer*>((WaylandWindow*)this),
    //   dynamic_cast<const fydeos::WaylandRenderer*>(this)
    // );
    return (EGLNativeWindowType)dynamic_cast<const fydeos::WaylandRenderer*>(this);
    return (EGLNativeWindowType)task_;    
  }

public:
  anbox::fydeos::Buffer_Ext* bind() override;
  void unbind(anbox::fydeos::Buffer_Ext *pExt) override;

private:
  void messageLoop(){
    do{
      DEBUG("WaylandWindow::messageLoop %llX", pthread_self());
    // } while (wl_display_dispatch_queue(display_.get(), queue_.get()) != -1);  
    } while (wl_display_dispatch(display_.get()) != -1);  
  }

private:
  std::unique_ptr<Buffer> CreateBuffer(int width, int height,
                                       int32_t drm_format,
                                       int32_t bo_usage);
  std::unique_ptr<Buffer> CreateDrmBuffer(int width, int height,
                                          int32_t drm_format,
                                          int32_t bo_usage,
                                          bool y_invert);

  Buffer* DequeueBuffer();                                        
};

}
#endif