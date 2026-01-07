FROM ubuntu:22.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install only what is needed to run the benchmark
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        gcc \
        libc6-dev \
        make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /bench

# Copy source code
COPY baseline.c .

# Compile the benchmark
RUN gcc -O2 -pthread baseline.c -o baseline.out

# Default command (can be overridden)
ENTRYPOINT ["./baseline.out"]

