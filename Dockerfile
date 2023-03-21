# Fetch ubuntu image
FROM ubuntu:22.04

# Install required stuff
RUN \
	apt update && \
	apt install -y git python3 && \
	apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential
	
# Install PICO SDK
RUN \
	mkdir -p /project/pico/ && \
	cd /project/pico/ && \
	git clone https://github.com/raspberrypi/pico-sdk.git --branch master && \
	cd pico-sdk/ && \
	git submodule update --init && \
	cd /
	
# Set the Pico SDK environment variable
ENV PICO_SDK_PATH=/project/pico/pico-sdk/

# Copy in our source files
RUN \
	mkdir -p /project/pio/WS2812/
COPY pio/* /project/pio/
COPY pio/WS2812/* /project/pio/WS2812/

# Build project
RUN \
	mkdir /project/pio/WS2812/build/ && \
	cd /project/pio/WS2812/build/ && \
	cmake .. && \
	make

# Command that will be invoked when the container starts
ENTRYPOINT ["/bin/bash"]