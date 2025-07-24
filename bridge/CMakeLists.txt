cmake_minimum_required(VERSION 3.16)
project(zenoh_ros_bridge_cpp_example
        DESCRIPTION "C++ Examples for Zenoh ROS Bridge"
        VERSION 0.1.0
        LANGUAGES CXX)

find_package(zenohc REQUIRED)
find_package(zenohcxx REQUIRED)

add_executable(zenoh_sub zenoh_sub.cxx)
target_link_libraries(zenoh_sub PRIVATE zenohc::lib zenohcxx::zenohc)
