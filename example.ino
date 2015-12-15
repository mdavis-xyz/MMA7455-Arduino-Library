// Example code for my MMA7455 accelerometer library
// This uses I2C to talk to the MMA7455 accelerometer
//
// Written by Matthew Davis
// December 2015
//
// Distributed under the GNU Public Licence
//
// Wiring:
// Connect GND to GND, and VCC (on the sensor) to 5V (on the Arduino)
// Connect CS to VCC = 5V
// Connect SCL on the sensor to A5 on the Uno
// Connect SDA on the sensor to A4 on the Uno
//
// this example code works out which way the device is pointing
// and prints it through serial
//
// it assumes the device starts horizontal (the device callibrates itself
// in the setup phase). Change FIRST_ORIENTATION to change that.
//
//


#include <Wire.h>
#include "MMA7455.h"

//this is the orientation it starts in
// for callibration purposes
#define FIRST_ORIENTATION Y_NEG

orientation previous_orientation = FIRST_ORIENTATION;

void sendNewOrientation(orientation next_orientation);

void setup()
{
    int error;
    uint8_t c;

    Serial.begin(9600);
    Serial.println("Freescale MMA7455 accelerometer");
    Serial.println("May 2012");

    // Initialize the 'Wire' class for I2C-bus communication.
    Wire.begin();


    // Initialize the MMA7455, and set the offset.
    error = MMA7455_init(FIRST_ORIENTATION);

    if (error == 0){
        Serial.println("The MMA7455 is okay");
    }else{
        Serial.println("Check your wiring !");
    }

    // Read the Status Register
    MMA7455_status(&c);
    Serial.print("STATUS : ");
    Serial.println(c,HEX);

    // Read the "Who am I" value
    MMA7455_who_am_I(&c);
    Serial.print("WHOAMI : ");
    Serial.println(c,HEX);

    Serial.print("Assuming the device is ");
    sendNewOrientation(FIRST_ORIENTATION);
}



void loop()
{
    orientation next_orientation = MMA7455_orientation();
    if((next_orientation != previous_orientation) &&
       (next_orientation != NOT_SURE)){
        previous_orientation = next_orientation;

        sendNewOrientation(next_orientation);
    }
    delay(200);
}

void sendNewOrientation(orientation next_orientation){
    char * dir;
    switch(next_orientation){
        case X_POS:
            dir="X direction up";
            break;
        case X_NEG:
            dir="X direction down";
            break;
        case Y_POS:
            dir="Y direction up";
            break;
        case Y_NEG:
            dir="Y direction down";
            break;
        case Z_POS:
            dir="Z direction up";
            break;
        case Z_NEG:
            dir="Z direction down";
            break;
        default:
            dir="unknown direction";
            break;
    }
    Serial.print("The device is now ");
    Serial.println(dir);
}
