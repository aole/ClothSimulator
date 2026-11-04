param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release'
)
$ErrorActionPreference = 'Stop'
$output = Join-Path $PSScriptRoot "build\$Configuration"
$exe = Join-Path $output 'ClothSimulator.exe'
if (!(Test-Path $exe)) {
    & "$PSScriptRoot\build.ps1" -Configuration $Configuration
}
Start-Process -FilePath $exe -WorkingDirectory $output
