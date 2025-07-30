cmake_minimum_required(VERSION 3.14)
project(can_interface)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Required for ROS 2 packages using ament
find_package(ament_cmake REQUIRED)

# Include directories
include_directories(include)

# Create the library
add_library(can_interface
  SHARED
  src/can_interface.cpp
)

target_include_directories(can_interface PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Install the library and headers
install(TARGETS can_interface
  EXPORT export_can_interface
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

# TEST EXECUTABLE
add_executable(test_can_interface examples/test_can_interface.cpp)
target_link_libraries(test_can_interface can_interface pthread)

add_executable(test_extended_can_interface examples/test_extended_can_interface.cpp)
target_link_libraries(test_extended_can_interface can_interface pthread)

add_executable(test_all_can examples/test_all_can.cpp)
target_link_libraries(test_all_can can_interface pthread)

install(TARGETS test_can_interface test_extended_can_interface test_all_can
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_export_targets(export_can_interface)
ament_export_include_directories(include)
ament_export_libraries(can_interface)

ament_package()
