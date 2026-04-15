/**
 **************************************************
 * @file        AutoPVT_ExplicitUpdate.ino
 * @brief       Configures auto PVT over I2C with explicit checkUblox()
 *              calls to control exactly when the data buffer is parsed.
 *
 * @details     Calls setAutoPVT(true, false) so the library does not
 *              implicitly update stale data when getLatitude() etc. are
 *              called. Instead, checkUblox() is called every 50 ms to drain
 *              the I2C buffer in the background. Position data is printed
 *              every 500 ms. This approach is useful with threaded OSes
 *              (e.g. FreeRTOS on ESP32) where an implicit update could
 *              create race conditions across cores.
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
 * - Latitude, longitude (degrees * 10^-7), altitude (mm), and satellites
 *   in view printed roughly every 500 ms.
 *
 * Notes:
 * - getPVT() returns false in explicit mode; data is accessed directly
 *   from the internal struct without triggering a new I2C poll.
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

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.setNavigationFrequency(2);  //Produce two solutions per second
  myGNSS.setAutoPVT(true, false);    //Tell the GNSS to "send" each solution and the lib not to update stale data implicitly
  //myGNSS.saveConfiguration();        //Optional: Save the current settings to flash and BBR
}

/*
     Calling getPVT would return false now (compare to previous example where it would return true), so we just use the data provided
     If you are using a threaded OS eg. FreeRTOS on an ESP32, the explicit mode of autoPVT allows you to use the data provided on both cores and inside multiple threads
     The data update in background creates an inconsistent state, but that should not cause issues for most applications as they usually won't change the GNSS location significantly within a 2Hz - 5Hz update rate.
     Also you could oversample (10Hz - 20Hz) the data to smooth out such issues...
*/
void loop()
{
  static uint16_t counter = 0;

  if (counter % 10 == 0)
  {
    // update your AHRS filter here for a ~100Hz update rate
    // GNSS data will be quasi static but data from your IMU will be changing
  }
  // debug output each half second
  if (counter % 500 == 0)
  {
    Serial.println();
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

    Serial.println();
  }
  // call checkUblox all 50ms to capture the GNSS data
  if (counter % 50 == 0)
  {
    myGNSS.checkUblox();
  }
  delay(1);
  counter++;
}
