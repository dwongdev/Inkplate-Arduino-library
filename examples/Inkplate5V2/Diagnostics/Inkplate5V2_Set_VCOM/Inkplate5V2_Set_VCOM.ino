/*
  Inkplate5v2_Set_VCOM sketch for Soldered Inkplate 5v2.
  For this sketch you will need USB and Inkplate 5v2.
  Select "Soldered Inkplate 5v2" from Tools -> Board menu.
  Don't have "Soldered Inkplate 5v2" option? Follow our tutorial and add it:
  https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

  WARNING! - VCOM voltage is written in EEPROM, which means it can be set a limited number of 
  times, so don't run this sketch repeteadly! VCOM should be set once and then left as is.
  Use with caution!

  Want to learn more about Inkplate? Visit https://soldered.com/documentation/inkplate/
  Looking to get support? Write on our forums: https://forum.soldered.com/
  29 July 2025 by Soldered
*/

#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include <EEPROM.h>
#include <Inkplate.h>
#include <Wire.h>

Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set the grayscale to 3bit.

double currentVCOM;         //Stores the current VCOM value stored on EEPROM
const int EEPROMaddress=0;  //Should leave the address as it is for correct EEPROM reading later
double vcomVoltage;

double readPanelVCOM();
double getVCOMFromSerial(double *_vcom);
void displayTestImage();

void setup() {
  Serial.begin(115200);     //Start serial at 115200 baud
  EEPROM.begin(512);        //Initialize EEPROM
  Wire.begin();             //Initialize I2C buss
  display.begin();          //Initialize the Inkplate 
  Serial.println("The optimal VCOM Voltage for your Inkplate's panel can sometimes be found written on the flat cable connector");
  Serial.println("Write VCOM voltage from epaper panel. \r\nDon't forget negative (-) sign!\r\nUse dot as the decimal point.\r\nFor example -1.23\n");
  displayTestImage();
}

void loop() {
  
  if (Serial.available())
  {
    while (true)
    {
      // Get VCOM voltage from serial from user
      uint8_t flag = getVCOMFromSerial(&vcomVoltage);

      // Show the user the entered VCOM voltage
      Serial.print("Entered VCOM: ");
      Serial.println(vcomVoltage);
      display.print(vcomVoltage);
      display.partialUpdate();

      if (display.setVcom(vcomVoltage, EEPROMaddress))
      {
        Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");
        break;
      }
      else
      {
        Serial.println("ERROR");
      }
    }
    displayTestImage();
  }
}

double getVCOMFromSerial(double *_vcom)
{
  double vcom=0;
  char buff[32];
  unsigned long start;
  while (!Serial.available());
  start=millis();
  int idx=0;
  while ((millis()-start)<500 && idx<sizeof(buff)-1){
    if(Serial.available()){
      char c=Serial.read();
      buff[idx++]=c;
      start=millis();
    }
  }
  buff[idx]='\0';
  sscanf(buff, "%lf", &vcom);
  *_vcom=vcom;
  return vcom;
}

void displayTestImage() 
{
  display.clearDisplay();
  currentVCOM = display.getVcomVoltage();

  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(5, 5);
  display.print("Current VCOM: ");
  display.print(currentVCOM, 2);
  display.print(" V");

  for (int i = 0; i < 8; i++) 
  {
    int x = (display.width() / 8) * i;
    display.drawRect(x, 40, display.width() / 8, display.height(), i);
    display.fillRect(x, 40, display.width() / 8, display.height(), i);
  }
  display.display();
}
