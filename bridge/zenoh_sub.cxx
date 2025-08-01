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
// Include CycloneDDS CDR Deserializer
#include <dds/cdr/dds_cdrstream.h>
// Include the message types you need
#include "PointCloud2.hpp"

using namespace zenoh;
using namespace std::chrono_literals;

#define ROS_TOPIC_TF          "tf"
#define ROS_TOPIC_TF_STATIC   "tf_static"
//#define ROS_TOPIC_POINT_CLOUD "point_cloud"
#define ROS_TOPIC_POINT_CLOUD "intel_realsense_r200_depth/points"

//const struct dds_cdrstream_allocator dds_cdrstream_default_allocator = {malloc, realloc, free};
//
//void idl_deser(unsigned char *buf, uint32_t sz, void *obj, const dds_topic_descriptor_t *desc)
//{
//    dds_istream_t is = {.m_buffer = buf, .m_size = sz, .m_index = 0, .m_xcdr_version = DDSI_RTPS_CDR_ENC_VERSION_2};
//    // TODO(CY): Use a better way to cast
//    dds_stream_read(&is, (char *)obj, &dds_cdrstream_default_allocator, desc->m_ops);
//}

int main(int argc, char **argv)
{
    std::cout << "Zenoh Subscriber Example" << std::endl;

    // Initialize Zenoh session
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
        std::cout << ">> [Point Cloud Subscriber] Received" << std::endl;
        std::cout << "   Zenoh key: " << sample.get_keyexpr().as_string_view() << ", Size: " << sample.get_payload().size() << std::endl;
        sensor_msgs::msg::PointCloud2 point_cloud_msg;
        point_cloud_msg.height(10);
        point_cloud_msg.width(11);
        std::vector<char> buffer(100000, 0x0);
        basic_cdr_stream stream;
        stream.set_buffer(buffer.data(), buffer.size());
        write(stream, point_cloud_msg, key_mode::not_key);
        stream.reset();
        sensor_msgs::msg::PointCloud2 point_cloud_msg2;
        read(stream, point_cloud_msg2, key_mode::not_key);
        std::cout << "Height: " << point_cloud_msg2.height() << std::endl;
        //sensor_msgs_msg_PointCloud2 point_cloud_msg;
        //idl_deser(((unsigned char *)sample.get_payload().as_vector().data()), sample.get_payload().size(), &point_cloud_msg, &sensor_msgs_msg_PointCloud2_desc);
        //std::cout << "   Point Cloud Header: " << point_cloud_msg.header.frame_id << ", Height: " << point_cloud_msg.height << ", Width: " << point_cloud_msg.width << std::endl;
        // TODO(CY): Deserialize the Point Cloud message and process it
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
