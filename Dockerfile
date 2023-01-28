FROM ubuntu:20.04

ENV DEBIAN_FRONTEND='noninteractive'
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8
ENV CXX_VERSION 14

# Install minimum tools for run applications

WORKDIR /temp

RUN apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y dist-upgrade && \
    apt-get -y autoremove && \
    apt-get install -y \
    git wget curl unzip \
    build-essential gdb clang-format cmake lcov \
    libssl-dev libperlio-gzip-perl libjson-perl \
    libpq-dev libsqlite3-dev && \
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/lib/apt/lists/*

RUN cd /temp/ && \
    git clone https://github.com/Tencent/rapidjson.git && cd rapidjson/ && \
    git submodule update --init && mkdir build && cd build && \
    cmake  .. && make -j$(nproc) && make install

RUN cd /temp/ && \
    wget -O opencv.zip https://codeload.github.com/opencv/opencv/zip/refs/tags/4.6.0 && \
    unzip opencv.zip && cd opencv-4.6.0/ && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_LIST=core,highgui,improc,imgcodecs .. && \
    make -j$(nproc) && \
    make install

# Cleanup
RUN apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/lib/apt/lists/*
RUN rm -rf /temp/*

# App directory
WORKDIR /workspace/image-resizer-app/

COPY include include/
COPY src src/
COPY tests tests/
COPY CMakeLists.txt ./
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)
