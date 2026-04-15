/**
 **************************************************
 * @file        PartialSecond_MaxOutput.ino
 * @brief       Reads millisecond and nanosecond sub-second timestamps at
 *              10 Hz from a u-blox GNSS module with maximised throughput.
 *
 * @details     Increases navigation frequency to 10 Hz, raises the I2C
 *              clock to 400 kHz, and sets serial to 500000 bps to handle
 *              the increased data rate. Calls getPVT() on each fresh
 *              solution and prints a full date/time string with millisecond
 *              precision (zero-padded) plus raw nanoseconds.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 500000 baud (not the standard 115200).
 *
 * Expected output:
 * - Latitude, longitude (degrees * 10^-7), altitude (mm), satellites in
 *   view, date/time with milliseconds, and nanoseconds, printed at up to
 *   10 Hz.
 *
 * Notes:
 * - Not all modules support 10 Hz with all constellations enabled; if the
 *   rate drops to 1 Hz, reduce the navigation frequency.
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
  Serial.begin(500000); //Increase serial speed to maximize
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();
  Wire.setClock(400000); // Increase I2C clock speed to 400kHz

  //myGNSS.enableDebugging(); //Uncomment this line to enable debug messages over Serial

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)

  // Note: not all u-blox modules can output solutions at 10Hz - or not while tracking all satellite constellations
  // If the rate drops back to 1Hz, you're asking too much of your module
  myGNSS.setNavigationFrequency(10);           //Set output to 10 times a second
  
  byte rate = myGNSS.getNavigationFrequency(); //Get the update rate of this module
  Serial.print("Current update rate:");
  Serial.println(rate);

  //myGNSS.saveConfiguration(); //Optional: Save the current settings to flash and BBR
}

void loop()
{
  // Calling getPVT returns true if there actually is a fresh navigation solution available.
  if (myGNSS.getPVT())
  {
    lastTime = millis(); //Update the timer

    long latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.print(F(" (mm)"));

    byte SIV = myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.print(" ");
    Serial.print(myGNSS.getYear());
    Serial.print("-");
    Serial.print(myGNSS.getMonth());
    Serial.print("-");
    Serial.print(myGNSS.getDay());
    Serial.print(" ");
    Serial.print(myGNSS.getHour());
    Serial.print(":");
    Serial.print(myGNSS.getMinute());
    Serial.print(":");
    Serial.print(myGNSS.getSecond());
    Serial.print(".");
    //Pretty print leading zeros
    int mseconds = myGNSS.getMillisecond();
    if (mseconds < 100)
      Serial.print("0");
    if (mseconds < 10)
      Serial.print("0");
    Serial.print(mseconds);

    Serial.print(" nanoseconds: ");
    Serial.print(myGNSS.getNanosecond());

    Serial.println();
  }
}
