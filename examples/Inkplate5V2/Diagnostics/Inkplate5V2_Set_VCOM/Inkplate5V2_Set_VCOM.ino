/**
 **************************************************
 * @file        Inkplate5v2_Set_VCOM.ino
 * @brief       Programs the e-paper panel VCOM voltage on Inkplate 5v2 by
 *              writing the value into the panel driver EEPROM.
 *
 * @details     This utility sketch is used to set the VCOM voltage for the
 *              specific e-paper panel installed in your Inkplate 5v2. VCOM is a
 *              critical panel bias setting that affects contrast, ghosting, and
 *              overall image quality. Some panels ship with an “optimal VCOM”
 *              value printed on the flat-flex cable/connector; this sketch lets
 *              you enter that value and permanently store it.
 *
 *              The sketch reads the current VCOM setting from the TPS65186
 *              e-paper power/driver IC over I2C, displays it on screen, and
 *              shows a grayscale test pattern for visual inspection. When you
 *              type a new VCOM value into the Serial Monitor (range [-5.00, 0]
 *              volts, negative), the sketch programs the TPS65186 VCOM EEPROM,
 *              verifies the write by reading the registers back, and then
 *              updates the on-screen readout and test pattern.
 *
 *              VCOM is stored both in the panel driver EEPROM and in ESP32
 *              EEPROM storage for later reference. Because the driver EEPROM
 *              has limited write endurance, VCOM should be set once and then
 *              left unchanged.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      none
 *
 * Configuration:
 * - Boards Manager -> Inkplate Boards -> Soldered Inkplate5v2
 * - Serial Monitor: 115200 baud, line ending optional
 * - Enter VCOM as a negative decimal number (use '.' as decimal separator)
 *
 * Don't have Inkplate Boards in Arduino Boards Manager?
 * See https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 *
 * How to use:
 * 1) Upload the sketch and open Serial Monitor at 115200 baud.
 * 2) Read the printed label on the panel flex cable (if available) to find the
 *    recommended VCOM value (typically a negative voltage).
 * 3) Type the VCOM value (e.g., -1.23) into Serial Monitor and send it.
 * 4) The sketch programs the TPS65186 VCOM EEPROM, verifies the write, and
 *    redraws the current VCOM readout and grayscale bars.
 * 5) After successful programming, avoid rerunning the sketch unless necessary.
 *
 * Expected output:
 * - Display shows "Current VCOM: <value> V" and a grayscale bar test pattern.
 * - Serial prints instructions, echoes the entered VCOM, warns on out-of-range
 *   input, and reports verification success/failure.
 *
 * Notes:
 * - Display mode: 3-bit grayscale (INKPLATE_3BIT) for the test pattern display.
 * - WARNING: Programming VCOM writes to the TPS65186 EEPROM, which has limited
 *   write cycles. Do not repeatedly program VCOM as it may wear out the EEPROM.
 * - Valid input range enforced by the sketch is [-5.00, 0.00] V (negative).
 * - einkOn()/einkOff() control panel power while accessing the driver; the
 *   sketch toggles wake/power states during programming and verification.
 * - I2C address 0x48 is used for TPS65186 register access in this sketch.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 *
 * @author      Soldered
 * @date        2025-07-29
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include <EEPROM.h>
#include <Inkplate.h>
#include <Wire.h>

Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set the grayscale to 3bit.


double currentVCOM; //Stores the current VCOM value stored on EEPROM
const int EEPROMAddress=0; //Should leave the address as it is for correct EEPROM reading later
double vcomVoltage;

double readPanelVCOM();
double getVCOMFromSerial(double *_vcom);
uint8_t writeVCOMToEEPROM(double v);
void displayTestImage();
void writeReg(uint8_t reg, float data);
uint8_t readReg(uint8_t reg);

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
  
  if (Serial.available()){
    //Serial.println("Enter VCOM value, it must be [-5, 0]");
    do{
      getVCOMFromSerial(&vcomVoltage);
      Serial.println(vcomVoltage, 2);
      display.display();
      if(vcomVoltage < -5.0 || vcomVoltage > 0.0){
        Serial.println("VCOM out of range! [-5, 0]");
      }
    }while(vcomVoltage <-5.0 || vcomVoltage > 0.0);

    //Program the panel EEPROM
    display.internalIO.pinMode(6, INPUT_PULLUP, true);
    if(writeVCOMToEEPROM(vcomVoltage)){
      EEPROM.put(EEPROMAddress, vcomVoltage);
      EEPROM.commit();
    }
    displayTestImage();
  }

}

double readPanelVCOM(){
  delay(10); //Wake up TPS65186 so registers respond
  uint8_t vcomL=readReg(0x03); // REad low 8 bits from register 0x03
  uint8_t vcomH = readReg(0x04) & 0x01; // Read full byte, mask off all but bit 0 (MSB)
  delay(10); //Power down driver
  int raw=(vcomH << 8) | vcomL; //Value between 0 - 511
  return -(raw/100.0);
}

double getVCOMFromSerial(double *_vcom){
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

uint8_t writeVCOMToEEPROM(double v){
  //Build a 9-bit raw value (0 - 511)
  int raw=int(abs(v)*100)&0x1FF;
  uint8_t lsb=raw & 0xFF;
  uint8_t msb=(raw >> 8)&0x01;
  
  display.einkOn();
  delay(10);

  writeReg(0x03, lsb);
  uint8_t r4=readReg(0x04)&~0x01;
  writeReg(0x04, r4 | msb);
  writeReg(0x04, (r4 | msb) | (1 << 6));
  while ( display.internalIO.digitalRead(6, true) ) {
    delay(1);
  }
  readReg(0x07);  // clear interrupt flag
  writeReg(0x03, 0);
  writeReg(0x04, 0);
  display.einkOff(); // WAKEUP low
  delay(10);
  display.einkOn();  // WAKEUP high
  delay(10);
  uint8_t vL = readReg(0x03);
  uint8_t vH = readReg(0x04) & 0x01;
  int check = (vH << 8) | vL;
  if (check != raw) {
    Serial.printf("Verification failed: got %d, want %d\n", check, raw);
    return 0;
  }
  Serial.println("VCOM EEPROM PROGRAMMING OK");
  return 1;
}
void writeReg(uint8_t reg, float data){
  Wire.beginTransmission(0x48);
  Wire.write(reg);
  Wire.write((uint8_t)data);
  Wire.endTransmission();
}
uint8_t readReg(uint8_t reg){
  Wire.beginTransmission(0x48);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)0x48, (uint8_t)1);
  return Wire.read();
}
void displayTestImage() {
    display.clearDisplay();
    currentVCOM = readPanelVCOM();

    display.setTextColor(BLACK);
    display.setTextSize(2);
    display.setCursor(5, 5);
    display.print("Current VCOM: ");
    display.print(currentVCOM, 2);
    display.print(" V");

    for (int i = 0; i < 8; i++) {
        int x = (display.width() / 8) * i;
        display.drawRect(x, 40, display.width() / 8, display.height(), i);
        display.fillRect(x, 40, display.width() / 8, display.height(), i);
    }
    display.display();
}