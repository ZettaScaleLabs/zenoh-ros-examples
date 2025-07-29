//
// Copyright (c) 2025 ZettaScale Technology
// All rights reserved.
//
// This software is the confidential and proprietary information of ZettaScale Technology.
//
#include <chrono>
#include <iostream>
#include <thread>

#include "zenoh.hxx"

using namespace zenoh;
using namespace std::chrono_literals;

#define ROS_TOPIC_TF          "tf"
#define ROS_TOPIC_TF_STATIC   "tf_static"
//#define ROS_TOPIC_POINT_CLOUD "point_cloud"
#define ROS_TOPIC_POINT_CLOUD "intel_realsense_r200_depth/points"

int main(int argc, char **argv) {
    std::cout << "Zenoh Subscriber Example" << std::endl;

    zenoh::Config config = zenoh::Config::create_default();
    auto session = Session::open(std::move(config));

    KeyExpr tf_keyexpr(ROS_TOPIC_TF);
    auto tf_handler = [](const Sample &sample) {
        std::cout << ">> [TF Subscriber] Received" << std::endl;
    }; 
    auto tf_subscriber = session.declare_subscriber(tf_keyexpr, tf_handler, closures::none);

    KeyExpr point_cloud_keyexpr(ROS_TOPIC_POINT_CLOUD);
    auto point_cloud_handler = [](const Sample &sample) {
        std::cout << ">> [Point Cloud Subscriber] Received" << std::endl;
    }; 
    auto point_cloud_subscriber = session.declare_subscriber(point_cloud_keyexpr, point_cloud_handler, closures::none);

    std::cout << "Press CTRL-C to quit...\n";
    while (true) {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
