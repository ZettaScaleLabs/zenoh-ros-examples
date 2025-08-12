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
#define MANGLED_TF            "%tf"
#define ROS_TOPIC_TF          "*/tf/*/*"
#define MANGLED_TF_STATIC     "%tf_static"
#define ROS_TOPIC_TF_STATIC   "*/tf_static/*/*"
// TODO(CY): Why ROS 2 humble can't work?
#define MANGLED_POINT_CLOUD   "%point_cloud"
#define ROS_TOPIC_POINT_CLOUD "*/point_cloud/*/*"
// The point cloud topic which is used in the turtlebot demo
//#define MANGLED_POINT_CLOUD   "%local_costmap%clearing_endpoints"
//#define ROS_TOPIC_POINT_CLOUD "*/local_costmap/clearing_endpoints/*/*"
//#define MANGLED_POINT_CLOUD   "%intel_realsense_r200_depth_driver"
//#define ROS_TOPIC_POINT_CLOUD "*/intel_realsense_r200_depth_driver/*/*"

// The name that will be shown in ROS 2 node list
#define NODE_NAME "zenoh_sub"

// The history depth for the subscriber
#define HISTORY_DEPTH 100

// The function is to get the next unique ID for entities used in ROS 2.
int get_next_entities_id()
{
    static int id = 0;
    return id++;
}

int main(int argc, char **argv)
{
    // Initialize Zenoh logging
    zenoh::init_log_from_env_or("error");

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
    adv_sub_opts.history->max_samples = HISTORY_DEPTH;
    // Only needed if the topic is reliable.
    adv_sub_opts.recovery.emplace().last_sample_miss_detection =
      AdvancedSubscriberOptions::RecoveryOptions::Heartbeat{};
    zenoh::KeyExpr tf_static_keyexpr(ROS_TOPIC_TF_STATIC);
    auto tf_querying_sub = session.ext().declare_advanced_subscriber(
                                            tf_static_keyexpr,        // Point Cloud key expression
                                            std::move(tf_handler),    // The callback to receive sample
                                            zenoh::closures::none,    // Drop callback which is not used
                                            std::move(adv_sub_opts)   // Advanced Subscriber configuration
                                         );

    // Declare a liveliness token for the detection.
    // The token format should follow the specification in rmw_zenoh.
    //   https://github.com/ros2/rmw_zenoh/blob/rolling/docs/design.md#graph-cache
    // QoS settings for the liveliness token
    //   https://github.com/ros2/rmw_zenoh/blob/cdb66eed88a41775e4d6b7a3919805d4963f606b/rmw_zenoh_cpp/src/detail/liveliness_utils.cpp#L239
    // Node liveliness token
    int node_id = get_next_entities_id();
    std::stringstream ss_node;
    ss_node << "@ros2_lv/0/" << session.get_zid() << "/" << node_id << "/" << node_id 
            << "/NN/%/%/" << NODE_NAME;
    auto liveliness_token_node = session.liveliness_declare_token(
      zenoh::KeyExpr(ss_node.str()),
      zenoh::Session::LivelinessDeclarationOptions::create_default());
    // TF liveliness token
    std::stringstream ss_tf;
    ss_tf << "@ros2_lv/0/" << session.get_zid() << "/" << node_id << "/" << get_next_entities_id()
          << "/MS/%/%/" << NODE_NAME << "/" << MANGLED_TF << "/tf2_msgs::msg::dds_::TFMessage_/RIHS01_e369d0f05a23ae52508854b66f6aa0437f3449d652e8cbf22d5abe85d020f087/::,100:,:,:,,";
    auto liveliness_token_tf = session.liveliness_declare_token(
      zenoh::KeyExpr(ss_tf.str()),
      zenoh::Session::LivelinessDeclarationOptions::create_default());
    // TF static liveliness token
    std::stringstream ss_tf_static;
    ss_tf_static << "@ros2_lv/0/" << session.get_zid() << "/" << node_id << "/" << get_next_entities_id()
                 << "/MS/%/%/" << NODE_NAME << "/" << MANGLED_TF_STATIC << "/tf2_msgs::msg::dds_::TFMessage_/RIHS01_e369d0f05a23ae52508854b66f6aa0437f3449d652e8cbf22d5abe85d020f087/:1:,1:,:,:,,";
    auto liveliness_token_tf_static = session.liveliness_declare_token(
      zenoh::KeyExpr(ss_tf_static.str()),
      zenoh::Session::LivelinessDeclarationOptions::create_default());
    // Point Cloud liveliness token
    std::stringstream ss_point_cloud;
    ss_point_cloud << "@ros2_lv/0/" << session.get_zid() << "/" << node_id << "/" << get_next_entities_id()
                   << "/MS/%/%/" << NODE_NAME << "/" << MANGLED_POINT_CLOUD
                   << "/sensor_msgs::msg::dds_::PointCloud2_/RIHS01_9198cabf7da3796ae6fe19c4cb3bdd3525492988c70522628af5daa124bae2b5/::,5:,:,:,,";   // Volatile, Keep last 5
                   //<< "/sensor_msgs::msg::dds_::PointCloud2_/RIHS01_9198cabf7da3796ae6fe19c4cb3bdd3525492988c70522628af5daa124bae2b5/:1:,1:,:,:,,";   // Transient Local, Keep last 1
    auto liveliness_token_point_cloud = session.liveliness_declare_token(
      zenoh::KeyExpr(ss_point_cloud.str()),
      zenoh::Session::LivelinessDeclarationOptions::create_default());

    // Waiting for CTRL-C to exit
    std::cout << "Press CTRL-C to quit...\n";
    while (true) {
        std::this_thread::sleep_for(1s);
    }

    // Undeclare the liveliness token when exiting
    std::move(liveliness_token_point_cloud).undeclare();
    std::move(liveliness_token_tf_static).undeclare();
    std::move(liveliness_token_tf).undeclare();
    std::move(liveliness_token_node).undeclare();

    return 0;
}