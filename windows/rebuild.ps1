<#
Delete the COMP371 container and image and rebuild
#>

Start-Process PowerShell -ArgumentList "$PSScriptRoot\delete_container.ps1" -Wait

Start-Process PowerShell -ArgumentList "$PSScriptRoot\delete_image.ps1" -Wait

Start-Process PowerShell -ArgumentList "$PSScriptRoot\build_docker.ps1" -Wait

