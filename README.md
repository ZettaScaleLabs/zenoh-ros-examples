# zenoh-ros-examples

This repository shows two examples of native Zenoh applications subscribing to ROS 2 publications, depending the `RMW_IMPLEMENTATION` which is used.

* If you're using **`rmw_cylonedds_cpp`**: the [**`bridge`**](./bridge/) directory contains an example of a C++ Zenoh application subcribing to ROS 2 Publishers through the [zenoh-bridge-ros2dds](https://github.com/eclipse-zenoh/zenoh-plugin-ros2dds)
* If you're using **`rmw_zenoh_cpp`**: the [**`rmw_zenoh`**](./rmw_zenoh/) directory contains an example of a C++ Zenoh application subcribing directly to ROS 2 Publishers, without any intermediary.

Both examples subscribing to those ROS topics and using CycloneDDS' CDR library to deserialize the data:

* `/tf`: [tf2_msgs/msg/TFMessage](https://github.com/ros2/geometry2/blob/rolling/tf2_msgs/msg/TFMessage.msg)
* `/tf_static`: [tf2_msgs/msg/TFMessage](https://github.com/ros2/geometry2/blob/rolling/tf2_msgs/msg/TFMessage.msg)
* `/point_cloud`: [sensor_msgs/msg/PointCloud2](https://github.com/ros2/common_interfaces/blob/rolling/sensor_msgs/msg/PointCloud2.msg)

## Prerequisite

> [!WARNING]
> For serialization and deserialization of CDR-encoded data, this project uses a recent version of CycloneDDS.  
> To avoid a potential conflict, **DON'T** build those examples under a ROS environment where an incompatible CycloneDDS library could be already installed.

Install zenoh-c, zenoh-cpp and zenoh-bridge-ros2dds:

```bash
curl -L https://download.eclipse.org/zenoh/debian-repo/zenoh-public-key | sudo gpg --dearmor --yes --output /etc/apt/keyrings/zenoh-public-key.gpg
echo "deb [signed-by=/etc/apt/keyrings/zenoh-public-key.gpg] https://download.eclipse.org/zenoh/debian-repo/ /" | sudo tee /etc/apt/sources.list.d/zenoh.list > /dev/null
sudo apt update
sudo apt install libzenohc-dev libzenohcpp-dev zenoh-bridge-ros2dds
```

## Build the project

We are using [just](https://github.com/casey/just) to simplify the command.
You can install it via `sudo apt install just`.

* Build the whole project
  * **NOTE**: To avoid the potential conflict of CycloneDDS library, please **DON'T** build the example under the ROS environment (i.e. don't run `source /opt/ros/$ROS_DISTRO/setup.bash`).

```bash
just all
# Option: You can build the project step by step
just prepare               # Init submodule
just cyclonedds            # Install CycloneDDS
just cyclonedds-cxx        # Install CycloneDDS C++
just bridge_sub            # Build the bridge example
just rmw_zenoh_sub         # Build the rmw zenoh example
```

* Clean the whole project

```bash
just clean
```

## Usage

### Using **`rmw_cylonedds_cpp`**, subscribing through **`zenoh-bridge-ros2dds`**

* Run your ROS 2 program with CycloneDDS

  ```bash
  source /opt/ros/$ROS_DISTRO/setup.bash
  export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
  # Your ROS 2 program
  ```

* Run the `zenoh-bridge-ros2dds`

  ```bash
  zenoh-bridge-ros2dds
  ```

  * You can also run the bridge with the provided configuration that allows only the configured topics to be routed to Zenoh:

    ```bash
    zenoh-bridge-ros2dds -c bridge/config/bridge-config.json5
    ```

* Run the subscriber
  * Replace localhost with the IP that runs the bridge.

  ```bash
  # DON'T source ROS environment in the terminal to avoid CycloneDDS library conflict
  ./install/bin/bridge_sub -e tcp/localhost:7447
  ```

### Using **`rmw_zenoh_cpp`**, subscribing directly

* Run Zenoh router

  ```bash
  source /opt/ros/$ROS_DISTRO/setup.bash
  export RMW_IMPLEMENTATION=rmw_zenoh_cpp
  ros2 run rmw_zenoh_cpp rmw_zenohd
  ```

  * You can also run the zenohd with the provided configuration that allows only the configured topics to be accessed externally:

    ```bash
    source /opt/ros/$ROS_DISTRO/setup.bash
    export RMW_IMPLEMENTATION=rmw_zenoh_cpp
    export ZENOH_ROUTER_CONFIG_URI=$PWD/rmw_zenoh/config/ROUTER_CONFIG.json5
    ros2 run rmw_zenoh_cpp rmw_zenohd
    ```

* Run your ROS 2 program with `rmw_zenoh`

  ```bash
  source /opt/ros/$ROS_DISTRO/setup.bash
  export RMW_IMPLEMENTATION=rmw_zenoh_cpp
  # Your ROS 2 program
  ```

* Run the subscriber
  * Replace localhost with the IP that runs zenohd.

  ```bash
  # DON'T source ROS environment in the terminal to avoid CycloneDDS library conflict
  ./install/bin/rmw_zenoh_sub -e tcp/localhost:7447
  ```
