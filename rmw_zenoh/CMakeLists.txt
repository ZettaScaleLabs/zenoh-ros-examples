#
# Copyright (c) 2025 ZettaScale Technology
# All rights reserved.
#
# This software is the confidential and proprietary information of ZettaScale Technology.
#
cmake_minimum_required(VERSION 3.16)
project(zenoh_rmw_cpp_example
        DESCRIPTION "C++ Examples for Zenoh RMW"
        VERSION 0.1.0
        LANGUAGES CXX)

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
add_executable(zenoh_sub zenoh_sub.cxx)
target_link_libraries(zenoh_sub PRIVATE zenohc::lib zenohcxx::zenohc CycloneDDS-CXX::ddscxx IdlGenerated_lib)
