#ifndef ANBOX_FYDEOS_INPUT_H_
#define ANBOX_FYDEOS_INPUT_H_

#include <vector>
#include <map>
#include <thread>

#include "../anbox/input/device.h"
#include "../anbox/input/manager.h"
#include "../anbox/graphics/rect.h"
#include "../anbox/logger.h"
#include "../anbox/graphics/emugl/DisplayManager.h"

namespace anbox{

class AnboxInput{
public:
  graphics::Rect display_frame_;
  std::shared_ptr<input::Manager> input_manager_;

  std::shared_ptr<input::Device> pointer_;
  std::shared_ptr<input::Device> keyboard_;
  std::shared_ptr<input::Device> touch_;

  static const int MAX_FINGERS = 10;
  static const int MAX_TRACKING_ID = 10;
  int touch_slots[MAX_FINGERS];
  int last_slot = -1;

public:
  AnboxInput(const std::shared_ptr<input::Manager> &input_manager):    
    input_manager_(input_manager){            
  }

  void init(graphics::Rect &display_frame){
    graphics::emugl::DisplayInfo::get()->set_resolution(display_frame.width(), display_frame.height());
    display_frame_ = display_frame;

    pointer_ = input_manager_->create_device();
    pointer_->set_name("anbox-pointer");
    pointer_->set_driver_version(1);
    pointer_->set_input_id({BUS_VIRTUAL, 2, 2, 2});
    pointer_->set_physical_location("none");
    pointer_->set_key_bit(BTN_MOUSE);
    // NOTE: We don't use REL_X/REL_Y in reality but have to specify them here
    // to allow InputFlinger to detect we're a cursor device.
    pointer_->set_rel_bit(REL_X);
    pointer_->set_rel_bit(REL_Y);
    pointer_->set_rel_bit(REL_HWHEEL);
    pointer_->set_rel_bit(REL_WHEEL);
    pointer_->set_prop_bit(INPUT_PROP_POINTER);

    keyboard_ = input_manager_->create_device();
    keyboard_->set_name("anbox-keyboard");
    keyboard_->set_driver_version(1);
    keyboard_->set_input_id({BUS_VIRTUAL, 3, 3, 3});
    keyboard_->set_physical_location("none");
    keyboard_->set_key_bit(BTN_MISC);
    keyboard_->set_key_bit(KEY_OK);

    touch_ = input_manager_->create_device();
    touch_->set_name("anbox-touch");
    touch_->set_driver_version(1);
    touch_->set_input_id({BUS_VIRTUAL, 4, 4, 4});
    touch_->set_physical_location("none");
    touch_->set_abs_bit(ABS_MT_SLOT);
    touch_->set_abs_max(ABS_MT_SLOT, 10);
    touch_->set_abs_bit(ABS_MT_TOUCH_MAJOR);
    touch_->set_abs_max(ABS_MT_TOUCH_MAJOR, 127);
    touch_->set_abs_bit(ABS_MT_TOUCH_MINOR);
    touch_->set_abs_max(ABS_MT_TOUCH_MINOR, 127);
    touch_->set_abs_bit(ABS_MT_POSITION_X);
    touch_->set_abs_max(ABS_MT_POSITION_X, display_frame.width());
    touch_->set_abs_bit(ABS_MT_POSITION_Y);
    touch_->set_abs_max(ABS_MT_POSITION_Y, display_frame.height());
    touch_->set_abs_bit(ABS_MT_TRACKING_ID);
    touch_->set_abs_max(ABS_MT_TRACKING_ID, MAX_TRACKING_ID);
    touch_->set_prop_bit(INPUT_PROP_DIRECT);

    for (int i = 0; i < MAX_FINGERS; i++){
      touch_slots[i] = -1;      
    }

    DEBUG("AnboxInput width: %d, height: %d", display_frame.width(), display_frame.height());
  }

private:
  int find_touch_slot(int id){
    for (int i = 0; i < MAX_FINGERS; i++) {      
      if (touch_slots[i] == id)
        return i;
    }

    DEBUG("not found touch slot.");
    return -1;
  }

public:
  void push_slot(std::vector<input::Event> &touch_events, int slot){
    if (last_slot != slot) {
        touch_events.push_back({EV_ABS, ABS_MT_SLOT, slot});
        last_slot = slot;
    }
  }

  void push_finger_down(int x, int y, int finger_id, std::vector<input::Event> &touch_events){
    int slot = find_touch_slot(-1);
    if (slot == -1) {        
        return;
    }
    touch_slots[slot] = finger_id;
    push_slot(touch_events, slot);
    touch_events.push_back({EV_ABS, ABS_MT_TRACKING_ID, static_cast<std::int32_t>(finger_id % MAX_TRACKING_ID + 1)});
    touch_events.push_back({EV_ABS, ABS_MT_POSITION_X, x});
    touch_events.push_back({EV_ABS, ABS_MT_POSITION_Y, y});
    touch_events.push_back({EV_SYN, SYN_REPORT, 0});
  }  

  void push_finger_up(int finger_id, std::vector<input::Event> &touch_events){
    int slot = find_touch_slot(finger_id);
    if (slot == -1) 
      return;
    push_slot(touch_events, slot);
    touch_events.push_back({EV_ABS, ABS_MT_TRACKING_ID, -1});
    touch_events.push_back({EV_SYN, SYN_REPORT, 0});
    touch_slots[slot] = -1;
  }

  void push_finger_motion(int x, int y, int finger_id, std::vector<input::Event> &touch_events){
    int slot = find_touch_slot(finger_id);
    if (slot == -1) 
      return;
    push_slot(touch_events, slot);
    touch_events.push_back({EV_ABS, ABS_MT_POSITION_X, x});
    touch_events.push_back({EV_ABS, ABS_MT_POSITION_Y, y});
    touch_events.push_back({EV_SYN, SYN_REPORT, 0});
  }
};

}

#endif