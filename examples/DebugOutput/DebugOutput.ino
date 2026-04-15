/**
 **************************************************
 * @file        DebugOutput.ino
 * @brief       Shows how to enable, configure, and disable library debug
 *              output on a u-blox GNSS module.
 *
 * @details     Calls enableDebugging() to direct packet and status messages
 *              to the Serial port. Debug can be restricted to critical
 *              messages only by passing a second argument. After 20 readings
 *              the example automatically calls disableDebugging() to silence
 *              the output. The same position/time data as in the GetPosition
 *              example is printed alongside the debug messages.
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
 * - Verbose library debug messages interleaved with latitude, longitude,
 *   altitude, satellites in view, and timestamp. Debug stops after 20
 *   readings.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

unsigned long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.
int counter = 0; // Disable the debug messages when counter reaches 20

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

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  //myGNSS.saveConfiguration(); //Optional: Save the current settings to flash and BBR

  myGNSS.enableDebugging(); //Enable all the debug messages over Serial (default)
  
  //myGNSS.enableDebugging(SerialUSB); //Enable debug messages over Serial USB

  //myGNSS.enableDebugging(Serial, true); //Enable only the critical debug messages over Serial
  
}

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
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

    Serial.print(F("   "));
    Serial.print(myGNSS.getYear());
    Serial.print(F("-"));
    Serial.print(myGNSS.getMonth());
    Serial.print(F("-"));
    Serial.print(myGNSS.getDay());
    Serial.print(F(" "));
    Serial.print(myGNSS.getHour());
    Serial.print(F(":"));
    Serial.print(myGNSS.getMinute());
    Serial.print(F(":"));
    Serial.println(myGNSS.getSecond());
    
    Serial.println();

    counter++; // Increment counter
    if (counter == 20)
    {
      myGNSS.disableDebugging(); // Disable the debug messages when counter reaches 20
    }
  }
}
