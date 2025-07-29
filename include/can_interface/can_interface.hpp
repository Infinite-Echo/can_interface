#pragma once

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

class CANInterface {
public:
  using CANCallback = std::function<void(const can_frame&)>;

  explicit CANInterface(const std::string& interface_name)
      : interface_name_(interface_name), running_(false), sock_(-1) {}

  ~CANInterface() { stop(); }

  bool start();
  void stop();

  bool send(const can_frame& frame);

  void register_callback(uint32_t can_id, CANCallback callback);
  void unregister_callback(uint32_t can_id);

private:
  void receive_loop();

  std::string interface_name_;
  int sock_;
  std::atomic<bool> running_;
  std::thread receiver_thread_;

  std::mutex callbacks_mutex_;
  std::unordered_map<uint32_t, CANCallback> callbacks_;
};