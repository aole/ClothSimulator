param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release'
)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (!(Test-Path $vswhere)) {
        throw 'Install Visual Studio 2022 or 2026 Build Tools with Desktop development with C++, Windows SDK, and CMake tools.'
    }
    $vsPath = & $vswhere -latest -version '[17.0,19.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1
    if (!$vsPath) { throw 'Visual Studio 2022 or 2026 C++ build tools were not found.' }
    $vsVersion = & $vswhere -latest -version '[17.0,19.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion
    $generator = if ($vsVersion.StartsWith('18.')) { 'Visual Studio 18 2026' } else { 'Visual Studio 17 2022' }
    $env:VCPKG_VISUAL_STUDIO_PATH = $vsPath
    $cmake = Join-Path $vsPath 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
    if (!(Test-Path $cmake)) {
        $cmake = (Get-Command cmake -ErrorAction Stop).Source
    }
    $vcpkg = Join-Path $PSScriptRoot '.tools\vcpkg'
    $baseline = (Get-Content vcpkg.json -Raw | ConvertFrom-Json).'builtin-baseline'
    if (!(Test-Path "$vcpkg\scripts\buildsystems\vcpkg.cmake")) {
        & git clone https://github.com/microsoft/vcpkg.git $vcpkg
        if ($LASTEXITCODE -ne 0) { throw 'Could not download vcpkg.' }
        & git -C $vcpkg checkout $baseline
        if ($LASTEXITCODE -ne 0) { throw 'Could not select the pinned vcpkg version.' }
    }
    if (!(Test-Path "$vcpkg\vcpkg.exe")) {
        & "$vcpkg\bootstrap-vcpkg.bat" -disableMetrics
        if ($LASTEXITCODE -ne 0) { throw 'vcpkg bootstrap failed.' }
    }
    & $cmake -S . -B build -G $generator -A x64 "-DCMAKE_GENERATOR_INSTANCE=$vsPath" "-DCMAKE_TOOLCHAIN_FILE=$vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows
    if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
    & $cmake --build build --config $Configuration --parallel
    if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }
    Write-Host "Built build\$Configuration\ClothSimulator.exe. Run .\run.ps1 -Configuration $Configuration"
}
finally { Pop-Location }
