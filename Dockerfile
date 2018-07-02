# openCV install instructions from
# https://blogs.wcode.org/2014/10/howto-install-build-and-use-opencv-macosx-10-10/

# To build copy (INCLUDE THE FINAL DOT)
# docker build -t docker-alpr .

# For ARM architectures use
FROM arm32v7/ubuntu:latest

# For x86 architectures use
# FROM ubuntu:latest

# Install dependencies
# Important to keep the '-y' to say 'yes' to the prompt or will raise non-zero error.

RUN apt-get update \
    && apt-get install -y build-essential \
                          sudo \
                          cmake \
                          git \
                          nano \
                          wget \
                          libgtk2.0-dev \
                          pkg-config \
                          libavcodec-dev \
                          libavformat-dev \
                          libswscale-dev \
                          libmysqlcppconn-dev \
                          python-dev \
                          python-numpy \
                          libtbb2 \
                          libtbb-dev \
                          libjpeg-dev \
                          libpng-dev \
                          libtiff-dev \
                          libdc1394-22-dev \
# Compile and install openCV
    && git clone https://github.com/opencv/opencv.git \
    && cd opencv \
    && mkdir build \
    && cd build \
    && cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. \
    && make \
    && make install \
# Link opencv libraries
    && echo '/usr/local/lib/' > /etc/ld.so.conf.d/opencv.conf \
    && sudo ldconfig -v

# Run start script when starting the container not when building the image
CMD sh /root/start.sh
