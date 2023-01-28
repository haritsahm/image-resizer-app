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
    build-essential gdb clang-format cmake \
    libssl-dev libperlio-gzip-perl libjson-perl \
    libpq-dev libsqlite3-dev && \
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/lib/apt/lists/*

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
    cmake .. && make -j$(nproc)

CMD ["/bin/bash"]