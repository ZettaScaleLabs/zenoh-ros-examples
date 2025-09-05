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

# Don't remove RPATH while install binaries
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Add Zenoh libraries
find_package(zenohc REQUIRED)
find_package(zenohcxx REQUIRED)

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# Check IDLC version
find_program(IDLC idlc)
message(STATUS, "IDLC: ${IDLC}")

# IDL code generation
find_package(CycloneDDS-CXX REQUIRED)
file(GLOB IDL_FILES ../common/idl/*.idl)
message(STATUS "IDL files found: ${IDL_FILES}")
idlcxx_generate(TARGET IdlGenerated_lib FILES ${IDL_FILES} WARNINGS no-implicit-extensibility)
include_directories(${CMAKE_BINARY_DIR})

# Include the common directory for shared code
include_directories(../common)

# Build
add_executable(bridge_sub bridge_sub.cxx)
target_link_libraries(bridge_sub PRIVATE zenohc::lib zenohcxx::zenohc CycloneDDS-CXX::ddscxx IdlGenerated_lib)

# Install
install(TARGETS bridge_sub DESTINATION bin)
