all: prepare cyclonedds cyclonedds-cxx zenoh-bridge-examples

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

# Build the zenoh-bridge-examples with the path of CycloneDDS and the idlc
zenoh-bridge-examples:
	mkdir -p bridge/build
	# Use the idlc we built
	export PATH=$(pwd)/cyclonedds/install/bin:$PATH && \
	cd bridge/build && \
		cmake -DCMAKE_PREFIX_PATH=../install .. && \
		cmake --build .

# Clean the build folder
clean:
	rm -rf bridge/build
	rm -rf cyclonedds/build
	rm -rf cyclonedds-cxx/build
	rm -rf install
	