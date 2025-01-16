<#
This script opens the comp371:W22 Docker container with the given path bound to
`/COMP371` and opens and interactive Bash shell

Author: Jonathan Llewellyn <inexistenz@gmail.com> 2022-12-12
#>

. $PSScriptRoot\utils.ps1

Function Usage
{
<#
.SYNOPSIS

Print usage and exits with 1
#>

    Write-Host "Usage: $PSCommandPath /path/to/code"
    exit 1
}

Function Main
{
<#
.SYNOPSIS

Run the container with an interactive Bash shell and X11 Display binding.

.INPUTS

System.String. The Path to bind to `/COMP371` in container. Must be absolute.
#>
    param([string]$CODE_PATH)

    if(-Not $(Check-Path "${CODE_PATH}"))
    {
        Usage
    }

    $WSL_IP_ADDR = Get-WSL-IP

    if (-Not "${WSL_IP_ADDR}") 
    {
        Write-Host "Error: No WSL IP address detected"
        exit 1
    }

    # Set $DISPLAY variable (Defaulting to 0.0)
    $DISPLAY = "${WSL_IP_ADDR}:0.0"

# Breaking down the docker command:
#
#    docker run \
#        --name COMP371 \ # Name of the container
#        --rm \ # Delete container after exiting
#        -v $CODE_PATH:/COMP371 \ # Binds CODE_PATH to /src/code inside the container
#        -e DISPLAY=$DISPLAY \ # Required for X Server
#        --entrypoint /bin/bash  # Enter to bash terminal
#        -it \ # Use interactive and TTY terminal
#        comp371:W22 # Name of the image (main positional argument for docker run)

    docker run --name COMP371 --rm -v ${CODE_PATH}:/COMP371 -e DISPLAY=${DISPLAY} --entrypoint /bin/bash -it comp371:W22
}

Main $args[0]
