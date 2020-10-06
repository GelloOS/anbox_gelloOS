#include <functional>
#include <iostream>
#include <memory>

#include "anbox/container/service.h"
#include "anbox/common/loop_device_allocator.h"
#include "anbox/logger.h"
#include "anbox/runtime.h"
#include "anbox/system_configuration.h"

#include "anbox/common/loop_device.h"
#include "anbox/common/mount_entry.h"

#include "core/posix/signal.h"
#include "core/posix/exec.h"

#include <sys/mount.h>
#include <linux/loop.h>
#include <fcntl.h>


using namespace anbox;
namespace fs = boost::filesystem;

namespace {
// constexpr unsigned int unprivileged_user_id{655360};
constexpr unsigned int unprivileged_user_id{656360};
}

std::string android_img_path_;
std::string data_path_;

//std::shared_ptr<common::LoopDevice> android_img_loop_dev_;
std::vector<std::shared_ptr<common::MountEntry>> mounts_;
bool privileged_ = true;
// bool daemon_ = false;
bool enable_rootfs_overlay_ = false;
std::string container_network_address_;
std::string container_network_gateway_;
std::string container_network_dns_servers_;

bool setup_rootfs_overlay() {
  const auto combined_rootfs_path = SystemConfiguration::instance().combined_rootfs_dir();
  if (!fs::exists(combined_rootfs_path))
    fs::create_directories(combined_rootfs_path);

  const auto overlay_path = SystemConfiguration::instance().overlay_dir();
  if (!fs::exists(overlay_path))
    fs::create_directories(overlay_path);

  const auto rootfs_path = SystemConfiguration::instance().rootfs_dir();
  const auto overlay_config = utils::string_format("lowerdir=%s:%s", overlay_path, rootfs_path);
  auto m = common::MountEntry::create("overlay", combined_rootfs_path, "overlay", MS_RDONLY, overlay_config.c_str());
  if (!m) {
    ERROR("Failed to setup rootfs overlay");
    mounts_.clear();
    return false;
  }
  mounts_.push_back(m);

  DEBUG("Successfully setup rootfs overlay");
  return true;
}

bool setup_mounts() {
  fs::path android_img_path = android_img_path_ + "/android.img";
  if (android_img_path.empty())
    android_img_path = SystemConfiguration::instance().data_dir() / "android.img";

  if (!fs::exists(android_img_path)) {
    ERROR("Android image does not exist at path %s", android_img_path);
    return false;
  }

  // const auto android_rootfs_dir = SystemConfiguration::instance().rootfs_dir();  
  const auto android_rootfs_dir = android_img_path_ + "/rootfs";
  if (utils::is_mounted(android_rootfs_dir)) {
    ERROR("Androd rootfs is already mounted!?");
    return false;
  }

  if (!fs::exists(android_rootfs_dir))
    fs::create_directory(android_rootfs_dir);

  // We prefer using the kernel for mounting the squashfs image but
  // for some cases (unprivileged containers) where no loop support
  // is available we do the mount instead via squashfuse which will
  // work entirely in userspace.
  if (fs::exists("/dev/loop-control")) {
    std::shared_ptr<common::LoopDevice> loop_device;

    try {
      loop_device = common::LoopDeviceAllocator::new_device();
    } catch (const std::exception& e) {
      ERROR("Could not create loopback device: %s", e.what());
      return false;
    } catch (...) {
      ERROR("Could not create loopback device");
      return false;
    }

    if (!loop_device->attach_file(android_img_path)) {
      ERROR("Failed to attach Android rootfs image to loopback device");
      return false;
    }

    auto m = common::MountEntry::create(loop_device, android_rootfs_dir, "squashfs", MS_MGC_VAL | MS_RDONLY | MS_PRIVATE);
    if (!m) {
      ERROR("Failed to mount Android rootfs");
      return false;
    }
    mounts_.push_back(m);
  } else if (fs::exists("/dev/fuse") && !utils::find_program_on_path("squashfuse").empty()) {
    std::vector<std::string> args = {
      "-t", "fuse.squashfuse",
      // Allow other users than root to access the rootfs
      "-o", "allow_other",
      android_img_path.string(),
      android_rootfs_dir,
    };

    // Easiest is here to go with the standard mount program as that
    // will handle everything for us which is relevant to get the
    // squashfs via squashfuse properly mount without having to
    // reimplement all the details. Once the mount call comes back
    // without an error we can expect the image to be mounted.
    auto child = core::posix::exec("/bin/mount", args, {}, core::posix::StandardStream::empty, []() {});
    const auto result = child.wait_for(core::posix::wait::Flags::untraced);
    if (result.status != core::posix::wait::Result::Status::exited ||
        result.detail.if_exited.status != core::posix::exit::Status::success) {
      ERROR("Failed to mount squashfs Android image");
      return false;
    }

    auto m = common::MountEntry::create(android_rootfs_dir);
    if (!m) {
      ERROR("Failed to create mount entry for Android rootfs");
      return false;
    }
    mounts_.push_back(m);
  } else {
    ERROR("No loop device or FUSE support found. Can't setup Android rootfs!");
    return false;
  }

  auto final_android_rootfs_dir = android_rootfs_dir;
  if (enable_rootfs_overlay_) {
    if (!setup_rootfs_overlay())
      return false;

    final_android_rootfs_dir = SystemConfiguration::instance().combined_rootfs_dir();
  }  
    
  //for (const auto &dir_name : std::vector<std::string>{"cache", "data", "dalvik-cache"}) {
  for (const auto &dir_name : std::vector<std::string>{"cache", "data"}) {  
    auto target_dir_path = fs::path(final_android_rootfs_dir) / dir_name;
    auto src_dir_path = SystemConfiguration::instance().data_dir() / dir_name;

    if (!fs::exists(src_dir_path)) {
      if (!fs::create_directory(src_dir_path)) {
        ERROR("Failed to create Android %s directory", dir_name);
        mounts_.clear();
        return false;
      }

      if (0 == strcmp(dir_name.data(), "cache")){
        if (::chown(src_dir_path.c_str(), unprivileged_user_id - 1000, unprivileged_user_id - 1000) != 0) {
          ERROR("Failed to allow access for unprivileged user on %s directory of the rootfs", dir_name);
          mounts_.clear();
          return false;
        }
      }else{
        if (::chown(src_dir_path.c_str(), unprivileged_user_id, unprivileged_user_id) != 0) {
          ERROR("Failed to allow access for unprivileged user on %s directory of the rootfs", dir_name);
          mounts_.clear();
          return false;
        }
      }      
    }
    
    auto m = common::MountEntry::create(src_dir_path, target_dir_path, "", MS_MGC_VAL | MS_BIND | MS_PRIVATE);    
    if (!m) {
      ERROR("Failed to mount Android %s directory", dir_name);
      mounts_.clear();
      return false;
    }
    mounts_.push_back(m);
  }  

  for (const auto &dir_name : std::vector<std::string>{"default", "read", "write"}) {          
    fs::create_directories(std::string("/run/arc/sdcard/" + dir_name + "/emulated"));
  }  

  // Unmounting needs to happen in reverse order
  std::reverse(mounts_.begin(), mounts_.end());

  return true;
}


int main(int argc, char** argv) {
  android_img_path_ = argv[1];
  data_path_ = argv[2];

  auto trap = core::posix::trap_signals_for_process(
      {core::posix::Signal::sig_term, core::posix::Signal::sig_int});
  trap->signal_raised().connect([trap](const core::posix::Signal& signal) {
    INFO("Signal %i received. Good night.", static_cast<int>(signal));
    trap->stop();
  });

  if (!data_path_.empty())
    SystemConfiguration::instance().set_data_path(data_path_);

  if (!fs::exists(data_path_))
    fs::create_directories(data_path_);

  if (!setup_mounts())
    return EXIT_FAILURE;

  auto rt = Runtime::create();
  // container::Service::Configuration config;
  // config.privileged = privileged_;
  // config.rootfs_overlay = enable_rootfs_overlay_;
  // config.container_network_address = container_network_address_;
  // config.container_network_gateway = container_network_gateway_;

  // if (container_network_dns_servers_.length() > 0)
  //   config.container_network_dns_servers = utils::string_split(container_network_dns_servers_, ',');

  // auto service = container::Service::create(rt, config);

  rt->start();
  trap->run();
  rt->stop();

  return 0;
}