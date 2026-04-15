/**
 **************************************************
 * @file        NMEA_GGA_VTG_RMC_ZDA_Callbacks.ino
 * @brief       Receive GGA, VTG, RMC and ZDA NMEA sentences autonomously using callbacks.
 *
 * @details     This example configures the GNSS module to output GGA, VTG, RMC and
 *              ZDA sentences over I2C and registers individual callbacks for both the
 *              GP (single-GNSS) and GN (multi-constellation) Talker ID variants of
 *              each sentence. The Main Talker ID is forced to "GP" and High Precision
 *              Mode is enabled for extra decimal places in GGA.
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
 * - GPGGA, GPVTG, GPRMC and GPZDA (or GN variants) sentence length and raw
 *   NMEA string printed each navigation cycle.
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

// Callback: printGPGGA will be called when new GPGGA NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_GGA_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGPGGAcallback
//        /               _____  This _must_ be NMEA_GGA_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGPGGA(NMEA_GGA_data_t *nmeaData)
{
    Serial.print(F("\r\nGPGGA: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

// Callback: printGNGGA will be called if new GNGGA NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_GGA_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGNGGAcallback
//        /               _____  This _must_ be NMEA_GGA_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGNGGA(NMEA_GGA_data_t *nmeaData)
{
    Serial.print(F("\r\nGNGGA: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

// Callback: printGPVTG will be called when new GPVTG NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_VTG_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGPVTGcallback
//        /               _____  This _must_ be NMEA_VTG_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGPVTG(NMEA_VTG_data_t *nmeaData)
{
    Serial.print(F("\r\nGPVTG: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

// Callback: printGNVTG will be called if new GNVTG NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_VTG_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGNVTGcallback
//        /               _____  This _must_ be NMEA_VTG_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGNVTG(NMEA_VTG_data_t *nmeaData)
{
    Serial.print(F("\r\nGNVTG: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

// Callback: printGPRMC will be called when new GPRMC NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_RMC_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGPRMCcallback
//        /               _____  This _must_ be NMEA_RMC_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGPRMC(NMEA_RMC_data_t *nmeaData)
{
    Serial.print(F("\r\nGPRMC: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

// Callback: printGNRMC will be called if new GNRMC NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_RMC_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGNRMCcallback
//        /               _____  This _must_ be NMEA_RMC_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGNRMC(NMEA_RMC_data_t *nmeaData)
{
    Serial.print(F("\r\nGNRMC: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

// Callback: printGPZDA will be called when new GPZDA NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_ZDA_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGPZDAcallback
//        /               _____  This _must_ be NMEA_ZDA_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGPZDA(NMEA_ZDA_data_t *nmeaData)
{
    Serial.print(F("\r\nGPZDA: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

// Callback: printGNZDA will be called if new GNZDA NMEA data arrives
// See u-blox_structs.h for the full definition of NMEA_ZDA_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setNMEAGNZDAcallback
//        /               _____  This _must_ be NMEA_ZDA_data_t
//        |              /           _____ You can use any name you like for the struct
//        |              |          /
//        |              |          |
void printGNZDA(NMEA_ZDA_data_t *nmeaData)
{
    Serial.print(F("\r\nGNZDA: Length: "));
    Serial.print(nmeaData->length);
    Serial.print(F("\tData: "));
    Serial.print((const char *)nmeaData->nmea); // .nmea is printable (NULL-terminated) and already has \r\n on the end
}

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

  // Disable or enable various NMEA sentences over the I2C interface
  myGNSS.setI2COutput(COM_TYPE_NMEA | COM_TYPE_UBX); // Turn on both UBX and NMEA sentences on I2C. (Turn off RTCM and SPARTN)
  myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_I2C);
  myGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_I2C);
  myGNSS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_I2C);
  myGNSS.enableNMEAMessage(UBX_NMEA_RMC, COM_PORT_I2C);
  myGNSS.enableNMEAMessage(UBX_NMEA_VTG, COM_PORT_I2C);
  myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_I2C);
  myGNSS.enableNMEAMessage(UBX_NMEA_ZDA, COM_PORT_I2C);

  // Set the Main Talker ID to "GP". The NMEA GGA messages will be GPGGA instead of GNGGA
  myGNSS.setMainTalkerID(SFE_UBLOX_MAIN_TALKER_ID_GP);
  //myGNSS.setMainTalkerID(SFE_UBLOX_MAIN_TALKER_ID_DEFAULT); // Uncomment this line to restore the default main talker ID

  myGNSS.setHighPrecisionMode(true); // Enable High Precision Mode - include extra decimal places in the GGA messages

  //myGNSS.saveConfiguration(VAL_CFG_SUBSEC_IOPORT | VAL_CFG_SUBSEC_MSGCONF); //Optional: Save only the ioPort and message settings to NVM

  Serial.println(F("Messages configured"));

  //myGNSS.setNMEAOutputPort(Serial); // Uncomment this line to echo all NMEA data to Serial for debugging

  // Set up the callback for GPGGA
  myGNSS.setNMEAGPGGAcallbackPtr(&printGPGGA);

  // Set up the callback for GNGGA
  myGNSS.setNMEAGNGGAcallbackPtr(&printGNGGA);

  // Set up the callback for GPVTG
  myGNSS.setNMEAGPVTGcallbackPtr(&printGPVTG);

  // Set up the callback for GNVTG
  myGNSS.setNMEAGNVTGcallbackPtr(&printGNVTG);

  // Set up the callback for GPRMC
  myGNSS.setNMEAGPRMCcallbackPtr(&printGPRMC);

  // Set up the callback for GNRMC
  myGNSS.setNMEAGNRMCcallbackPtr(&printGNRMC);

  // Set up the callback for GPZDA
  myGNSS.setNMEAGPZDAcallbackPtr(&printGPZDA);

  // Set up the callback for GNZDA
  myGNSS.setNMEAGNZDAcallbackPtr(&printGNZDA);
}

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.

  Serial.print(".");
  delay(50);
}
