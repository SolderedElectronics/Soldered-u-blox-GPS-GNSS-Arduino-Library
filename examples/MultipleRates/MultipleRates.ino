/**
 **************************************************
 * @file        MultipleRates.ino
 * @brief       Configures a u-blox GNSS module to output PVT, POSECEF, and
 *              VELNED messages at different update rates simultaneously.
 *
 * @details     Sets PVT to output every measurement (1 s), POSECEF every
 *              5th measurement (5 s), and VELNED every 10th measurement
 *              (10 s). Each message type is read in the loop only when fresh
 *              data is available, demonstrating how to handle independent
 *              output rates from a single module.
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
 * - Latitude/longitude/altitude (mm) every second from PVT; ECEF
 *   X/Y/Z position (m) every 5 s from POSECEF; NED velocity components
 *   (m/s) every 10 s from VELNED.
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

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.setMeasurementRate(1000); //Produce a measurement every 1000ms
  myGNSS.setNavigationRate(1); //Produce a navigation solution every measurement
  
  myGNSS.setAutoPVTrate(1); //Tell the GNSS to send the PVT solution every measurement
  myGNSS.setAutoNAVPOSECEFrate(5); //Tell the GNSS to send each POSECEF solution every 5th measurement
  myGNSS.setAutoNAVVELNEDrate(10); //Tell the GNSS to send each VELNED solution every 10th measurement
  //myGNSS.saveConfiguration(); //Optional: Save the current settings to flash and BBR
}

void loop()
{
  // Calling getPVT returns true if there actually is a fresh navigation solution available.
  if (myGNSS.getPVT())
  {
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
    Serial.println(F(" (mm)"));
  }

  // Calling getNAVPOSECEF returns true if there actually is a fresh position solution available.
  if (myGNSS.getNAVPOSECEF())
  {
    Serial.print(F("ecefX: "));
    Serial.print((float)myGNSS.packetUBXNAVPOSECEF->data.ecefX / 100.0, 2); // convert ecefX to m

    Serial.print(F(" ecefY: "));
    Serial.print((float)myGNSS.packetUBXNAVPOSECEF->data.ecefY / 100.0, 2); // convert ecefY to m

    Serial.print(F(" ecefZ: "));
    Serial.print((float)myGNSS.packetUBXNAVPOSECEF->data.ecefZ / 100.0, 2); // convert ecefY to m
    Serial.println(F(" (m)"));

    myGNSS.flushNAVPOSECEF(); //Mark all the data as read/stale so we get fresh data next time
  }

  // Calling getNAVVELNED returns true if there actually is fresh velocity data available.
  if (myGNSS.getNAVVELNED())
  {
    Serial.print(F("velN: "));
    Serial.print((float)myGNSS.packetUBXNAVVELNED->data.velN / 100.0, 2); // convert velN to m/s

    Serial.print(F(" velE: "));
    Serial.print((float)myGNSS.packetUBXNAVVELNED->data.velE / 100.0, 2); // convert velE to m/s

    Serial.print(F(" velD: "));
    Serial.print((float)myGNSS.packetUBXNAVVELNED->data.velD / 100.0, 2); // convert velD to m/s
    Serial.println(F(" (m/s)"));

    myGNSS.flushNAVVELNED(); //Mark all the data as read/stale so we get fresh data next time
  }
}
