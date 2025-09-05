all: prepare cyclonedds cyclonedds-cxx bridge_sub rmw_zenoh_sub

# Initialize git submodules
prepare:
    git submodule init
    git submodule update

# Build the CycloneDDS library
cyclonedds:
	mkdir -p install
	mkdir -p cyclonedds/build
	cd cyclonedds/build && \
		cmake -DCMAKE_INSTALL_PREFIX=../../install .. && \
		cmake --build . && \
		cmake --build . --target install
	
# Build the CycloneDDS C++ library
cyclonedds-cxx: cyclonedds
	mkdir -p cyclonedds-cxx/build
	cd cyclonedds-cxx/build && \
		cmake -DCMAKE_INSTALL_PREFIX=../../install -DCMAKE_PREFIX_PATH=../install .. && \
		cmake --build . && \
		cmake --build . --target install

# Build the bridge_sub examples with the path of CycloneDDS and the idlc
bridge_sub:
	mkdir -p bridge_sub/build
	# Use the idlc we built
	export PATH=$(pwd)/cyclonedds/install/bin:$PATH && \
	cd bridge_sub/build && \
		cmake -DCMAKE_INSTALL_PREFIX=../../install -DCMAKE_PREFIX_PATH=../install .. && \
		cmake --build . && \
		cmake --build . --target install

# Build the rmw_zenoh_sub examples with the path of CycloneDDS and the idlc
rmw_zenoh_sub:
	mkdir -p rmw_zenoh_sub/build
	# Use the idlc we built
	export PATH=$(pwd)/cyclonedds/install/bin:$PATH && \
	cd rmw_zenoh_sub/build && \
		cmake -DCMAKE_INSTALL_PREFIX=../../install -DCMAKE_PREFIX_PATH=../install .. && \
		cmake --build . && \
		cmake --build . --target install

# Clean the build folder
clean:
	rm -rf bridge_sub/build
	rm -rf rmw_zenoh_sub/build
	rm -rf cyclonedds/build
	rm -rf cyclonedds-cxx/build
	rm -rf install
	