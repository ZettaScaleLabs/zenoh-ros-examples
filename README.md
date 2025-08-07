# zenoh-ros-examples

Examples to subscribe ROS 2 data with Zenoh, including:

* `/tf`: [tf2_msgs/msg/TFMessage](https://github.com/ros2/geometry2/blob/rolling/tf2_msgs/msg/TFMessage.msg)
* `/tf_static`: [tf2_msgs/msg/TFMessage](https://github.com/ros2/geometry2/blob/rolling/tf2_msgs/msg/TFMessage.msg)
* `/point_cloud`: [sensor_msgs/msg/PointCloud2](https://github.com/ros2/common_interfaces/blob/rolling/sensor_msgs/msg/PointCloud2.msg)

## Prerequisite

The following tutorial to install packages via apt, but you can also build them manually by yourselves.

* Install zenoh-c, zenoh-cpp and zenoh-bridge-ros2dds

```bash
echo "deb [trusted=yes] https://download.eclipse.org/zenoh/debian-repo/ /" | sudo tee -a /etc/apt/sources.list > /dev/null
sudo apt update
sudo apt install libzenohc-dev libzenohcpp-dev zenoh-bridge-ros2dds
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
# Option: You can build the project step by step
just prepare               # Init submodule
just cyclonedds            # Install CycloneDDS
just cyclonedds-cxx        # Install CycloneDDS C++
just zenoh-bridge-examples # Build the bridge example
just zenoh-rmw-examples    # Build the rmw zenoh example
```

* Clean the whole project

```bash
just clean
```

## Usage

### Subscribe messages from `zenoh-bridge-ros2dds`

* Run your ROS 2 program with CycloneDDS

```bash
source /opt/ros/jazzy/setup.bash
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
# Your ROS 2 program
```

* Run the `zenoh-bridge-ros2dds`
  * Note that you can run with the configuration that filters unnecessary topics.

```bash
source /opt/ros/jazzy/setup.bash
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
zenoh-bridge-ros2dds -c bridge/config/bridge-config.json5
```

* Run the subscriber

```bash
./bridge/build/zenoh_sub
```

### Subscribe messages from `rmw_zenoh`

* Run Zenoh router

```bash
source /opt/ros/jazzy/setup.bash
export RMW_IMPLEMENTATION=rmw_zenoh_cpp
ros2 run rmw_zenoh_cpp rmw_zenohd
```

* Run your ROS 2 program with `rmw_zenoh`

```bash
source /opt/ros/jazzy/setup.bash
export RMW_IMPLEMENTATION=rmw_zenoh_cpp
# Your ROS 2 program
```

* Run the subscriber
  * You might need to update the connection of `rmw_zenohd` in the configuration file first.
  * The path of the configuration file: `rmw_zenoh/config/zenoh-config.json5`

```bash
# Accept the configuration that creates a connection to rmw_zenohd
./rmw_zenoh/build/zenoh_sub ./rmw_zenoh/config/zenoh-config.json5
```
