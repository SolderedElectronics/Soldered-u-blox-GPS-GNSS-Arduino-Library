/**
 **************************************************
 * @file        PowerSaveMode.ino
 * @brief       Enables and disables UBX-CFG-RXM power save mode on a
 *              u-blox GNSS module interactively from the Serial Monitor.
 *
 * @details     Sends '1' to enable power save mode (powerSaveMode()) or
 *              '2' to disable it (powerSaveMode(false)). After each
 *              command, getPowerSaveMode() reads back the current low-power
 *              mode value. Position is polled every 10 seconds. For the
 *              ZOE-M8Q with a passive antenna, current drops from ~25-28 mA
 *              to ~9 mA in power save mode.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox M8 GNSS module (e.g. ZOE-M8Q)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) Send '1' to enable power save mode, '2' to disable it.
 *
 * Expected output:
 * - Mode change confirmation and current low-power mode value; position and
 *   fix type printed every 10 seconds.
 *
 * Notes:
 * - UBX-CFG-RXM is not supported on ZED-F9P (protocol version >= 27);
 *   this example will fail on that module.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark (PaulZC) @ SparkFun Electronics
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
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //myGNSS.enableDebugging(); // Uncomment this line to enable debug messages

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  //myGNSS.saveConfiguration(); //Optional: Uncomment this line to save the current settings to flash and BBR

  Serial.println("Power save example.");
  Serial.println("1) Enable power saving");
  Serial.println("2) Disable power saving");
}

void loop()
{
  if (Serial.available())
  {
    byte incoming = Serial.read();

    if (incoming == '1')
    {
      // Put the GNSS into power save mode
      // (If you want to disable power save mode, call myGNSS.powerSaveMode(false) instead)
      // This will fail on the ZED (protocol version >= 27) as UBX-CFG-RXM is not supported
      if (myGNSS.powerSaveMode()) // Defaults to true
        Serial.println(F("Power Save Mode enabled."));
      else
        Serial.println(F("*** Power Save Mode FAILED ***"));
    }
    else if (incoming == '2')
    {
      //Go to normal power mode (not power saving mode)
      if (myGNSS.powerSaveMode(false))
        Serial.println(F("Power Save Mode disabled."));
      else
        Serial.println(F("*** Power Save Disable FAILED ***"));
    }

    // Read and print the new low power mode
    uint8_t lowPowerMode = myGNSS.getPowerSaveMode();
    if (lowPowerMode == 255)
    {
      Serial.println(F("*** getPowerSaveMode FAILED ***"));
    }
    else
    {
      Serial.print(F("The low power mode is: "));
      Serial.print(lowPowerMode);
      if (lowPowerMode == 0)
      {
        Serial.println(F(" (Continuous)"));
      }
      else if (lowPowerMode == 1)
      {
        Serial.println(F(" (Power Save)"));
      }
      else if (lowPowerMode == 4)
      {
        Serial.println(F(" (Continuous)"));
      }
      else
      {
        Serial.println(F(" (Unknown!)"));
      }
    }
  }

  //Query module every 10 seconds so it is easier to monitor the current draw
  if (millis() - lastTime > 10000)
  {
    lastTime = millis(); //Update the timer

    byte fixType = myGNSS.getFixType(); // Get the fix type
    Serial.print(F("Fix: "));
    Serial.print(fixType);
    if (fixType == 0)
      Serial.print(F("(No fix)"));
    else if (fixType == 1)
      Serial.print(F("(Dead reckoning)"));
    else if (fixType == 2)
      Serial.print(F("(2D)"));
    else if (fixType == 3)
      Serial.print(F("(3D)"));
    else if (fixType == 4)
      Serial.print(F("(GNSS + Dead reckoning)"));

    long latitude = myGNSS.getLatitude();
    Serial.print(F(" Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.print(F(" (mm)"));

    Serial.println();
  }
}
