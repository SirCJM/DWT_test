# STM32 DWT Test Project Instructions

## Build Commands
- Configure: `cmake --preset Debug` (or Release)
- Build: `cmake --build --preset Debug`
- Clean: `cmake --build build/Debug --target clean`
- Toolchain: Requires `arm-none-eabi-gcc` (v7+) in PATH, CMake 3.22+, Ninja.

## Architecture
- HAL-based STM32F429 project using STM32CubeMX.
- Components: Core (generated), Drivers (HAL/CMSIS), BSP (custom drivers), App_Middlewares (tasks).
- DWT for precise timing; RTT for debugging.

## Conventions
- Edit only USER CODE sections in generated files ([main.c](Core/Src/main.c), [gpio.c](Core/Src/gpio.c)).
- Custom code in BSP/ or App_Middlewares/.
- Regenerate from [DWT_test.ioc](DWT_test.ioc) after config changes.
- Prefix functions with module (e.g., DWT_Init).

## Pitfalls
- Ensure arm-none-eabi-gcc is installed; build fails otherwise.
- DWT requires CoreDebug enable; delays assume constant 180MHz clock.
- No automated tests; verify via debug/RTT.
- Flash manually with ST-Link tools (e.g., STM32CubeProgrammer).

## Key Files
- [drv_dwt.c](BSP/Src/drv_dwt.c): DWT cycle counting for delays.
- [CMakeLists.txt](CMakeLists.txt): Build configuration.
- [cmake/gcc-arm-none-eabi.cmake](cmake/gcc-arm-none-eabi.cmake): Toolchain settings.