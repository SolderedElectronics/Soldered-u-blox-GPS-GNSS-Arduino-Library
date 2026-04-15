/**
 **************************************************
 * @file        Nanosecond_MaxOutput.ino
 * @brief       Maximises GNSS output rate and reads nanosecond-precision
 *              timestamps from a u-blox module.
 *
 * @details     Sets navigation frequency to 5 Hz, increases the I2C clock
 *              to 400 kHz, and raises the serial baud rate to 500000 bps to
 *              keep up with the data rate. Reads latitude, longitude,
 *              altitude, satellites in view, date/time, and nanosecond
 *              sub-second offset via getPVT() on each fresh solution.
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
 * - Current update rate, then latitude, longitude (degrees * 10^-7),
 *   altitude (mm), satellites in view, date/time, and nanoseconds printed
 *   at up to 5 Hz.
 *
 * Notes:
 * - Nanosecond values can be negative; this is normal and indicates the
 *   sub-second offset relative to the top of the second.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      davidallenmann @ SparkFun Electronics
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
  //myGNSS.saveConfiguration(); //Optional: Save the current settings to flash and BBR

  myGNSS.setNavigationFrequency(5); //Set output to 5 times a second
  
  byte rate = myGNSS.getNavigationFrequency(); //Get the update rate of this module
  Serial.print("Current update rate: ");
  Serial.println(rate);
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
    Serial.print(" nanoseconds: ");
    Serial.print(myGNSS.getNanosecond()); // Nanoseconds can be negative

    myGNSS.flushPVT();

    Serial.println();
  }
}
