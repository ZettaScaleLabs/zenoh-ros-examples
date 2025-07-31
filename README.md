# zenoh-ros-examples

Examples to subscribe ROS 2 data with Zenoh, including:

* `/tf`: [tf2_msgs/msg/TFMessage](https://github.com/ros2/geometry2/blob/rolling/tf2_msgs/msg/TFMessage.msg)
* `/tf_static`: [tf2_msgs/msg/TFMessage](https://github.com/ros2/geometry2/blob/rolling/tf2_msgs/msg/TFMessage.msg)
* `/point_cloud`: [sensor_msgs/msg/PointCloud2](https://github.com/ros2/common_interfaces/blob/rolling/sensor_msgs/msg/PointCloud2.msg)

## Prerequisite

The following tutorial to install packages via apt, but you can also build them manually by yourselves.

* Install zenoh-c, zenoh-cpp and zenoh-plugin-ros2dds

```bash
echo "deb [trusted=yes] https://download.eclipse.org/zenoh/debian-repo/ /" | sudo tee -a /etc/apt/sources.list > /dev/null
sudo apt update
sudo apt install libzenohc-dev libzenohcpp-dev zenoh-plugin-ros2dds
```

* Install rmw_zenoh

```bash
sudo apt install ros-jazzy-rmw-zenoh-cpp
```

## Build the project

We are using [just](https://github.com/casey/just) to simplify the command.
You can install it via `sudo apt install just`.

* Build the whole project

```bash
just all
```

* Clean the whole project

```bash
just clean
```
