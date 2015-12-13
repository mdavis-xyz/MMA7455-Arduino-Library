
#include "MMA7455.h"
#include <Wire.h>



// --------------------------------------------------------
// MMA7455_init
//
// Initialize the MMA7455.
// Set also the offset, assuming that the accelerometer is
// in flat horizontal position.
//
// Important notes about the offset:
//    The sensor has internal registers to set an offset.
//    But the offset could also be calculated by software.
//    This function uses the internal offset registers
//    of the sensor.
//    That turned out to be bad idea, since setting the
//    offset alters the actual offset of the sensor.
//    A second offset calculation had to be implemented
//    to fine tune the offset.
//    Using software variables for the offset would be
//    much better.
//
//    The offset is influenced by the slightest vibration
//    (like a computer on the table).
//
int MMA7455_init(axis up)
{
  uint16_t x, y, z;
  int error;
  xyz_union xyz;
  uint8_t c1, c2;

  // Initialize the sensor
  //
  // Sensitivity:
  //    2g : GLVL0
  //    4g : GLVL1
  //    8g : GLVL1 | GLVL0
  // Mode:
  //    Standby         : 0
  //    Measurement     : MODE0
  //    Level Detection : MODE1
  //    Pulse Detection : MODE1 | MODE0
  // There was no need to add functions to write and read
  // a single byte. So only the two functions to write
  // and read multiple bytes are used.

  // Set mode for "2g sensitivity" and "Measurement Mode".
  c1 = bit(MMA7455_GLVL0) | bit(MMA7455_MODE0);
  error = MMA7455_write(MMA7455_MCTL, &c1, 1);
  if (error != 0)
    return (error);

  // Read it back, to test the sensor and communication.
  error = MMA7455_read(MMA7455_MCTL, &c2, 1);
  if (error != 0)
    return (error);

  if (c1 != c2)
    return (-99);

  // Clear the offset registers.
  // If the Arduino was reset or with a warm-boot,
  // there still could be offset written in the sensor.
  // Only with power-up the offset values of the sensor
  // are zero.
  xyz.value.x = xyz.value.y = xyz.value.z = 0;
  error = MMA7455_write(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
  if (error != 0)
    return (error);

  // The mode has just been set, and the sensor is activated.
  // To get a valid reading, wait some time.
  delay(100);

#define USE_INTERNAL_OFFSET_REGISTERS
#ifdef USE_INTERNAL_OFFSET_REGISTERS

  // Calcuate the offset.
  //
  // The values are 16-bits signed integers, but the sensor
  // uses offsets of 11-bits signed integers.
  // However that is not a problem,
  // as long as the value is within the range.

  // Assuming that the sensor is flat horizontal,
  // the 'z'-axis should be 1 'g'. And 1 'g' is
  // a value of 64 (if the 2g most sensitive setting
  // is used).
  // Note that the actual written value should be doubled
  // for this sensor.

  error = MMA7455_xyz (&x, &y, &z); // get the x,y,z values
  if (error != 0)
    return (error);

  //which axis is up?
  //that one is now measuring 1g, not 0g
  switch(up){
    case(X):
      // The sensor wants double values.
      xyz.value.x = 2 * -(x-MMA7455_ONE_G);
      xyz.value.y = 2 * -y;
      xyz.value.z = 2 * -z;
      break;
    case(Y):
      xyz.value.x = 2 * -x;        // The sensor wants double values.
      xyz.value.y = 2 * -(y-MMA7455_ONE_G);
      xyz.value.z = 2 * -z;
      break;
    case(Z):
      xyz.value.x = 2 * -x;        // The sensor wants double values.
      xyz.value.y = 2 * -y;
      xyz.value.z = 2 * -(z-MMA7455_ONE_G);
      break;
  }


  error = MMA7455_write(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
  if (error != 0)
    return (error);

  // The offset has been set, and everything should be okay.
  // But by setting the offset, the offset of the sensor
  // changes.
  // A second offset calculation has to be done after
  // a short delay, to compensate for that.
  delay(200);

  error = MMA7455_xyz (&x, &y, &z);    // get te x,y,z values again
  if (error != 0)
    return (error);


  //which axis is up?
  //that one is now measuring 1g, not 0g
  switch(up){
    case(X):
      // The sensor wants double values.
      xyz.value.x += 2 * -(x-MMA7455_ONE_G);
      xyz.value.y += 2 * -y;
      xyz.value.z += 2 * -z;
      break;
    case(Y):
      xyz.value.x += 2 * -x;        // The sensor wants double values.
      xyz.value.y += 2 * -(y-MMA7455_ONE_G);
      xyz.value.z += 2 * -z;
      break;
    case(Z):
      xyz.value.x += 2 * -x;        // The sensor wants double values.
      xyz.value.y += 2 * -y;
      xyz.value.z += 2 * -(z-MMA7455_ONE_G);
      break;
  }

  // Write the offset for a second time.
  // This time the offset is fine tuned.
  error = MMA7455_write(MMA7455_XOFFL, (uint8_t *) &xyz, 6);
  if (error != 0)
    return (error);

#endif

  return (0);          // return : no error
}


// --------------------------------------------------------
// MMA7455_xyz
//
// Get the 'g' forces.
// The values are with integers as 64 per 'g'.
//
int MMA7455_xyz( uint16_t *pX, uint16_t *pY, uint16_t *pZ)
{
  xyz_union xyz;
  int error;
  uint8_t c;

  // Wait for status bit DRDY to indicate that
  // all 3 axis are valid.
  do
  {
    error = MMA7455_read (MMA7455_STATUS, &c, 1);
  } while ( !bitRead(c, MMA7455_DRDY) && error == 0);
  if (error != 0)
    return (error);

  // Read 6 bytes, containing the X,Y,Z information
  // as 10-bit signed integers.
  error = MMA7455_read (MMA7455_XOUTL, (uint8_t *) &xyz, 6);
  if (error != 0)
    return (error);

  // The output is 10-bits and could be negative.
  // To use the output as a 16-bit signed integer,
  // the sign bit (bit 9) is extended for the 16 bits.
  if (xyz.reg.x_msb & 0x02)    // Bit 9 is sign bit.
    xyz.reg.x_msb |= 0xFC;     // Stretch bit 9 over other bits.
  else
    xyz.reg.x_msb &= 0x3;

  if (xyz.reg.y_msb & 0x02)
    xyz.reg.y_msb |= 0xFC;
  else
    xyz.reg.y_msb &= 0x3;

  if (xyz.reg.z_msb & 0x02)
    xyz.reg.z_msb |= 0xFC;
  else
    xyz.reg.z_msb &= 0x3;

  // The result is the g-force in units of 64 per 'g'.
  *pX = xyz.value.x;
  *pY = xyz.value.y;
  *pZ = xyz.value.z;

  return (0);                  // return : no error
}


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
int MMA7455_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(MMA7455_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false); // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MMA7455_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);                  // return : no error
}


// --------------------------------------------------------
// MMA7455_write
//
// This is a common function to write multiple bytes
// to an I2C device.
//
// Only this function is used to write.
// There is no function for a single byte.
//
int MMA7455_write(int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(MMA7455_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);                   // return : no error
}

// reads the status of the MMA7455
//writes it into the "status" variable
//returns nothing
void MMA7455_status(uint8_t * status){
    MMA7455_read(MMA7455_STATUS, status, 1);
}


// reads the "Who am I" value of the MMA7455
//writes it into the "value" variable
//returns nothing
void MMA7455_who_am_I(uint8_t * value){
    MMA7455_read(MMA7455_WHOAMI, value, 1);
}


// --------------------------------------------------------
// MMA7455_flag_reset
//
// clears the interrupt flags
// can clear both, or just INT1, or just INT2
// int1 - if true, clear INT1 flag
// int2 - if true, clear INT2 flag

void MMA7455_flag_reset(uint8_t int1, uint8_t int2){
    // see page 19 of datasheet

    uint8_t pData[] = {0x00, 0x00};
    if(int1){
        pData[0] |= bit(0);
    }
    if(int2){
        pData[0] |= bit(1);
    }
    if(pData[0] == 0){
        //neither flag is set, so don't write anything
        return;
    }
    MMA7455_write(MMA7455_INTRST, pData, 2);
}
