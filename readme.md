# ESP32-BLE-Touchpad-ELAN

 An ESP32-based Bluetooth touchpad project that drives an ELAN controller touchpad to connect to a computer or mobile phone via Bluetooth, providing mouse functionality.

## Motivation

It all started with discomfort—I couldn’t lean back comfortably in my chair without having to keep my hand on the table just to move the mouse. So I searched to see if there were any USB touchpads, but the prices were outrageous. Then I looked for Bluetooth touchpads, but only found ones bundled with mini keyboards.

Eventually, I started looking for ways to build one myself and found many tutorials on converting old touchpads into external mice. BUT THEY WERE ALL SYNAPTICS. Meanwhile, the only touchpad I had was an ELAN, and fortunately, its pinout was easier to identify thanks to its simple labeling:

- TV: VCC
- TG: Ground
- TD: Data
- TC: Clock

With zero Arduino knowledge (of course idk how to use ESP32), I boldly experimented with many things until I finally got it working. Special thanks to Kristopher for the PS2-Mouse-Arduino library, T-vK for the ESP32-BLE-Mouse library, and Gaowanliang for the Synaptics-based references—which I didn’t understand at all. Without them, I would never have been able to create this masterpiece.


## Feature List

- [x] Mouse movement
- [x] Tap to click
- [x] Single-finger double tap as mouse left click (build-in from my touchpad)
- [x] Tap and drag to enable dragging
- [x] Physical left and right mouse button (build-in from my touchpad)

I still don't know how to active multitouch, it just won't give any data if more than 1 finger touch it.

## Compilation

This program is compiled using PlatformIO. You can compile it by installing the PlatformIO plugin in VSCode. The project has already configured the `platformio.ini` file. You just need to open the project folder and click the PlatformIO `Build` button to compile.

You need to modify the 48th and 49th lines in the `src/main.cpp` file, change `#define MDATA` and `#define MCLK` to your actual pins.

```cpp
#define MDATA 5
#define MCLK 23
```

## Contribution

You're welcome to submit issues and pull requests to improve the project.

## Acknowledgments

[@Gaowanliang's ESP32-BLE-Touchpad project](https://github.com/gaowanliang/ESP32-BLE-Touchpad)

[@T-vK's ESP32-BLE-Mouse project](https://github.com/T-vK/ESP32-BLE-Mouse)

[@Kristopher's PS2-Mouse-Arduino project](https://github.com/kristopher/PS2-Mouse-Arduino)

## License

This project is based on the MIT license. For details, please refer to the [LICENSE](LICENSE) file.
