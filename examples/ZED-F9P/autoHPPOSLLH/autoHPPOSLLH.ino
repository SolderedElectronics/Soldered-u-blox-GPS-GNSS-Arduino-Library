/**
 **************************************************
 * @file        autoHPPOSLLH.ino
 * @brief       Enable automatic HPPOSLLH navigation reports over I2C.
 *
 * @details     Configures the GNSS module to output NAV-HPPOSLLH reports
 *              automatically and reads the latest packet with getHPPOSLLH.
 *              This avoids blocking polls for fresh solutions and is useful
 *              when high-rate position updates are required.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox ZED-F9P (or other High Precision GNSS module)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - High-precision latitude, longitude, and altitude from automatic HPPOSLLH
 *   reports, printed continuously.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable lots of helpful debug messages
  //myGNSS.enableDebugging(Serial, true); // Uncomment this line to enable the minimum of helpful debug messages

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  // Uncomment the next line if you want to reset your module back to the default settings with 1Hz navigation rate
  //myGNSS.factoryDefault(); delay(5000);

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save the communications port settings to flash and BBR

  myGNSS.setNavigationFrequency(1); //Produce one solution per second


  // The acid test: all four of these combinations should work seamlessly :-)

  //myGNSS.setAutoPVT(false); // Library will poll each reading
  //myGNSS.setAutoHPPOSLLH(false); // Library will poll each reading

  //myGNSS.setAutoPVT(true); // Tell the GPS to "send" each solution automatically
  //myGNSS.setAutoHPPOSLLH(false); // Library will poll each reading

  //myGNSS.setAutoPVT(false); // Library will poll each reading
  //myGNSS.setAutoHPPOSLLH(true); // Tell the GPS to "send" each hi res solution automatically

  myGNSS.setAutoPVT(true); // Tell the GPS to "send" each solution automatically
  myGNSS.setAutoHPPOSLLH(true); // Tell the GPS to "send" each hi res solution automatically
}

void loop()
{
  // Calling getHPPOSLLH returns true if there actually is a fresh navigation solution available.
  // Calling getPVT returns true if there actually is a fresh navigation solution available.
  if ((myGNSS.getHPPOSLLH()) || (myGNSS.getPVT()))
  {
    Serial.println();

    long highResLatitude = myGNSS.getHighResLatitude();
    Serial.print(F("Hi Res Lat: "));
    Serial.print(highResLatitude);

    int highResLatitudeHp = myGNSS.getHighResLatitudeHp();
    Serial.print(F(" "));
    Serial.print(highResLatitudeHp);

    long highResLongitude = myGNSS.getHighResLongitude();
    Serial.print(F(" Hi Res Long: "));
    Serial.print(highResLongitude);

    int highResLongitudeHp = myGNSS.getHighResLongitudeHp();
    Serial.print(F(" "));
    Serial.print(highResLongitudeHp);

    unsigned long horizAccuracy = myGNSS.getHorizontalAccuracy();
    Serial.print(F(" Horiz accuracy: "));
    Serial.print(horizAccuracy);

    long latitude = myGNSS.getLatitude();
    Serial.print(F(" Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.println(longitude);
  }
  else
  {
    Serial.print(".");
    delay(50);
  }
}
