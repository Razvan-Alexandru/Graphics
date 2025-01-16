#!/usr/bin/env bash
# Build docker image named comp371:W22 from current directory
# using Dockerfile.comp371

docker build "${PWD}" -f Dockerfile.comp371 -t comp371:W22
