#include <stdio.h>
#include <boost/filesystem.hpp>

#include "core/posix/signal.h"
#include "core/posix/exec.h"

#include "anbox/rpc/channel.h"
#include "anbox/rpc/connection_creator.h"
#include "anbox/logger.h"
#include "anbox/system_configuration.h"
#include "anbox/common/dispatcher.h"
#include "anbox/container/client.h"
#include "anbox/input/manager.h"
#include "anbox/bridge/android_api_stub.h"
#include "anbox/bridge/platform_api_skeleton.h"
#include "anbox/bridge/platform_message_processor.h"
#include "anbox/platform/base_platform.h"
#include "anbox/application/database.h"
#include "anbox/wm/multi_window_manager.h"
#include "anbox/graphics/gl_renderer_server.h"
#include "anbox/audio/server.h"
#include "anbox/network/published_socket_connector.h"
#include "anbox/qemu/pipe_connection_creator.h"
#include "anbox/wm/single_window_manager.h"
#include "anbox/platform/null/platform.h"

using namespace anbox;

int main(int argc, char** argv) {        
  Log().Init(anbox::Logger::Severity::kDebug);  

  auto trap = core::posix::trap_signals_for_process(
        {core::posix::Signal::sig_term, core::posix::Signal::sig_int});

  trap->signal_raised().connect([trap](const core::posix::Signal &signal) {
      INFO("Signal %i received. Good night.", static_cast<int>(signal));
      trap->stop();
    });    

  utils::ensure_paths({
      SystemConfiguration::instance().socket_dir(),
      SystemConfiguration::instance().input_device_dir(),
  });  

  auto rt = Runtime::create();
  auto dispatcher = anbox::common::create_dispatcher_for_runtime(rt);

  std::shared_ptr<container::Client> container_ = std::make_shared<container::Client>(rt);
  container_->register_terminate_handler([&]() {
    WARNING("Lost connection to container manager, terminating.");
    trap->stop();
  });

  auto input_manager = std::make_shared<input::Manager>(rt);
  auto android_api_stub = std::make_shared<bridge::AndroidApiStub>();

  bool single_window = true;
  platform::Configuration platform_config;
  platform_config.single_window = single_window;
  platform_config.no_touch_emulation = false;
  platform_config.display_frame = anbox::graphics::Rect{0, 0, 1024, 768};

  // auto platform = platform::create(utils::get_env_value("ANBOX_PLATFORM", "sdl"),
  //                                    input_manager,
  //                                    platform_config);

  auto platform = std::make_shared<platform::NullPlatform>();

  if (!platform)
      return EXIT_FAILURE;

  auto app_db = std::make_shared<application::Database>();

  std::shared_ptr<wm::Manager> window_manager;
  if (single_window){
    window_manager = std::make_shared<wm::SingleWindowManager>(platform, platform_config.display_frame, app_db);
  }
  // else{
  //   window_manager = std::make_shared<wm::MultiWindowManager>(platform, android_api_stub, app_db);  
  // }    
  
  auto gl_driver = graphics::GLRendererServer::Config::Driver::Host;
  // auto gl_driver = graphics::GLRendererServer::Config::Driver::Software;

  graphics::GLRendererServer::Config renderer_config {
    gl_driver,
    single_window
  };
  auto gl_server = std::make_shared<graphics::GLRendererServer>(renderer_config, window_manager);

  platform->set_window_manager(window_manager);  
  platform->set_renderer(gl_server->renderer());  
  window_manager->setup();  

  // auto app_manager = std::static_pointer_cast<application::Manager>(android_api_stub);
  // if (!single_window){
  //   app_manager = std::make_shared<application::RestrictedManager>(
  //     android_api_stub, 
  //     wm::Stack::Id::Freeform
  //   );
  // }

  auto audio_server = std::make_shared<audio::Server>(rt, platform);

  const auto socket_path = SystemConfiguration::instance().socket_dir();  

  // The qemu pipe is used as a very fast communication channel between guest
  // and host for things like the GLES emulation/translation, the RIL or ADB.
  auto qemu_pipe_connector =
      std::make_shared<network::PublishedSocketConnector>(
          utils::string_format("%s/qemu_pipe", socket_path), rt,
          std::make_shared<qemu::PipeConnectionCreator>(gl_server->renderer(), rt));  

  boost::asio::deadline_timer appmgr_start_timer(rt->service());  

  auto bridge_connector = std::make_shared<network::PublishedSocketConnector>(
      utils::string_format("%s/anbox_bridge", socket_path), rt,
      std::make_shared<rpc::ConnectionCreator>(
          rt, [&](const std::shared_ptr<network::MessageSender> &sender) {            

            auto pending_calls = std::make_shared<rpc::PendingCallCache>();
            auto rpc_channel = std::make_shared<rpc::Channel>(pending_calls, sender);
            // This is safe as long as we only support a single client. If we
            // support
            // more than one one day we need proper dispatching to the right
            // one.
            android_api_stub->set_rpc_channel(rpc_channel);            
            
            auto server = std::make_shared<bridge::PlatformApiSkeleton>(
                pending_calls, platform, window_manager, app_db);
            server->register_boot_finished_handler([&]() {              

              DEBUG("Android successfully booted");
              android_api_stub->ready().set(true);
              appmgr_start_timer.expires_from_now(boost::posix_time::milliseconds(50));
              appmgr_start_timer.async_wait([&](const boost::system::error_code &err) {                
                printf("%d\n", err.value());
                if (err != boost::system::errc::success){                  
                  printf("found error %d\n", err.value());
                  return;
                }

                if (!single_window){
                  return;
                }

                constexpr const char *default_appmgr_package{"org.anbox.appmgr"};
                constexpr const char *default_appmgr_component{"org.anbox.appmgr.AppViewActivity"};

                android::Intent launch_intent;
                launch_intent.package = default_appmgr_package;
                launch_intent.component = default_appmgr_component;
                // As this will only be executed in single window mode we don't have
                // to specify and launch bounds.
                android_api_stub->launch(launch_intent, graphics::Rect::Invalid, wm::Stack::Id::Default);                
              });

              // std::thread{
            //   [&android_api_stub](){   
                // android::Intent intent;
                // intent.package = "org.anbox.appmgr";
                // intent.component = "org.anbox.appmgr.AppViewActivity";      

                // android_api_stub->launch(intent, graphics::Rect::Invalid, wm::Stack::Id::Default);
            //   }
            // }.join();                        
            });            

            return std::make_shared<bridge::PlatformMessageProcessor>(
                sender, server, pending_calls);
          }));

  container::Configuration container_configuration;
  container_configuration.extra_properties.push_back("ro.boot.fake_battery=1");
  container_configuration.bind_mounts = {
        {qemu_pipe_connector->socket_file(), "/dev/qemu_pipe"},
        {bridge_connector->socket_file(), "/dev/anbox_bridge"},
        {audio_server->socket_file(), "/dev/anbox_audio"},
        {SystemConfiguration::instance().input_device_dir(), "/dev/input"},

      };

  container_configuration.devices = {
    {"/dev/binder", {0666}},
    {"/dev/ashmem", {0666}},
    {"/dev/fuse", {0666}},
  };

  // dispatcher->dispatch([&]() {
  //   container_->start(container_configuration);
  // });

  // auto bus_type = anbox::dbus::Bus::Type::Session;
  // auto bus = std::make_shared<anbox::dbus::Bus>(bus_type);

  // auto skeleton = anbox::dbus::skeleton::Service::create_for_bus(bus, app_manager);
  // bus->run_async();  

  rt->start();    
  trap->run();

  // container_->stop();
  // rt->stop();
  return 0;
}