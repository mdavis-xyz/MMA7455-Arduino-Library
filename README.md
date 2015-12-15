# MMA7455 Arduino Library

This is a library for using the [MMA7455 accelerometer ](http://playground.arduino.cc/Main/MMA7455) as a *tilt sensor*, with an Arduino, through I<sup>2</sup>C. A *tilt sensor* simply tells you which way the device is oriented (e.g. Y axis up). This library abstracts away all the messy detail of the accelerometer stuff.

![MMA7455 Image - from above](http://imgur.com/0RAcA57.jpg)

![MMA7455 Image - from above](http://imgur.com/Ew7YtrO.jpg)

It is based on the code from the [Arduino website](http://playground.arduino.cc/Main/MMA7455).

## Usage

To use this library, simply paste `MMA7455.h` and `MMA7455.ino` into the same folder as your main `.ino` file. Then add the following line to the top of your code.

    #include "MMA7455.h"

If you only want to use this device as a simple 3-axis (or fewer) tilt sensor, you only need to consider two functions. The other functions are included just in case.

First *initialise* the device, using the following function. The argument `up` is set to the orientation the device starts in, which is `Y_POS`, `Y_NEG`, `X_POS`, `X_NEG`, `Z_POS` or `Z_NEG`. For example, in the first photo above, the device is in the `Z_POS` position, because the positive z axis is pointing up. In the 2nd photo it's in the `Z_NEG` position.

    int MMA7455_init(orientation up);

Similarly, to *read* what orientation the device is in *after* initialising in a known orientation, use the following function.

    orientation MMA7455_orientation(void);

This function returns `Y_POS`, `Y_NEG`, `X_POS`, `X_NEG`, `Z_POS`, `Z_NEG` or `NOT_SURE`. This function looks for which axis is measuring acceleration of about 1 g-force. (More specifically, greater than `THRESH` g-force.) If no axis is measuring close to 1 g-force, or multiple are, then it returns `NOT_SURE`.

The default I<sup>2</sup>C address for this device is `0x1D`. If you are having trouble communicating to the device, change the following line in `MMA7455.h`.

    #define MMA7455_I2C_ADDRESS 0x1D

## Example

There is an example program included. This prints the new orientation through serial, whenever the orientation changes.

To use it, start the device in the `Z_POS` position (like the first photo above). Then run the code. Open the serial monitor and see what it printed as you rotate the device.
