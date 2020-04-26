#ifndef ANBOX_WAYLAND_KEYBOARD_H_
#define ANBOX_WAYLAND_KEYBOARD_H_

#include "wayland_helper.h"
#include "../anbox/logger.h"
#include "anbox_input.h"

namespace anbox{

class WaylandKeyboard{
private:
  std::shared_ptr<wl_keyboard> keyboard_;

public:  
  AnboxInput *input_manager_;
  
public:
  WaylandKeyboard(
    const std::shared_ptr<wl_keyboard> &keyboard_,
    AnboxInput *input_manager
  );
  ~WaylandKeyboard();

private:
// wl_keyboard_listener
  static void Keymap(void* data,
                     wl_keyboard* obj,
                     uint32_t format,
                     int32_t fd,
                     uint32_t size){                       
    DEBUG("WaylandKeyboard::Keymap");

    WaylandKeyboard *keyboard = (WaylandKeyboard*)data;      

  }

  static void Enter(void* data,
                    wl_keyboard* obj,
                    uint32_t serial,
                    wl_surface* surface,
                    wl_array* keys){                      
    DEBUG("WaylandKeyboard::Enter");

    WaylandKeyboard *keyboard = (WaylandKeyboard*)data;  
  }

  static void Leave(void* data,
                    wl_keyboard* obj,
                    uint32_t serial,
                    wl_surface* surface){                      
    DEBUG("WaylandKeyboard::Leave");

    WaylandKeyboard *keyboard = (WaylandKeyboard*)data;  
  }

  static void Key(void* data,
                  wl_keyboard* obj,
                  uint32_t serial,
                  uint32_t time,
                  uint32_t key,
                  uint32_t state){                    
    DEBUG("WaylandKeyboard::Key %d %d", key, state);

    WaylandKeyboard *keyboard = (WaylandKeyboard*)data;  

    std::vector<input::Event> keyboard_events;    
    if (state == WL_KEYBOARD_KEY_STATE_PRESSED){
      keyboard_events.push_back({EV_KEY, key, 1});
    }

    if (state == WL_KEYBOARD_KEY_STATE_RELEASED){
      keyboard_events.push_back({EV_KEY, key, 0});
    }      

    keyboard->input_manager_->keyboard_->send_events(keyboard_events);
  }

  static void Modifiers(void* data,
                        wl_keyboard* obj,
                        uint32_t serial,
                        uint32_t mods_depressed,
                        uint32_t mods_latched,
                        uint32_t mods_locked,
                        uint32_t group){                          
    DEBUG("WaylandKeyboard::Modifiers");
  
    WaylandKeyboard *keyboard = (WaylandKeyboard*)data;  
  }

  static void RepeatInfo(void* data,
                         wl_keyboard* obj,
                         int32_t rate,
                         int32_t delay){                           
    DEBUG("WaylandKeyboard::RepeatInfo");
    WaylandKeyboard *keyboard = (WaylandKeyboard*)data;
  }

  static void SyncCallback(void* data, struct wl_callback* cb, uint32_t time){
    __asm__("int3");
    DEBUG("WaylandKeyboard::SyncCallback");

  }
}; 

}
#endif 