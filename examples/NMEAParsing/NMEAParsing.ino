/**
 **************************************************
 * @file        NMEAParsing.ino
 * @brief       Reads NMEA sentences from a u-blox module over I2C and
 *              parses them with the MicroNMEA library.
 *
 * @details     Enables combined UBX + NMEA output on the I2C port and uses
 *              the processNMEA() callback to feed each incoming character to
 *              a MicroNMEA object. The NMEA filter is set to pass only GGA
 *              sentences, reducing overhead. Once a valid fix is parsed,
 *              latitude and longitude in decimal degrees are printed.
 *              Requires the MicroNMEA library (install via Library Manager).
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module (e.g. SAM-M8Q, NEO-M8P)
 * - Extra:      MicroNMEA library
 *
 * How to use:
 * 1) Install the MicroNMEA library via the Arduino Library Manager.
 * 2) Connect the u-blox module to your board via I2C.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud and go outside.
 *
 * Expected output:
 * - "Waiting for fresh data" until a valid fix, then latitude and longitude
 *   in decimal degrees printed every 250 ms.
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

#include <MicroNMEA.h> //http://librarymanager/All#MicroNMEA
char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  if (myGNSS.begin() == false)
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX | COM_TYPE_NMEA); //Set the I2C port to output both NMEA and UBX messages
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  myGNSS.setProcessNMEAMask(SFE_UBLOX_FILTER_NMEA_ALL); // Make sure the library is passing all NMEA messages to processNMEA

  myGNSS.setProcessNMEAMask(SFE_UBLOX_FILTER_NMEA_GGA); // Or, we can be kind to MicroNMEA and _only_ pass the GGA messages to it
}

void loop()
{
  myGNSS.checkUblox(); //See if new data is available. Process bytes as they come in.

  if(nmea.isValid() == true)
  {
    long latitude_mdeg = nmea.getLatitude();
    long longitude_mdeg = nmea.getLongitude();

    Serial.print("Latitude (deg): ");
    Serial.println(latitude_mdeg / 1000000., 6);
    Serial.print("Longitude (deg): ");
    Serial.println(longitude_mdeg / 1000000., 6);

    nmea.clear(); // Clear the MicroNMEA storage to make sure we are getting fresh data
  }
  else
  {
    Serial.println("Waiting for fresh data");
  }

  delay(250); //Don't pound too hard on the I2C bus
}

//This function gets called from the SparkFun u-blox Arduino Library
//As each NMEA character comes in you can specify what to do with it
//Useful for passing to other libraries like tinyGPS, MicroNMEA, or even
//a buffer, radio, etc.
void SFE_UBLOX_GNSS::processNMEA(char incoming)
{
  //Take the incoming char from the u-blox I2C port and pass it on to the MicroNMEA lib
  //for sentence cracking
  nmea.process(incoming);
}
