#include "can_interface/can_interface.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void handle_can_message(const can_frame& frame) {
  std::cout << "Received frame with ID : " << std::hex << frame.can_id
            << ", DLC: " << std::dec << static_cast<int>(frame.can_dlc)
            << ", Data: ";
  for (int i = 0; i < frame.can_dlc; ++i) {
    std::printf("%02X ", frame.data[i]);
  }
  std::cout << std::endl;
}

int main() {
  CANInterface can;

  if (!can.start("vcan0")) {
    std::cerr << "Failed to start CAN interface.\n";
    return 1;
  }

  // Register a callback for CAN ID 0x123
  can.register_callback(0x123, handle_can_message);
  can.register_callback(0x456, handle_can_message);

  // Prepare and send a test frame
  can_frame tx_frame{};
  tx_frame.can_id = 0x123;
  tx_frame.can_dlc = 2;
  tx_frame.data[0] = 0xAB;
  tx_frame.data[1] = 0xCD;

  if (!can.send(tx_frame)) {
    std::cerr << "Failed to send CAN frame.\n";
    return 1;
  }

  std::cout << "Sent CAN frame with ID 0x123.\n";

  // Sleep briefly to wait for receive
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }


  can.stop();
  return 0;
}
