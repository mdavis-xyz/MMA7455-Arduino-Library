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


// this example code works out which way the device is pointing
// and prints it through serial
//
// it assumes the device starts horizontal (the device callibrates itself in the setup phase)
// then during the loop phase, it assumes the device is vertical

//when stationary, the device will measure 1 g-force in the 'up' direction

#include <Wire.h>
#include "MMA7455.h"


// the device looks for which direction has this much force
#define THRESH 0.8 //g-force

typedef enum {Y_POS, Y_NEG, X_POS, X_NEG, Z_POS, Z_NEG, NOT_SURE} orientation;

orientation previous_orientation = Z_POS;

orientation getOrientation(void);

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
    error = MMA7455_init(Z); // assuming it starts horizontal
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

}



void loop()
{
    orientation next_orientation = getOrientation();
    if((next_orientation != previous_orientation) &&
       (next_orientation != NOT_SURE)){
        previous_orientation = next_orientation;

        sendNewOrientation(next_orientation);
    }
    delay(200);
}

orientation getOrientation(void){
    uint16_t x,y,z, error;
    double dX,dY,dZ;

    // The function MMA7455_xyz returns the 'g'-force
    // as an integer in 64 per 'g'.

    // set x,y,z to zero (they are not written in case of an error).
    x = y = z = 0;
    error = MMA7455_xyz(&x, &y, &z); // get the accelerometer values.
    //
    dX = ((int16_t) x) / ((double) MMA7455_ONE_G); // calculate the 'g' values.
    dY = ((int16_t) y) / ((double) MMA7455_ONE_G);
    dZ = ((int16_t) z) / ((double) MMA7455_ONE_G);

    // ignore z value
    if(dX > THRESH){
        return X_POS;
    }else if(dX <  - THRESH){
        return X_NEG;
    }else if(dY > THRESH){
        return Y_POS;
    }else if(dY <  - THRESH){
        return Y_NEG;
    }else if(dZ > THRESH){
        return Z_POS;
    }else if(dZ <  - THRESH){
        return Z_NEG;
    }else{
        return NOT_SURE;
    }
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
