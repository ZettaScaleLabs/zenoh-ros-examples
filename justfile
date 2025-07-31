all: prepare cyclonedds zenoh-bridge-examples

# Initialize git submodules
prepare:
    git submodule init
    git submodule update

# Build the CycloneDDS library
cyclonedds:
	mkdir -p cyclonedds/build cyclonedds/install
	cd cyclonedds/build && \
		cmake -DCMAKE_INSTALL_PREFIX=../install .. && \
		cmake --build . && \
		cmake --build . --target install

# Build the zenoh-bridge-examples with the path of CycloneDDS and the idlc
zenoh-bridge-examples:
	mkdir -p bridge/build
	# Use the idlc we built
	export PATH=$(pwd)/cyclonedds/install/bin:$PATH && \
	cd bridge/build && \
		cmake -DCMAKE_PREFIX_PATH=../cyclonedds/install .. && \
		cmake --build .

# Clean the build folder
clean:
	rm -rf build
	rm -rf cyclonedds/build
