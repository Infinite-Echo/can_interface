#include "can_interface/can_interface.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

std::atomic<bool> keep_running{true};

void signal_handler(int) {
  keep_running = false;
}

void handle_extended_can_frame(const can_frame& frame) {
  uint32_t id = frame.can_id & CAN_EFF_MASK;
  std::cout << "Received extended frame with ID 0x" << std::hex << id << ": ";
  for (int i = 0; i < frame.can_dlc; ++i) {
    std::printf("%02X ", frame.data[i]);
  }
  std::cout << std::endl;
}

int main() {
  signal(SIGINT, signal_handler);

  CANInterface can;
  if (!can.start("vcan0")) {
    std::cerr << "Failed to start CAN interface.\n";
    return 1;
  }

  can.register_callback(0x1ABCDE | CAN_EFF_FLAG, handle_extended_can_frame);

  can_frame tx_frame{};
  tx_frame.can_id = 0x1ABCDE | CAN_EFF_FLAG;
  tx_frame.can_dlc = 4;
  tx_frame.data[0] = 0xDE;
  tx_frame.data[1] = 0xAD;
  tx_frame.data[2] = 0xBE;
  tx_frame.data[3] = 0xEF;

  can.send(tx_frame);
  std::cout << "Sent extended CAN frame with ID 0x1ABCDE.\n";

  std::cout << "Listening for CAN messages (Ctrl+C to quit)...\n";

  while (keep_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  can.stop();
  return 0;
}
