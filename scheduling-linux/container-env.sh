#!/bin/sh
# Container environment for scheduler testing
#
# NOTE: Containers share the host kernel!!! Only userspace isolation!!!
# - Scheduler changes affect the host kernel scheduler
# - Only provides namespace isolation (filesystem, PIDs, network)
# - For true isolation, use a VM instead
#
# Useful for: clean build environment, process isolation, reproducible tests

podman run --rm -it \
    --cap-add=SYS_NICE \
    -v "$(pwd)":/root \
    -w /root \
    docker.io/debian:trixie-slim \
    /bin/bash
