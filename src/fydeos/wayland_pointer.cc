#include "wayland_pointer.h"

namespace anbox{

WaylandPointer::WaylandPointer(
    const std::shared_ptr<wl_pointer> &pointer,
    AnboxInput *input_manager
  ):
  pointer_(pointer),input_manager_(input_manager){

  static const wl_pointer_listener listener = {
    &WaylandPointer::Enter,  
    &WaylandPointer::Leave, 
    &WaylandPointer::Motion,
    &WaylandPointer::Button, 
    &WaylandPointer::Axis,
  };

  wl_pointer_add_listener(pointer_.get(), &listener, this);
}

WaylandPointer::~WaylandPointer(){}

void WaylandPointer::Enter(void* data,
                           wl_pointer* obj,
                           uint32_t serial,
                           wl_surface* surface,
                           wl_fixed_t surface_x,
                           wl_fixed_t surface_y) {

  DEBUG("WaylandPointer::Enter");
}

void WaylandPointer::Leave(void* data,
                           wl_pointer* obj,
                           uint32_t serial,
                           wl_surface* surface) {                                        
  DEBUG("WaylandPointer::Leave");     
}

void WaylandPointer::Motion(void* data,
                            wl_pointer* obj,
                            uint32_t time,
                            wl_fixed_t surface_x,
                            wl_fixed_t surface_y) {                              
  DEBUG("WaylandPointer::Motion");     
}                   

void WaylandPointer::Button(void* data,
                            wl_pointer* obj,
                            uint32_t serial,
                            uint32_t time,
                            uint32_t button,
                            uint32_t state) {                              
  DEBUG("WaylandPointer::Button");     
}                              

void WaylandPointer::Axis(void* data,
                          wl_pointer* obj,
                          uint32_t time,
                          uint32_t axis,
                          wl_fixed_t value) {                            
  DEBUG("WaylandPointer::Axis");     
}

}