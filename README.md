# TRR_L475_IOT1 (B-L475E-IOT01A) Zephyr Project

This repository contains the firmware and board support for the TRR_L475_IOT1 (B-L475E-IOT01A) IoT development board, based on the STM32L475 MCU, using the Zephyr RTOS.

## Features
- GPIO, I2C, SPI, PWM, ADC, DAC, DMA, RTC, QSPI, NVS, Watchdog, Counter, USB Device, timer
- Sensors: HTS221 (humidity/temperature), LPS22HB (pressure), LSM6DSL (IMU), VL53L0X (proximity), LIS3MDL (magnetic)
- Bluetooth LE (BLE) support, WiFi, MQTT

## Directory Structure

- `boards/st/trr_l475_iot1/` - Board definition, device tree, and documentation
- `code/app_mcu/00.src/` - Main application source code
  - `drv/` - Device drivers (sensors, GPIO, etc.)
  - `tsk/` - Application tasks (environment sensor, BLE, WiFi, etc.)
  - `utl/` - Utility code
- `code/app_mcu/01.inc/` - C++ header files

## Build Instructions

1. **Install Zephyr SDK and Tools**  
   Follow the [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).

2. **Initialize and Build**
   ```sh
   west init -l .
   west update
   west build -b trr_l475_iot1 .
   ```

   Or with CMake/Ninja:
   ```sh
   cmake -Bcmake_build/trr_l475_iot1 -DBOARD=trr_l475_iot1
   ninja -C cmake_build/trr_l475_iot1
   ```

3. **Flash to Board**
   ```sh
   west flash
   ```
   Or use the provided PowerShell scripts.

## Configuration

- Main project configuration: `prj.conf`
- Board device tree: `boards/st/trr_l475_iot1/trr_l475_iot1.dts`
- Board YAML: `boards/st/trr_l475_iot1/trr_l475_iot1.yaml`

## License

SPDX-License-Identifier: Apache-2.0
