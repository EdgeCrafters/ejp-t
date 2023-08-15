# Use the specified image as the base
FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Update and install packages
RUN apt-get clean && \
    apt-get update --fix-missing && \
    apt-get install -y \
    cmake \
    ninja-build \
    clang-14 \
    clang++-14 \
    && apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN rm -rf ~/.ssh \
    && ssh-keygen -t rsa -f /root/.ssh/id_rsa -N ""

# Set clang and clang++ as default compilers
ENV CC=clang
ENV CXX=clang++

# Any other necessary setup can be added here

CMD ["/bin/bash"]