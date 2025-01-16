#!/usr/bin/env bash
# Delete the COMP371 container and image and rebuild

"$(dirname ${BASH_SOURCE[0]})"/delete_container.sh

"$(dirname ${BASH_SOURCE[0]})"/delete_image.sh

"$(dirname ${BASH_SOURCE[0]})"/build_docker.sh
