# Use official Ubuntu image
FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    g++ \
    libgtest-dev \
    libtbb-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
WORKDIR /app
COPY . .

# Build Google Test
RUN cd /usr/src/gtest && \
    cmake CMakeLists.txt && \
    make && \
    cp *.a /usr/lib

# Build project
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

# Run tests
CMD ["cd build && ctest --output-on-failure"]