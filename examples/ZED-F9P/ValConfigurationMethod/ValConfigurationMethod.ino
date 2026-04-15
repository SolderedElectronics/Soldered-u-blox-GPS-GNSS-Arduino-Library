/**
 **************************************************
 * @file        ValConfigurationMethod.ino
 * @brief       Demonstrate VALGET, VALSET, and VALDEL configuration methods.
 *
 * @details     Shows the modern u-blox configuration workflow that replaces
 *              legacy UBX-CFG messages. The sketch reads selected keys,
 *              writes new values, and removes values where required, providing
 *              a compact reference for key-based configuration.
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
 * - Printed key values and status messages for VALGET/VALSET/VALDEL actions.
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
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  byte response;
  response = myGNSS.getVal8(UBLOX_CFG_I2C_ADDRESS, VAL_LAYER_RAM); // Get the I2C address (see u-blox_config_keys.h for details)
  Serial.print(F("I2C Address: 0x"));
  Serial.println(response >> 1, HEX); //We have to shift by 1 to get the common '7-bit' I2C address format

  response = myGNSS.getVal8(UBLOX_CFG_I2COUTPROT_NMEA, VAL_LAYER_RAM); // Get the flag indicating is NMEA should be output on I2C
  Serial.print(F("Output NMEA over I2C port: 0x"));
  Serial.print(response, HEX);
}

void loop()
{
}
