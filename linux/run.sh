#!/usr/bin/env bash
# Run the comp371:W22 Docker container to build and run the code at the given
# path which is bound to `/COMP371`. The project is named after the first
# argument.
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
    echo "Usage: $0 project_name /path/to/code" >&2
    exit 1
}

###############################################################################
# Run the container to build and run the program with an X11 Display binding.
#
# Globals:
#   DISPLAY
# Arguments:
#   Name of the CMake project.
#   Path to bind to `/COMP371` in container. Must be absolute.
###############################################################################
function main() {
    local PROJECT_NAME="$1"
    local CODE_PATH="$2"

    check_path "${CODE_PATH}"

    if [[ $? -ne 0 ]] || [[ $# -ne 2 ]]; then
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
#     comp371:W22 \ # Name of the image (main positional argument for docker run)
#     $PROJECT_NAME
    docker run --name COMP371 \
        --rm \
        -v $CODE_PATH:/COMP371 \
        -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
        -e DISPLAY=$DISPLAY \
        comp371:W22 \
        $PROJECT_NAME
}

main $@
