cmake_minimum_required(VERSION 3.16)
project(uinput_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(uinput_test src/core.hpp src/main.cpp src/virtual-device.hpp
                           src/virtual-device.cpp)
target_precompile_headers(uinput_test PRIVATE src/pch.hpp)
target_include_directories(uinput_test PUBLIC src)
target_link_libraries(uinput_test evdev)
