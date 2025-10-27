#
# Copyright (c) 2025 ZettaScale Technology
#
# This program and the accompanying materials are made available under the
# terms of the Apache License, Version 2.0
# which is available at https://www.apache.org/licenses/LICENSE-2.0.
#
# SPDX-License-Identifier: Apache-2.0
#
# Contributors:
#   ChenYing Kuo, <cy@zettascale.tech>
#
cmake_minimum_required(VERSION 3.16)
project(zenoh_rmw_cpp_example
        DESCRIPTION "C++ Examples for Zenoh RMW"
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
add_executable(rmw_zenoh_sub rmw_zenoh_sub.cxx)
target_link_libraries(rmw_zenoh_sub PRIVATE zenohc::lib zenohcxx::zenohc CycloneDDS-CXX::ddscxx IdlGenerated_lib)

# Install
install(TARGETS rmw_zenoh_sub DESTINATION bin)
