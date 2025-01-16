<#
Utility functions for Powershell

Author: Jonathan Llewellyn <inexistenz@gmail.com> 2022-12-12
#>

function Get-WSL-IP
{
<#
.SYNOPSIS

Get the WSL IP address from ipconfig and return it

.OUTPUTS

System.String. The IP address of the WSL. Empty string if not found.
#>

    [array] $IP_INFO = ipconfig

    $SECTION = $IP_INFO -like "*Ethernet adapter vEthernet (WSL*"

    if (-Not $SECTION)
    {
        return ""
    }

    $SECION_IDX = $IP_INFO.IndexOf($SECTION)
    $IPV4_LINE = $($IP_INFO[$($SECION_IDX..$($SECION_IDX+5))]) -like "*IPv4 Address*"

    $WSL_IP_ADDR = $($($IPV4_LINE -split ":")[1]).Trim()

    return $WSL_IP_ADDR
}

Function Check-Path
{
<#
.SYNOPSIS

Check if path exists and is a directory

.INPUTS

System.String. The Path to check.

.OUTPUTS

System.Bool. $true if path is a valid directory. $false otherwise.
#>
    param([string]$PATH_TO_CHECK)

    if (-Not "${PATH_TO_CHECK}")
    {
        Write-Host "Error: no path provided"
        return $false
    }

    if (-Not $(Test-Path -Path "${PATH_TO_CHECK}"))
    {
        Write-Host "Error: \"${PATH_TO_CHECK}\" does not exist"
        return $false
    }

    if (-Not $(Test-Path -Path "${PATH_TO_CHECK}" -PathType Container))
    {
        Write-Host "Error: \"${PATH_TO_CHECK}\" is not a directory"
        return $false
    }

    return $true
}
