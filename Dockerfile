FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install basic tools and prerequisites
RUN apt-get update && \
    apt-get install -y \
        curl gpg ca-certificates wget \
        git build-essential cmake pkg-config \
        python3 python3-pip \
        just \
        && rm -rf /var/lib/apt/lists/*

# Install zenoh-c, zenoh-cpp and zenoh-bridge-ros2dds (prerequisite)
RUN curl -L https://download.eclipse.org/zenoh/debian-repo/zenoh-public-key | gpg --dearmor --yes --output /etc/apt/keyrings/zenoh-public-key.gpg && \
    echo "deb [signed-by=/etc/apt/keyrings/zenoh-public-key.gpg] https://download.eclipse.org/zenoh/debian-repo/ /" | tee -a /etc/apt/sources.list > /dev/null && \
    apt-get update && \
    apt-get install -y libzenohc-dev libzenohcpp-dev zenoh-bridge-ros2dds && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# Copy repository content, including submodules
COPY . /workspace

# Build the project using just
RUN just all

# Set PATH to include installed binaries
ENV PATH="${PATH}:/workspace/install/bin"
