/**
 **************************************************
 * @file        GetVal.ino
 * @brief       Read a configuration value from a u-blox module using VALGET.
 *
 * @details     Demonstrates the VALGET command structure introduced in UBX
 *              protocol version 23, reading the module's current I2C address
 *              using getVal8(). The result is shifted to produce the standard
 *              7-bit I2C address format and printed to the Serial Monitor.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox GNSS module with UBX protocol >= v23 (e.g. ZED-F9P)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - "Current I2C address (should be 0x42): 0x42"
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("u-blox getVal example");

  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //myGNSS.enableDebugging(); //Enable debug messages over Serial (default)
  //myGNSS.enableDebugging(SerialUSB); //Enable debug messages over Serial USB

  uint8_t currentI2Caddress = myGNSS.getVal8(UBLOX_CFG_I2C_ADDRESS);
  Serial.print("Current I2C address (should be 0x42): 0x");
  Serial.println(currentI2Caddress >> 1, HEX); //u-blox module returns a shifted 8-bit address. Make it 7-bit unshifted.
}

void loop()
{
  // Nothing to do here
}
