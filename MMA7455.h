// This library is for using the
// MMA7455 accelerometer as tilt sensor
// ---------------------
//
// Based on code by arduino.cc user "Krodal".
// May 2012
// http://playground.arduino.cc/Main/MMA7455
// Then modified by Matthew Davis
// December 2015
//
//
// Using Arduino 1.0.1
// It will not work with an older version, since Wire.endTransmission()
// uses a parameter to hold or release the I2C bus.
//
// Documentation:
//     - The Freescale MMA7455L datasheet
//     - The AN3468 Application Note (programming).
//     - The AN3728 Application Note (calibrating offset).
//
//
// Wiring:
// Connect GND to GND, and VCC (on the sensor) to 5V (on the Arduino)
// Connect CS to VCC = 5V
// Connect SCL on the sensor to A5 on the Uno
// Connect SDA on the sensor to A4 on the Uno

#ifndef MMA7455H
#define MMA7455H


// Default I2C address for the MMA7455
#define MMA7455_I2C_ADDRESS 0x1D

// Look at the PCB, see the x axis, z axis and y axis printed on the board.
// that's what these refer to.
// e.g. Z_POS is when the board is horizontal, with chips on the top
typedef enum {Y_POS, Y_NEG, X_POS, X_NEG, Z_POS, Z_NEG, NOT_SURE} orientation;


////////////////////////////////////////////////////////////
// tilt functions
////////////////////////////////////////////////////////////


// --------------------------------------------------------
// MMA7455_init
//
// Initialize the MMA7455.
//
// Set also the offset, assuming that the accelerometer is
// in a horizontal or vertical position, with axis "up"
// facing up.
// e.g. Z_POS for when the board is horizontal, with chips on the top
//
// Don't set up to NOT_SURE if you're not sure. Find out which one.
//
// Important notes about the offset:
//    The offset is influenced by the slightest vibration
//    (like a computer on the table).
//
int MMA7455_init(orientation up);

// --------------------------------------------------------
// MMA7455_orientation
//
// returns the current orientation of the device
// e.g. Z_POS for when the board is horizontal, with chips on the top
//
// More specifically, it returns the axis which has over THRESH g-force
// in either direction (returning *_POS or *_NEG depending on direction)
// If no axis has more than THRESH g-force, or more than 1 axis does,
// then it returns NOT_SURE
#define THRESH 0.8 //g-force
orientation MMA7455_orientation(void);





////////////////////////////////////////////////////////////
// misc functions
////////////////////////////////////////////////////////////

// You shouldn't need the following functions when using the sensor
// as a tilt sensor
// I'm just leaving them here just in case

// --------------------------------------------------------
// MMA7455_xyz
//
// Get the 'g' forces.
// The values are with integers as 64 per 'g'.
//
int MMA7455_xyz( int16_t *pX, int16_t *pY, int16_t *pZ);

// --------------------------------------------------------
// MMA7455_read
//
// This is a common function to read multiple bytes
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus.
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read.
// There is no function for a single byte.
//
int MMA7455_read(int start, uint8_t *buffer, int size);

// --------------------------------------------------------
// MMA7455_write
//
// This is a common function to write multiple bytes
// to an I2C device.
//
// Only this function is used to write.
// There is no function for a single byte.
//
int MMA7455_write(int start, const uint8_t *pData, int size);


// --------------------------------------------------------
// MMA7455_status
//
// reads the status of the MMA7455
//writes it into the "status" variable
//returns nothing
void MMA7455_status(uint8_t * status);


// --------------------------------------------------------
// MMA7455_who_am_I
//
// reads the "Who am I" value of the MMA7455
//writes it into the "value" variable
//returns nothing
void MMA7455_who_am_I(uint8_t * value);


#endif
