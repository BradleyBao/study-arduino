# Arduino car (PlatformIO)

The PlatformIO project is in `BuildCar/`. It targets an **Arduino Mega 2560**
using the Arduino framework. No additional libraries are required.

## Run from VS Code

1. Install or enable the **PlatformIO IDE** extension.
2. Open `BuildCar/` as the folder, or open `study-arduino.code-workspace`.
3. In PlatformIO's **Project Tasks**, expand `megaatmega2560` and select
   **General > Build**.
4. Connect the Mega 2560 over USB and select **General > Upload**.

The program starts automatically after upload or reset. It repeatedly requests
forward, backward, left, and right movement, each for three seconds, at PWM
100 out of 255. There is no serial output in the current program.
For the first upload, raise the wheels clear of the surface because the motors
start automatically.

## Run from a terminal

From the repository root:

```sh
cd BuildCar
pio run
pio device list
pio run --target upload
```

If `pio` is not on your shell's PATH, use a PlatformIO terminal in VS Code,
or use the installed executable `~/.platformio/penv/bin/pio` in place of `pio`.
For example:

```sh
~/.platformio/penv/bin/pio run
~/.platformio/penv/bin/pio run --target upload
```

If automatic port detection fails, find the board's port with `pio device list`
and specify it explicitly (replace `/dev/ttyACM0` with the detected port):

```sh
pio run --target upload --upload-port /dev/ttyACM0
```

## Motor connections

`BuildCar/src/main.cpp` expects these connections to motor driver inputs:

| Motor | PWM / enable pin | Direction input 1 | Direction input 2 |
| --- | --- | --- | --- |
| Front right | 9 | 22 | 24 |
| Front left | 10 | 26 | 28 |
| Rear right | 11 | 5 | 6 |
| Rear left | 12 | 7 | 8 |

Use motor drivers and an appropriate motor power supply, with a common ground
between the Arduino and drivers. Arduino GPIO pins cannot power motors directly.

The left/right commands use opposite directions on diagonal pairs of wheels.
This pattern is intended for sideways movement with suitable mecanum wheels;
whether it works depends on wheel arrangement and motor polarity. Ordinary
wheels require different direction patterns for turning.

## Troubleshooting

- **`Arduino.h` missing or red underlines:** open the `BuildCar` project, build
  it once, then run **PlatformIO: Rebuild C/C++ Project Index** from VS Code's
  Command Palette. Use the PlatformIO build task for this firmware.
- **Upload fails:** check the USB data cable, detected port, and that another
  serial monitor is not holding the port. On Linux, a permission error may
  require serial-device access for your user.
- **Wrong movement:** verify the pin table, driver enable configuration, motor
  polarity, and wheel arrangement. A successful build cannot verify the wiring.
