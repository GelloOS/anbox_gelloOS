#include "wayland_keyboard.h"

namespace anbox{

WaylandKeyboard::WaylandKeyboard(
    const std::shared_ptr<wl_keyboard> &keyboard,
    AnboxInput *input_manager
  ):
  keyboard_(keyboard),
  input_manager_(input_manager){

  static const wl_keyboard_listener listener = {
      &WaylandKeyboard::Keymap,    
      &WaylandKeyboard::Enter,
      &WaylandKeyboard::Leave,     
      &WaylandKeyboard::Key,
      &WaylandKeyboard::Modifiers, 
      &WaylandKeyboard::RepeatInfo,
  };

  wl_keyboard_add_listener(keyboard.get(), &listener, this);    
}

WaylandKeyboard::~WaylandKeyboard(){}

}