<#
Run the comp371:W22 Docker container to build and run the code at the given
path which is bound to `/COMP371`. The project is named after the first
argument.

Author: Jonathan Llewellyn <inexistenz@gmail.com> 2022-12-12
#>

. $PSScriptRoot\utils.ps1

Function Usage
{
<#
.SYNOPSIS

Print usage and exits with 1
#>

    Write-Host "Usage: $PSCommandPath project_name /path/to/code"
    exit 1
}

Function Main 
{
<#
.SYNOPSIS

Run the container to build and run the program with an X11 Display binding.

.INPUTS

System.String. Name of the CMake project.
System.String. Path to bind to `/COMP371` in container. Must be absolute.
#>
    param([string]$PROJECT_NAME,
          [string]$CODE_PATH)

    if( -Not $PROJECT_NAME)
    {
        Write-Host "Error: no project_name provided"
        Usage
    }

    if(-Not $(Check-Path "${CODE_PATH}"))
    {
        Usage
    }

    $WSL_IP_ADDR =  Get-WSL-IP

    if (-Not "${WSL_IP_ADDR}") 
    {
        Write-Host "Error: No WSL IP address detected"
        exit 1
    }

    $DISPLAY = "${WSL_IP_ADDR}:0.0"

# Breaking down the docker command:
#
# docker run \
#     --name COMP371 \ # Name of the container
#     --rm \ # Delete container after exiting
#     -v $CODE_PATH:/COMP371 \ # Binds CODE_PATH to /src/code inside the container
#     -e DISPLAY=$DISPLAY \ # Required for X Server
#     comp371:W22 \ # Name of the image (main positional argument for docker run)
#     $PROJECT_NAME

    docker run --name COMP371 --rm -v ${CODE_PATH}:/COMP371 -e DISPLAY=${DISPLAY} comp371:W22 $PROJECT_NAME
}

if ($args.Count -ne 2)
{
    Usage
}

Main $args[0] $args[1]

