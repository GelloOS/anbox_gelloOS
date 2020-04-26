#ifndef ANBOX_WAYLAND_TOUCH_H_
#define ANBOX_WAYLAND_TOUCH_H_

#include "wayland_helper.h"
#include "../anbox/logger.h"
#include "wayland_window.h"
#include "anbox_input.h"

namespace anbox{

class WaylandTouch{
private:
  std::shared_ptr<wl_touch> touch_;
  int current_left_;
  int current_top_;

public:  
  AnboxInput *input_manager_;

public:
  WaylandTouch(
    const std::shared_ptr<wl_touch> &touch,
    AnboxInput *input_manager
  );
  ~WaylandTouch();

private:
  // wl_touch_listener
  static void Down(void* data,
                   wl_touch* obj,
                   uint32_t serial,
                   uint32_t time,
                   struct wl_surface* surface,
                   int32_t id,
                   wl_fixed_t x,
                   wl_fixed_t y){

    WaylandTouch *touch = (WaylandTouch*)data;    

    WaylandWindow *window = WaylandWindow::getWindowFromSurface(surface);

    // DEBUG("touch down window: %llX %llX", surface, window);

    int xx = window->current_rect_.left() + wl_fixed_to_double(x);    
    int yy = window->current_rect_.top() + wl_fixed_to_double(y);

    touch->current_left_ = window->current_rect_.left();
    touch->current_top_ = window->current_rect_.top();

    // int xx = wl_fixed_to_double(x);
    // int yy = wl_fixed_to_double(y);
     
    std::vector<input::Event> event;    
    touch->input_manager_->push_finger_down(xx, yy, id, event);  
    if (event.size() > 0){
      touch->input_manager_->touch_->send_events(event);
      DEBUG("WaylandTouch::Down %d %d %d %d %d %d", id, serial, time, xx, yy, event.size());
    }
  }

  static void Up(void* data,
                 wl_touch* obj,
                 uint32_t serial,
                 uint32_t time,
                 int32_t id){

    WaylandTouch *touch = (WaylandTouch*)data;    

    std::vector<input::Event> event;
    touch->input_manager_->push_finger_up(id, event);
    if (event.size() > 0){
      touch->input_manager_->touch_->send_events(event);
      DEBUG("WaylandTouch::Up %d %d %d %d", id, serial, time, event.size());
    }
  }

  static void Motion(void* data,
                     wl_touch* obj,
                     uint32_t time,
                     int32_t id,
                     wl_fixed_t x,
                     wl_fixed_t y){

    WaylandTouch *touch = (WaylandTouch*)data;                         

    // WaylandWindow *window = WaylandWindow::getWindowFromSurface(surface);

    // int xx = window->current_rect_.left() + wl_fixed_to_double(x);
    // int yy = window->current_rect_.top() + wl_fixed_to_double(y);

    int xx = touch->current_left_ + wl_fixed_to_double(x);
    int yy = touch->current_top_ + wl_fixed_to_double(y);

    std::vector<input::Event> event;
    touch->input_manager_->push_finger_motion(xx, yy, id, event);  
    if (event.size() > 0){
      touch->input_manager_->touch_->send_events(event);
      DEBUG("WaylandTouch::Motion %d %d %d %d %d", id, time, xx, yy, event.size());
    }
  }

  static void Frame(void* data, wl_touch* obj){
    DEBUG("WaylandTouch::Frame");
  }

  static void Cancel(void* data, wl_touch* obj){
    DEBUG("WaylandTouch::Cancel");
  }
}; 

}
#endif 