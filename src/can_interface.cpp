#include "can_interface/can_interface.hpp"
#include <iostream>
#include <sys/select.h>

bool CANInterface::start(const std::string& interface_name) {
  interface_name_ = interface_name;

  sock_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (sock_ < 0) return false;

  struct ifreq ifr;
  std::strncpy(ifr.ifr_name, interface_name_.c_str(), IFNAMSIZ - 1);
  if (ioctl(sock_, SIOCGIFINDEX, &ifr) < 0) return false;

  struct sockaddr_can addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) return false;

  running_ = true;
  receiver_thread_ = std::thread(&CANInterface::receive_loop, this);
  return true;
}

bool CANInterface::start() {
  return start(interface_name_);
}

void CANInterface::stop() {
  running_ = false;
  if (receiver_thread_.joinable()) receiver_thread_.join();
  if (sock_ >= 0) close(sock_);
}

bool CANInterface::send(const can_frame& frame) {
  if (sock_ < 0) return false;
  return write(sock_, &frame, sizeof(frame)) == sizeof(frame);
}

void CANInterface::register_callback(uint32_t can_id, CANCallback callback) {
  std::lock_guard<std::mutex> lock(callbacks_mutex_);
  callbacks_[can_id] = callback;
}

void CANInterface::unregister_callback(uint32_t can_id) {
  std::lock_guard<std::mutex> lock(callbacks_mutex_);
  callbacks_.erase(can_id);
}

void CANInterface::receive_loop() {
  struct can_frame frame;
  while (running_) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sock_, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int ret = select(sock_ + 1, &read_fds, nullptr, nullptr, &timeout);

    if (ret > 0 && FD_ISSET(sock_, &read_fds)) {
      int nbytes = read(sock_, &frame, sizeof(frame));
      if (nbytes > 0) {
        uint32_t key = frame.can_id;
        {
          std::lock_guard<std::mutex> lock(callbacks_mutex_);
          auto it = callbacks_.find(key);
          if (it == callbacks_.end()) {
            key = key & CAN_EFF_MASK;
            it = callbacks_.find(key);
          }
          if (it != callbacks_.end()) {
            try {
              it->second(frame);
            } catch (...) {
              // Prevent exceptions from crashing the receive thread
            }
          }
        }
      }
    }
  }
}