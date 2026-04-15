/**
 **************************************************
 * @file        EnableNMEASentences.ino
 * @brief       Selectively enables and disables NMEA sentences on UART1 of
 *              a u-blox GNSS module via I2C configuration.
 *
 * @details     Uses I2C to configure the module so that only GPGGA and
 *              GPVTG sentences are output on UART1 at 57600 bps. All other
 *              standard NMEA sentences (GLL, GSA, GSV, RMC) are disabled.
 *              The UBX-NAV-PVT message is also disabled on UART1 to avoid
 *              interference. No NMEA data is visible in the Arduino Serial
 *              Monitor — connect a USB-to-Serial adapter to UART1 to see
 *              the output.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 * - Extra:      USB-to-Serial adapter connected to UART1 on the u-blox
 *               module (to observe NMEA output at 57600 bps)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud to see configuration status.
 * 4) Connect a USB-to-Serial adapter to UART1 on the module and open a
 *    terminal at 57600 bps to observe the GPGGA and GPVTG sentences.
 *
 * Expected output:
 * - Arduino Serial Monitor: confirmation that NMEA sentences are configured.
 * - UART1 terminal: GPGGA and GPVTG sentences at 57600 bps.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/
#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h> //Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS
Soldered_GNSS myGNSS;

unsigned long lastGNSSsend = 0;

void setup()
{
  Serial.begin(115200); // Serial debug output over USB visible from Arduino IDE
  Serial.println("Example showing how to enable/disable certain NMEA sentences");

  Wire.begin();

  if (myGNSS.begin() == false)
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //Disable or enable various NMEA sentences over the UART1 interface
  myGNSS.disableNMEAMessage(UBX_NMEA_GLL, COM_PORT_UART1); //Several of these are on by default on ublox board so let's disable them
  myGNSS.disableNMEAMessage(UBX_NMEA_GSA, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_GSV, COM_PORT_UART1);
  myGNSS.disableNMEAMessage(UBX_NMEA_RMC, COM_PORT_UART1);
  myGNSS.enableNMEAMessage(UBX_NMEA_GGA, COM_PORT_UART1); //Only leaving GGA & VTG enabled at current navigation rate
  myGNSS.enableNMEAMessage(UBX_NMEA_VTG, COM_PORT_UART1);

  //Here's the advanced configure method
  //Some of the other examples in this library enable the PVT message so let's disable it
  myGNSS.configureMessage(UBX_CLASS_NAV, UBX_NAV_PVT, COM_PORT_UART1, 0); //Message Class, ID, and port we want to configure, sendRate of 0 (disable).

  myGNSS.setUART1Output(COM_TYPE_NMEA); //Turn off UBX and RTCM sentences on the UART1 interface

  myGNSS.setSerialRate(57600); //Set UART1 to 57600bps.

  //myGNSS.saveConfiguration(); //Optional: Save these settings to NVM

  Serial.println(F("Messages configured. NMEA now being output over the UART1 port on the u-blox module at 57600bps."));
}

void loop()
{
  if (millis() - lastGNSSsend > 200)
  {
    myGNSS.checkUblox(); //See if new data is available, but we don't want to get NMEA here. Go check UART1.
    lastGNSSsend = millis();
  }
}
