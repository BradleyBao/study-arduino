# Arduino car (PlatformIO)

The firmware in `BuildCar/` targets an Arduino Mega 2560. Build it with
`cd BuildCar && pio run` and upload with `pio run --target upload`.

## Automatic obstacle course

The car starts in automatic mode. Send `t` over the 9600 baud serial connection
to switch between automatic and manual control. Manual keys are `w/a/s/d` for
movement, `q/e/z/c` for diagonal movement, and space or `x` to stop.

The automatic route is shaped like `|-|`, with the horizontal segment at the
top: approach the first barrier, turn toward its open side, cross the top,
turn the **same direction** at the far barrier, then drive down the second leg.
It stops when it reaches a third barrier. The first turn chooses left or right
from the measured opening. The second turn uses that same direction, even if
the other side appears more open. A missing echo or blocked required turn stops
the motors; an automatic stop lasting five seconds switches to manual mode.

While going straight, the servo periodically checks the nearest wall. Its
distance reading changes the left and right motor PWM to correct drift. The
car stops during a side scan and rechecks the front before driving again. No
line sensor is needed for this route.

The main tuning values are near the top of `BuildCar/src/functions.cpp`:

| Value | Purpose |
| --- | --- |
| `AUTO_SPEED` / `AUTO_RIGHT_SPEED` | Base left and right PWM; adjust the right trim for your motors. |
| `OBSTACLE_THRESHOLD_CM` | Stop distance before a barrier. |
| `SIDE_CLEAR_CM` | Minimum opening for a turn. |
| `TURN_90_MS` / `TURN_LIMIT_MS` | Initial 90-degree spin and maximum allowed spin. |
| `SERVO_SETTLE_MS` | Time for the ultrasonic sensor to face a new direction. |

The side-wall correction compensates for unequal wheel speeds when a wall is
visible. Without a wall, the fixed PWM trim is the only straight-line
correction. A single ultrasonic sensor and one encoder channel cannot measure
the car's exact heading or guarantee a 90-degree turn, so calibrate the turn
time on the real surface. Put the car on blocks for the first motor check.

## Connections

| Device | Pins |
| --- | --- |
| Front right motor | PWM 9; direction 22, 24 |
| Front left motor | PWM 10; direction 26, 28 |
| Rear right motor | PWM 11; direction 5, 6 |
| Rear left motor | PWM 12; direction 7, 8 |
| Ultrasonic trigger / echo | 30 / 38 |
| Scan servo | 13 |
| Optional encoder A / B | 20 / 21 |
| Reflectors, used by the separate line follower | 2, 3, 4 |

The motor drivers need their own supply and a common ground with the Arduino.
Servo angle 0 is treated as left and 180 as right; swap these in software if
your sensor is mounted the opposite way. The sideways motor commands assume
mecanum wheels, while automatic corner turns use spin commands 8 and 9.
