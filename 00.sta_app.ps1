
# Change code page to UTF-8
$PSDefaultParameterValues['Out-File:Encoding'] = 'utf8'
CLS

if (Test-Path .venv\Scripts\Activate.ps1) {
    .venv\Scripts\Activate.ps1
} else {
    Write-Host "Virtual environment does not exist just skipped."
}

$host.ui.RawUI.WindowTitle = "Zephyr-RTOS BUILD ENVIRONMENT"
$cwd = Get-Location

# Option "zephyr", "gnuarmemb"
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR   = "C:/toolchain/zephyr_rtos/sdk/0.17.1"
$env:GNUARMEMB_TOOLCHAIN_PATH = "C:/toolchain/gnuarmemb/12.2.rel1"
$env:ZEPHYR_BASE  = "C:/toolchain/zephyr_rtos/zephyr"

$env:CMAKE_PATH   = "C:/toolchain/cmake/3.30.3/bin"         # cmake.exe
$env:NINJA_PATH   = "C:/toolchain/ninja/1.11.1"             # ninja.exe
$env:DTC_PATH     = "C:/toolchain/dtc/1.5.1"                # dtc.exe
$env:GCC_ARM_PATH = "C:/toolchain/gnuarmemb/12.2.rel1/bin"
$env:STM32_PROG_PATH = "C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin"

$env:Path = $env:CMAKE_PATH + ';' + $env:NINJA_PATH + ';' + $env:GCC_ARM_PATH + ';' + $env:DTC_PATH + ';' + $env:STM32_PROG_PATH + ';' + $env:Path

# Get-ChildItem env:

# Check first argument for target type
$target_variant = $args[0]

if ($target_variant -eq "trr") {
    $build_dir  = "cmake_build\trr_l475_iot1"
    $board_name = "trr_l475_iot1"
    $prj_conf   = "prj.conf"
}
else {
    Write-Host "Invalid target variant specified. " -ForegroundColor Red;
    Write-Host "Please use " -ForegroundColor Blue -NoNewline;
    Write-Host "'trr'" -ForegroundColor Magenta;
    exit 1
}

Write-Host " Welcome to Zephyr-RTOS build environment" -ForegroundColor White

# cmake -B $build_dir -G Ninja -D BOARD=$board_name -D BOARD_ROOT=. -D CONF_FILE=$prj_conf
Write-Host " cmake " -ForegroundColor Red  -NoNewline;
Write-Host "-B " -ForegroundColor Blue -NoNewline;
Write-Host "$build_dir " -ForegroundColor Magenta -NoNewline;
Write-Host "-G " -ForegroundColor Blue -NoNewline;
Write-Host "Ninja " -ForegroundColor Magenta -NoNewline;
Write-Host "-D " -ForegroundColor Blue -NoNewline;
Write-Host "BOARD" -ForegroundColor Magenta -NoNewline;
Write-Host "=" -ForegroundColor Blue -NoNewline;
Write-Host "$board_name " -ForegroundColor Yellow -NoNewline;
Write-Host "-D " -ForegroundColor Blue -NoNewline;
Write-Host "BOARD_ROOT" -ForegroundColor Magenta -NoNewline;
Write-Host "=" -ForegroundColor Blue -NoNewline;
Write-Host ". " -ForegroundColor Magenta -NoNewline;
Write-Host "-D " -ForegroundColor Blue -NoNewline;
Write-Host "CONF_FILE" -ForegroundColor Magenta -NoNewline;
Write-Host "=" -ForegroundColor Blue -NoNewline;
Write-Host "$prj_conf" -ForegroundColor Magenta;
Write-Host ""

# ninja -C $build_dir
Write-Host " example build command :" -ForegroundColor Green
Write-Host " ninja " -ForegroundColor Red  -NoNewline;
Write-Host "-C " -ForegroundColor Blue -NoNewline;
Write-Host "$build_dir " -ForegroundColor Magenta;
Write-Host ""

# New-Item -ItemType Junction -Path "cmake_build" -Target "T:\cmake_build\$board_name\"
Write-Host " example create junction :" -ForegroundColor Green
Write-Host " New-Item " -ForegroundColor Red  -NoNewline;
Write-Host "-ItemType " -ForegroundColor Blue -NoNewline;
Write-Host "Junction " -ForegroundColor Magenta -NoNewline;
Write-Host "-Path " -ForegroundColor Blue -NoNewline;
Write-Host """cmake_build"" " -ForegroundColor Magenta -NoNewline;
Write-Host "-Target " -ForegroundColor Blue -NoNewline;
Write-Host """T:\cmake_build\$board_name\"" " -ForegroundColor Magenta;

# Create Junction (if there is RAMDISK on T:\)
# New-Item -ItemType Junction -Path "cmake_build\$board_name" -Target "T:\cmake_build\$board_name\"
if ((Test-Path -Path "T:\") -and (-not (Test-Path -Path "T:\cmake_build\$board_name\"))) {
    New-Item -ItemType "directory" -Path "T:\cmake_build\$board_name\"
    New-Item -ItemType Junction -Path "cmake_build\$board_name" -Target "T:\cmake_build\$board_name\"
}

# Creation of SBOM
# west spdx --init -d cmake_build
# west build -d cmake_build -b $board_name -- -D BOARD_ROOT=.
# west spdx -d cmake_build
# @see https://tools.spdx.org/app/

# Building for a board revision
#          <board>@<revision>
# -D BOARD=$board_name@2.0

#Invoke-Expression -Command "cmake -B cmake_build -G Ninja -D BOARD=nucleo_u575zi_q -D CONF_FILE=dbg.conf"
#Invoke-Expression -Command "cmake -B cmake_build -G Ninja -D BOARD=$board_name@2.0 -D BOARD_ROOT=. -D CONF_FILE=prj.conf"
Invoke-Expression -Command "cmake -B cmake_build/$board_name -G Ninja -D BOARD=$board_name -D BOARD_ROOT=."
