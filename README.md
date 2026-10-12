# PS4Arduino
This library will allow you to turn your Arduino into a controller fully compatible with PS4!

**IMPORTANT: You must install the PS4Arduino-AVR core for the library to work. Instructions [here](https://github.com/Flamethr0wer/PS4Arduino-AVR/blob/master/README.md). \
Once installed, you need to select "____ as PS4 controller" as the board.**

## How to use
Really simple! This library includes 6 functions to cover all inputs:

- `PS4controller.begin()`: call this to initialize the library.

- `PS4controller.setButton(button, state)`: sets the state of a button. It takes two parameters: the button, which can be one of the following:
  - `TRIANGLE`
  - `CIRCLE`
  - `CROSS`
  - `SQUARE`
  - `SHARE`
  - `OPTIONS`
  - `PS`
  - `L1`
  - `L2`
  - `L3`
  - `R1`
  - `R2`
  - `R3`
  - `TOUCHPAD`
  
  and the state, which can be either `true`/`1` or `false`/`0`.

- `PS4controller.setDpad(direction)`: sets the direction of the dpad. Takes 1 parameter, which can be:
  - `DPAD_N`
  - `DPAD_NE`
  - `DPAD_E`
  - `DPAD_SE`
  - `DPAD_S`
  - `DPAD_SW`
  - `DPAD_W`
  - `DPAD_NW`
  - `DPAD_RELEASED`

  (the letters stand for the cardinal directions, e.g. NE is the top and right buttons pressed simultaneously).

- `PS4controller.setTrigger(trigger, value)`: this is the function to use if you want to press L2/R2 but not fully (useful for throttle/braking). Takes 2 parameters: the trigger (`LEFT`/`RIGHT`) and a value from 0 to 255 (0 is fully unpressed and 255 is fully pressed).

- `PS4controller.setJoystick(joystick, axis, value)`: sets the value of a joystick axis. Takes three parameters: the joystick (`LEFT`/`RIGHT`), the axis (`X`/`Y`) and a value from 0 to 255. 128 is the middle.

- `PS4controller.maintainConnection()`: You must call this frequently (e.g. once per loop) to prevent the board from being disconnected from your PS4.

And that's about it! You can take a look at [this example](https://github.com/Flamethr0wer/PS4Arduino/blob/main/examples/usage_example.ino) for further reference.

## Currently supported boards:
- Arduino Leonardo
- Arduino Leonardo ETH
- Arduino Micro
- Arduino Esplora


## Setup checklist

1. Install the compatible **PS4Arduino-AVR core** using its own installation instructions.
2. Select the board entry provided by that core in the Arduino IDE.
3. Install this library and open `examples/usage_example.ino`.
4. Compile for the supported board before connecting it to a console.
5. Keep `PS4controller.maintainConnection()` running frequently in your main loop, as the example demonstrates.

## Troubleshooting

- **Board option is missing:** confirm the custom core was installed in the correct Arduino hardware directory, then restart the IDE.
- **Compilation fails:** check that the selected board is supported and that the installed core version matches the library.
- **Controller disconnects:** make sure the loop calls `maintainConnection()` regularly; long blocking delays can prevent timely connection maintenance.
- **Inputs behave unexpectedly:** test one control at a time and use the documented value ranges (joystick axes and triggers use values from 0 to 255; 128 is joystick center).

This library relies on a compatible custom core; it is not a plug-and-play replacement for every Arduino board.
