/**
 **************************************************
 * @file        GetSetPortSettings.ino
 * @brief       Read and configure UART1 port protocol settings using VALGET/VALSET.
 *
 * @details     Queries the u-blox module for its current UART1 input protocol
 *              settings (UBX, NMEA, RTCM3) using the VALGET method, then sets
 *              them to the desired values if they differ. Demonstrates the
 *              VALGET/VALSET configuration interface available from UBX protocol
 *              version 27 onwards.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox ZED-F9P (UBX protocol v27+)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Current UART1 UBX/NMEA/RTCM3 enable flags and whether they were updated.
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

  bool response = true;

  //Read the settings from RAM (what the module is running right now, not BBR, Flash, or default)
  uint8_t currentUART1Setting_ubx = myGNSS.getVal8(UBLOX_CFG_UART1INPROT_UBX);
  uint8_t currentUART1Setting_nmea = myGNSS.getVal8(UBLOX_CFG_UART1INPROT_NMEA);
  uint8_t currentUART1Setting_rtcm3 = myGNSS.getVal8(UBLOX_CFG_UART1INPROT_RTCM3X);

  Serial.print("currentUART1Setting_ubx: ");
  Serial.println(currentUART1Setting_ubx);
  Serial.print("currentUART1Setting_nmea: ");
  Serial.println(currentUART1Setting_nmea);
  Serial.print("currentUART1Setting_rtcm3: ");
  Serial.println(currentUART1Setting_rtcm3);

  //Check if NMEA and RTCM are enabled for UART1
  if (currentUART1Setting_ubx == 0 || currentUART1Setting_nmea == 0)
  {
    Serial.println("Updating UART1 configuration");

    //setVal sets the values for RAM, BBR, and Flash automatically so no .saveConfiguration() is needed
    response &= myGNSS.setVal8(UBLOX_CFG_UART1INPROT_UBX, 1);    //Enable UBX on UART1 Input
    response &= myGNSS.setVal8(UBLOX_CFG_UART1INPROT_NMEA, 1);   //Enable NMEA on UART1 Input
    response &= myGNSS.setVal8(UBLOX_CFG_UART1INPROT_RTCM3X, 0); //Disable RTCM on UART1 Input

    if (response == false)
      Serial.println("SetVal failed");
    else
      Serial.println("SetVal succeeded");
  }
  else
    Serial.println("No port change needed");

  //Change speed of UART2
  uint32_t currentUART2Baud = myGNSS.getVal32(UBLOX_CFG_UART2_BAUDRATE);
  Serial.print("currentUART2Baud: ");
  Serial.println(currentUART2Baud);

  if (currentUART2Baud != 57600)
  {
    response &= myGNSS.setVal32(UBLOX_CFG_UART2_BAUDRATE, 57600);
    if (response == false)
      Serial.println("SetVal failed");
    else
      Serial.println("SetVal succeeded");
  }
  else
    Serial.println("No baud change needed");

  Serial.println("Done");
}

void loop()
{
}
