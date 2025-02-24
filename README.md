# Robodog
3D printed, STM32/ESP32-powered, mini robot dog based on James Bruton's miniDog

### File organization

  * `stm32/`: main controller (STM32) code
    * `stm32/platformio`: PlatformIO project where all STM32 code will live
    * `stm32/cube`: STM32Cube compatible project to facilitate code gen using CubeMX
  * `esp32/`: wireless module (ESP32) PlatformIO project/code
  * `ext/`: non-embedded code/data, i.e. interfaces on host computer, MATLAB analyses
  * `altium/`: schematics and PCB layouts for optional custom PCBs

### Team

  * [Joe Dai](https://github.com/jdtech3/)
  * [Swarnava Ghosh](https://github.com/swarnavaghosh04/)
