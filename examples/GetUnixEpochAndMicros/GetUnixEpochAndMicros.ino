/**
 **************************************************
 * @file        GetUnixEpochAndMicros.ino
 * @brief       Retrieves the current Unix epoch timestamp and sub-second
 *              microsecond offset from a u-blox GNSS module.
 *
 * @details     Calls getUnixEpoch() twice: once for a rounded integer epoch
 *              and once with a microsecond reference parameter for full
 *              precision. Also reads date/time fields and reports whether
 *              the time is fully resolved, valid, and confirmed. Works best
 *              on modules that support confirmedTime (e.g. ZED-F9P); modules
 *              like ZOE-M8Q do not support confirmedTime.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module (ZED-F9P recommended for full
 *               confirmed-time support)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Rounded and exact Unix epoch values with microseconds, date/time string,
 *   time-validity flags, and satellites in view, printed once per second.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      UT2UH @ SparkFun Electronics
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

  // Uncomment the next line if you need to completely reset your module
  //myGNSS.factoryDefault(); delay(5000); // Reset everything and wait while the module restarts

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  //myGNSS.saveConfiguration();        //Optional: Save the current settings to flash and BBR

  Serial.println(F("Compare Unix Epoch given with reference one from https://www.epochconverter.com/"));

}

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    // getUnixEpoch marks the PVT data as stale so you will get Unix time and PVT time on alternate seconds

    uint32_t us;  //microseconds returned by getUnixEpoch()
    uint32_t epoch = myGNSS.getUnixEpoch();
    Serial.print(F("Unix Epoch rounded: "));
    Serial.print(epoch, DEC);    
    epoch = myGNSS.getUnixEpoch(us);
    Serial.print(F("  Exact Unix Epoch: "));
    Serial.print(epoch, DEC);
    Serial.print(F("  micros: "));
    Serial.println(us, DEC);

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
    Serial.print(myGNSS.getSecond());
    
    Serial.print(F("  Time is "));
    if (myGNSS.getTimeFullyResolved() == false)
    {
      Serial.print(F("not fully resolved but "));
    } else {
      Serial.print(F("fully resolved and "));
    }
    if (myGNSS.getTimeValid() == false)
    {
      Serial.print(F("not "));
    }
    Serial.print(F("valid "));
    if (myGNSS.getConfirmedTime() == false)
    {
      Serial.print(F("but not "));
    } else {
      Serial.print(F("and "));
    }
    Serial.print(F("confirmed"));

    byte SIV = myGNSS.getSIV();
    Serial.print(F("  SIV: "));
    Serial.println(SIV);
  }
}
