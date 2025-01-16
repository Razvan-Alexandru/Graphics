#!/usr/bin/env bash
# Utilities: contains the check_path function
#
# Author: Jonathan Llewellyn <inexistenz@gmail.com> 2022-12-12

###############################################################################
# Check if path exists and is a directory
#
# Arguments:
#   The path to check
# Outputs:
#   Error message to STDERR
# Returns:
#   0 if path is valid directory, 1 on error
###############################################################################
function check_path() {
    local PATH_TO_CHECK="$1"

    if [[ -z "${PATH_TO_CHECK}" ]]; then
        echo "Error: no path provided" >&2
        return 1
    fi

    if [[ ! -e "${PATH_TO_CHECK}" ]]; then
        echo "Error: \"${PATH_TO_CHECK}\" does not exist" >&2
        return 1
    fi

    if [[ ! -d "${PATH_TO_CHECK}" ]]; then
        echo "Error: \"${PATH_TO_CHECK}\" is not a directory" >&2
        return 1
    fi

    return 0
}
