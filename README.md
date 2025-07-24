# zenoh-ros-examples

## Prerequisite

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
