/**
 **************************************************
 * @file        SetVal.ino
 * @brief       Write a configuration value to a u-blox module using VALSET.
 *
 * @details     Demonstrates the VALSET command structure introduced in UBX
 *              protocol version 23 by changing a configuration key (such as
 *              the module I2C address) and confirming the update. Shows the
 *              recommended replacement for deprecated UBX-CFG setters.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox GNSS module with UBX protocol >= v23 (e.g. ZED-F9P)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - The selected key value before/after update and command status messages.
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

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("u-blox getVal example");

  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //myGNSS.enableDebugging(); //Enable debug messages over Serial (default)
  //myGNSS.enableDebugging(SerialUSB); //Enable debug messages over Serial USB

  bool setValueSuccess;

  //These key values are hard coded and defined in u-blox_config_keys.h.
  //You can obtain them from the ZED-F9P interface description doc
  //or from u-center's Messages->CFG->VALSET window. Keys must be 32-bit.
  //setValueSuccess = myGNSS.setVal(UBLOX_CFG_NMEA_HIGHPREC, 0); //Enable high precision NMEA
  //setValueSuccess = myGNSS.setVal(UBLOX_CFG_RATE_MEAS, 1000); //Set measurement rate to 100ms (10Hz update rate)
  setValueSuccess = myGNSS.setVal(UBLOX_CFG_RATE_MEAS, 1000); //Set measurement rate to 1000ms (1Hz update rate)

  //Below is the original way we enabled the RTCM message on the I2C port. After that, we show how to do the same
  //but with setVal().
  //Original: myGNSS.enableRTCMmessage(UBX_RTCM_1005, COM_PORT_I2C, 1); //Enable message 1005 to output through I2C port, message every second
  //setValueSuccess = myGNSS.setVal(UBLOX_CFG_MSGOUT_RTCM_3X_TYPE1005_I2C, 1); //Set output rate of msg 1005 over the I2C port to once per second

  if (setValueSuccess == true)
  {
    Serial.println("Value was successfully set");
  }
  else
    Serial.println("Value set failed");
}

void loop()
{
}
