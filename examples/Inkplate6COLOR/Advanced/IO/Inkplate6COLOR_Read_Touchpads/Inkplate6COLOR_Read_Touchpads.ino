/*
   Inkplate6_Read_Touchpads example for Soldered Inkplate 6COLOR
   For this example you will need only a micro USB cable and Inkplate 6COLOR.
   Select "e-radionica Inkplate6COLOR" or "Soldered Inkplate6COLOR" from Tools -> Board menu.
   Don't have "e-radionica Inkplate6" or "Soldered Inkplate6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use built-in touchpads (on PCB marked with numbers 1, 2 and 3).
   These are basically touch sensitive switches. You can read state each of these with function touchpad.read()


   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 July 2020 by Soldered
*/


#include "Inkplate.h"            //Include Inkplate library to the sketch
Inkplate display; // Create an object on Inkplate library

void setup()
{
    Serial.begin(115200);               // Init Serial documentation
    display.begin();                    // Init Inkplate library (you should call this function ONLY ONCE)
}

void loop()
{
    if (display.touchpad.read(1))
    { 
        Serial.println("Pad 1 pressed!");
    }

    if (display.touchpad.read(2))
    { 
        Serial.println("Pad 2 pressed!");
    }

    if (display.touchpad.read(3))
    { 
        Serial.println("Pad 3 pressed!");
    }
    delay(100); // Wait a little bit between readings.
}

