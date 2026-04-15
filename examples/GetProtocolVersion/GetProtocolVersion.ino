/**
 **************************************************
 * @file        GetProtocolVersion.ino
 * @brief       Reads and prints the UBX protocol version from a u-blox
 *              GNSS module over I2C.
 *
 * @details     Calls getProtocolVersionHigh() and getProtocolVersionLow()
 *              to retrieve the major.minor protocol version. Protocol
 *              versions range from v18 to v27+ and determine which UBX
 *              commands are available on a given module.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Protocol version string, e.g. "Version: 27.30", printed once at
 *   startup.
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

  Serial.print(F("Version: "));
  byte versionHigh = myGNSS.getProtocolVersionHigh();
  Serial.print(versionHigh);
  Serial.print(".");
  byte versionLow = myGNSS.getProtocolVersionLow();
  Serial.print(versionLow);
}

void loop()
{
  //Do nothing
}
