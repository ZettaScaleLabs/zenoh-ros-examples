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
#include "TFMessage.hpp"

using namespace std::chrono_literals;

// The topic name we will use
// In rmw_zenoh, the format of the topic is 
//   "<domain_id>/<fully_qualified_name>/<type_name>/<type_hash>"
// Please refer to the rmw_zenoh documentation for more details.
// https://github.com/ros2/rmw_zenoh/blob/rolling/docs/design.md#topic-and-service-name-mapping-to-zenoh-key-expressions
#define ROS_TOPIC_TF          "*/tf/*/*"
// TODO(CY): Unable to subscribe transient_local topic
#define ROS_TOPIC_TF_STATIC   "*/tf_static/*/*"
#define ROS_TOPIC_POINT_CLOUD "*/point_cloud/*/*"
// The point cloud topic which is used in the turtlebot demo
//#define ROS_TOPIC_POINT_CLOUD "*/local_costmap/clearing_endpoints/*/*"
//#define ROS_TOPIC_POINT_CLOUD "*/intel_realsense_r200_depth_driver/*/*"

int main(int argc, char **argv)
{
    std::cout << "Zenoh RMW Subscriber Example" << std::endl;

    // Initialize Zenoh session with a default configuration
    zenoh::Config config = zenoh::Config::create_default();
    if (argc > 1) {
        std::cout << "Using configuration file: " << argv[1] << std::endl;
        // If a configuration file is provided, load it
        config = zenoh::Config::from_file(std::string(argv[1]));
    }
    auto session = zenoh::Session::open(std::move(config));

    // Subscribe to /tf
    zenoh::KeyExpr tf_keyexpr(ROS_TOPIC_TF);
    auto tf_handler = [](const zenoh::Sample & sample) {
        std::cout << ">> [TF Subscriber] Zenoh key: " << sample.get_keyexpr().as_string_view()
                  << ", Size: " << sample.get_payload().size() << std::endl;

        // Deserialize the CDR payload
        tf2_msgs::msg::TFMessage tf_msg;
        basic_cdr_stream stream;
        // Get the payload and set the buffer for the stream
        auto buffer = sample.get_payload().as_vector();
        stream.set_buffer(buffer.data() + 4, buffer.size());
        // Read the TFMessage from the stream
        read(stream, tf_msg, key_mode::not_key);

        // Print some information about the TFMessage
        std::cout << "   Number of transforms: " << tf_msg.transforms().size() << std::endl;
        for (const auto &transform : tf_msg.transforms()) {
            std::cout << "   Transform: " << transform.header().stamp()
                      << ", Frame ID: " << transform.header().frame_id()
                      << ", Child Frame ID: " << transform.child_frame_id() << std::endl;
            std::cout << "   Translation: (" 
                      << transform.transform().translation().x() << ", "
                      << transform.transform().translation().y() << ", "
                      << transform.transform().translation().z() << ")" << std::endl;
            std::cout << "   Rotation: (" 
                      << transform.transform().rotation().x() << ", "
                      << transform.transform().rotation().y() << ", "
                      << transform.transform().rotation().z() << ", "
                      << transform.transform().rotation().w() << ")" << std::endl;
        }
    }; 
    auto tf_subscriber = session.declare_subscriber(
                                    tf_keyexpr,               // TF key expression
                                    std::move(tf_handler),    // The callback to receive sample
                                    zenoh::closures::none     // Drop callback which is not used
                                 );

    // Subscribe to /point_cloud
    zenoh::KeyExpr point_cloud_keyexpr(ROS_TOPIC_POINT_CLOUD);
    auto point_cloud_handler = [](const zenoh::Sample & sample) {
        std::cout << ">> [Point Cloud Subscriber] Zenoh key: " << sample.get_keyexpr().as_string_view()
                  << ", Size: " << sample.get_payload().size() << std::endl;

        // Deserialize the CDR payload
        sensor_msgs::msg::PointCloud2 point_cloud_msg;
        basic_cdr_stream stream;
        // Get the payload and set the buffer for the stream
        auto buffer = sample.get_payload().as_vector();
        stream.set_buffer(buffer.data() + 4, buffer.size());
        // Read the PointCloud2 message from the stream
        read(stream, point_cloud_msg, key_mode::not_key);

        // Print some information about the PointCloud2 message
        std::cout << "   Time=" << point_cloud_msg.header().stamp() 
                  << ", Height=" << point_cloud_msg.height()
                  << ", Width=" << point_cloud_msg.width() << std::endl;
    }; 
    auto point_cloud_subscriber = session.declare_subscriber(
                                            point_cloud_keyexpr,               // Point Cloud key expression
                                            std::move(point_cloud_handler),    // The callback to receive sample
                                            zenoh::closures::none              // Drop callback which is not used
                                          );

    // Subscribe to /tf_static
    // Using advanced subscriber because /tf_static is a transient_local topic
    using AdvancedSubscriberOptions = zenoh::ext::SessionExt::AdvancedSubscriberOptions;
    auto adv_sub_opts = AdvancedSubscriberOptions::create_default();
    // Allow this subscriber to be detected through liveliness.
    adv_sub_opts.subscriber_detection = true;
    adv_sub_opts.query_timeout_ms = std::numeric_limits<uint64_t>::max();
    // History can only be retransmitted by Publishers that enable caching.
    adv_sub_opts.history = AdvancedSubscriberOptions::HistoryOptions::create_default();
    // Enable detection of late joiner publishers and query for their historical data.
    adv_sub_opts.history->detect_late_publishers = true;
    adv_sub_opts.history->max_samples = 10;
    zenoh::KeyExpr tf_static_keyexpr(ROS_TOPIC_TF_STATIC);
    auto tf_querying_sub = session.ext().declare_advanced_subscriber(
                                            tf_static_keyexpr,        // Point Cloud key expression
                                            std::move(tf_handler),    // The callback to receive sample
                                            zenoh::closures::none,    // Drop callback which is not used
                                            std::move(adv_sub_opts)   // Advanced Subscriber configuration
                                         );

    // TODO(CY): Declare a liveliness token for the detection.

    // Waiting for CTRL-C to exit
    std::cout << "Press CTRL-C to quit...\n";
    while (true) {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}