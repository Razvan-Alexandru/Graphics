#!/usr/bin/env bash
# This script opens the comp371:W22 Docker container with the given path bound to
# `/COMP371` and opens and interactive Bash shell
#
# Author: Jonathan Llewellyn <inexistenz@gmail.com> 2022-12-12

source "$(dirname ${BASH_SOURCE[0]})/utils.sh"

###############################################################################
# Print usage and exits with 1
#
# Arguments:
#   None
# Outputs:
#   Usage message to STDERR
###############################################################################
function usage() {
    echo "Usage: $0 /path/to/code" >&2
    exit 1
}

###############################################################################
# Run the container with an interactive Bash shell and X11 Display binding.
#
# Globals:
#   DISPLAY
# Arguments:
#   Path to bind to `/COMP371` in container. Must be absolute.
###############################################################################
function main() {
    local CODE_PATH="$1"

    check_path "${CODE_PATH}"

    if [[ $? -ne 0 ]]; then
        usage
    fi

# Breaking down the docker command:
#
# docker run \
#     --name COMP371 \ # Name of the container
#     --rm \ # Delete container after exiting
#     -v $CODE_PATH:/COMP371 \ # Binds CODE_PATH to /src/code inside the container
#     -v /tmp/.X11-unix:/tmp/.X11-unix:rw \ # Required for X Server use on Unix/Linux
#     -e DISPLAY=$DISPLAY \ # Required for X Server use on Unix/Linux
#     --entrypoint /bin/bash  # Enter through bash shell
#     -it \ # Use interactive and TTY terminal
#     comp371:W22 # Name of the image (main positional argument for docker run)
    docker run --name COMP371 \
        --rm \
        -v ${CODE_PATH}:/COMP371 \
        -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
        -e DISPLAY=${DISPLAY} \
        --entrypoint /bin/bash \
        -it \
        comp371:W22
}

main $@
