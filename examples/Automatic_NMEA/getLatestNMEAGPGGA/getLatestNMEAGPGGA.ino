/**
 **************************************************
 * @file        getLatestNMEAGPGGA.ino
 * @brief       Poll the latest GPGGA/GNGGA NMEA sentence using getLatestNMEAGPGGA (non-blocking).
 *
 * @details     This example configures the GNSS module to output only GGA sentences
 *              over I2C, forces the Main Talker ID to "GP" and enables High Precision
 *              Mode. In the loop it calls getLatestNMEAGPGGA and getLatestNMEAGNGGA
 *              to retrieve the most recent sentence without blocking. The functions
 *              return 0 (no data), 1 (stale) or 2 (fresh).
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C and sufficient RAM
 *               (not compatible with Arduino Uno)
 * - Hardware:   Any u-blox GNSS module (e.g. ZED-F9P, NEO-M8P-2)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - "No GPGGA data available", "stale", or latest GPGGA sentence with length,
 *   repeated every 250 ms.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h> //Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS
Soldered_GNSS myGNSS;

void setup()
{

  Serial.begin(115200);
  Serial.println(F("SparkFun u-blox GNSS Example"));

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable debug messages on Serial

  if (myGNSS.begin() == false)
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //Disable or enable various NMEA sentences over the I2C interface
  myGNSS.setI2COutput(COM_TYPE_NMEA | COM_TYPE_UBX); // Turn on both UBX and NMEA sentences on I2C. (Turn off RTCM and SPARTN)
  myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_I2C); // Several of these are on by default on ublox board so let's disable them
  myGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_I2C);
  myGNSS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_I2C);
  myGNSS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_I2C);
  myGNSS.disableNMEAMessage(UBX_NMEA_VTG, COM_PORT_I2C);
  myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_I2C); // Leave only GGA enabled at current navigation rate

  // Set the Main Talker ID to "GP". The NMEA GGA messages will be GPGGA instead of GNGGA
  myGNSS.setMainTalkerID(SFE_UBLOX_MAIN_TALKER_ID_GP);
  //myGNSS.setMainTalkerID(SFE_UBLOX_MAIN_TALKER_ID_DEFAULT); // Uncomment this line to restore the default main talker ID

  myGNSS.setHighPrecisionMode(true); // Enable High Precision Mode - include extra decimal places in the GGA messages

  //myGNSS.saveConfiguration(VAL_CFG_SUBSEC_IOPORT | VAL_CFG_SUBSEC_MSGCONF); //Optional: Save only the ioPort and message settings to NVM

  Serial.println(F("Messages configured"));

  //myGNSS.setNMEAOutputPort(Serial); // Uncomment this line to echo all NMEA data to Serial for debugging
}

void loop()
{
  // getLatestNMEAGPGGA calls checkUblox for us. We don't need to do it here

  NMEA_GGA_data_t data; // Storage for the GPGGA data
  uint8_t result = myGNSS.getLatestNMEAGPGGA(&data); // Get the latest GPGGA data (if any)
  
  if (result == 0)
  {
    Serial.println(F("No GPGGA data available"));
  }
  else if (result == 1)
  {
    Serial.println(F("GPGGA data is available but is stale"));
  }
  else // if (result == 2)
  {
    // Data contains .length and .nmea
    Serial.print(F("Latest GPGGA: Length: "));
    Serial.print(data.length);
    Serial.print(F("\tData: "));
    Serial.println((const char *)data.nmea); // .nmea is printable (NULL-terminated)
  }

  result = myGNSS.getLatestNMEAGNGGA(&data); // Get the latest GNGGA data (if any)
  
  if (result == 0)
  {
    Serial.println(F("No GNGGA data available"));
  }
  else if (result == 1)
  {
    Serial.println(F("GNGGA data is available but is stale"));
  }
  else // if (result == 2)
  {
    // Data contains .length and .nmea
    Serial.print(F("Latest GNGGA: Length: "));
    Serial.print(data.length);
    Serial.print(F("\tData: "));
    Serial.println((const char *)data.nmea); // .nmea is printable (NULL-terminated)
  }

  delay(250);
}
