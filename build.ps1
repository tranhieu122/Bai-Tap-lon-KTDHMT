# build.ps1 - Automated Build Script for CityStreet3D
# --------------------------------------------------

$msbuildPaths = @(
    "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
)

$msbuild = ""
foreach ($path in $msbuildPaths) {
    if (Test-Path $path) {
        $msbuild = $path
        break
    }
}

if ($msbuild -eq "") {
    Write-Host "Error: MSBuild.exe not found in common locations." -ForegroundColor Red
    Write-Host "Please ensure Visual Studio is installed with C++ development tools."
    Write-Host "If installed, please add MSBuild to your PATH or update this script with your path."
    exit 1
}

Write-Host "Found MSBuild at: $msbuild" -ForegroundColor Cyan
Write-Host "Starting build for CityStreet3D (x64 Debug)..." -ForegroundColor Yellow

& $msbuild "CityStreet3D.vcxproj" /p:Configuration=Debug /p:Platform=x64 /t:Rebuild

if ($lastExitCode -eq 0) {
    Write-Host "Build Successful!" -ForegroundColor Green
    Write-Host "Executable location: x64\Debug\CityStreet3D.exe" -ForegroundColor Green
} else {
    Write-Host "Build Failed. Please check the errors above." -ForegroundColor Red
}
