#include "wayland_touch.h"

namespace anbox{

WaylandTouch::WaylandTouch(
    const std::shared_ptr<wl_touch> &touch,
    AnboxInput *input_manager
  ):
  touch_(touch),
  input_manager_(input_manager){

  static const wl_touch_listener listener = {
      &WaylandTouch::Down,  
      &WaylandTouch::Up,     
      &WaylandTouch::Motion,
      &WaylandTouch::Frame, 
      &WaylandTouch::Cancel,
  };

  wl_touch_add_listener(touch_.get(), &listener, this);
}

WaylandTouch::~WaylandTouch(){}

}