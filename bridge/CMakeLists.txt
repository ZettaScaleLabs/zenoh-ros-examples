#
# Copyright (c) 2025 ZettaScale Technology
# All rights reserved.
#
# This software is the confidential and proprietary information of ZettaScale Technology.
#
cmake_minimum_required(VERSION 3.16)
project(zenoh_ros_bridge_cpp_example
        DESCRIPTION "C++ Examples for Zenoh ROS Bridge"
        VERSION 0.1.0
        LANGUAGES CXX)

find_package(zenohc REQUIRED)
find_package(zenohcxx REQUIRED)

# Check for ROS_DISTRO environment variable and add include path
if(NOT DEFINED ENV{ROS_DISTRO})
    message(FATAL_ERROR "Environment variable ROS_DISTRO is not set. Please source your ROS 2 setup file (e.g., 'source /opt/ros/humble/setup.bash')")
else()
    # Add the ROS include directory
    message(STATUS "ROS_DISTRO found: $ENV{ROS_DISTRO}, adding include path /opt/ros/$ENV{ROS_DISTRO}/include")
    include_directories(/opt/ros/$ENV{ROS_DISTRO}/include)
endif()

# Check IDLC version
find_program(IDLC idlc REQUIRED)

# IDL code generation
find_package(CycloneDDS REQUIRED)
file(GLOB IDL_FILES idl/*.idl)
message(STATUS "IDL files found: ${IDL_FILES}")
idlc_generate(TARGET IdlGenerated_lib FILES ${IDL_FILES} WARNINGS no-implicit-extensibility)
include_directories(${CMAKE_BINARY_DIR})

# Build
add_executable(zenoh_sub zenoh_sub.cxx PointCloud2.c)
target_link_libraries(zenoh_sub PRIVATE zenohc::lib zenohcxx::zenohc IdlGenerated_lib)
