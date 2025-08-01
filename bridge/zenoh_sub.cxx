//
// Copyright (c) 2025 ZettaScale Technology
// All rights reserved.
//
// This software is the confidential and proprietary information of ZettaScale Technology.
//
#include <chrono>
#include <iostream>
#include <thread>

// Include Zenoh C++ API
#include <zenoh.hxx>

// Include the message types you need
#include "PointCloud2.hpp"

using namespace zenoh;
using namespace std::chrono_literals;

#define ROS_TOPIC_TF          "tf"
#define ROS_TOPIC_TF_STATIC   "tf_static"
//#define ROS_TOPIC_POINT_CLOUD "point_cloud"
#define ROS_TOPIC_POINT_CLOUD "intel_realsense_r200_depth/points"

int main(int argc, char **argv)
{
    std::cout << "Zenoh Subscriber Example" << std::endl;

    // Initialize Zenoh session with a default configuration
    zenoh::Config config = zenoh::Config::create_default();
    auto session = Session::open(std::move(config));

    // Subscribe to /tf
    KeyExpr tf_keyexpr(ROS_TOPIC_TF);
    auto tf_handler = [](const Sample &sample) {
        std::cout << ">> [TF Subscriber] Received" << std::endl;
        // TODO(CY): Deserialize the TF message and process it
    }; 
    auto tf_subscriber = session.declare_subscriber(tf_keyexpr, tf_handler, closures::none);

    // Subscribe to /point_cloud
    KeyExpr point_cloud_keyexpr(ROS_TOPIC_POINT_CLOUD);
    auto point_cloud_handler = [](const Sample &sample) {
        std::cout << ">> [Point Cloud Subscriber] Zenoh key: " << sample.get_keyexpr().as_string_view() << ", Size: " << sample.get_payload().size() << std::endl;

        // Deserialize the CDR payload
        sensor_msgs::msg::PointCloud2 point_cloud_msg;
        basic_cdr_stream stream;
        // Get the payload and set the buffer for the stream
        auto buffer = sample.get_payload().as_vector();
        stream.set_buffer(buffer.data() + 4, buffer.size());
        // Read the PointCloud2 message from the stream
        read(stream, point_cloud_msg, key_mode::not_key);

        // Print some information about the PointCloud2 message
        std::cout << "   Time=" << point_cloud_msg.header().stamp() << ", Height=" << point_cloud_msg.height() << ", Width=" << point_cloud_msg.width() << std::endl;
    }; 
    auto point_cloud_subscriber = session.declare_subscriber(point_cloud_keyexpr, point_cloud_handler, closures::none);

    // Waiting for CTRL-C to exit
    std::cout << "Press CTRL-C to quit...\n";
    while (true) {
        std::this_thread::sleep_for(5s);
        // TODO(CY): Send query to /tf_static
    }

    return 0;
}
