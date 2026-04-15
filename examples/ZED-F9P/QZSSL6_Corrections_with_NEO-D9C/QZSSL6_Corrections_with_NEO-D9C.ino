/**
 **************************************************
 * @file        QZSSL6_Corrections_with_NEO-D9C.ino
 * @brief       Forward QZSS-L6 corrections from a NEO-D9C to a ZED-F9x via UART.
 *
 * @details     Configures a NEO-D9C QZSS-L6 correction data receiver over I2C
 *              and routes its UBX-RXM-QZSSL6 messages via UART to the ZED-F9P,
 *              avoiding an I2C address collision (both may default to 0x42).
 *              The Teensy communicates with the NEO-D9C over I2C on address 0x42
 *              and with the ZED-F9P over Serial1 (UART1). The NEO-D9C UART1
 *              connects directly to ZED-F9P UART2. Early NEO-D9C hardware may
 *              not support UBX-CFG-PRT; use assumeSuccess=true for .begin().
 *
 * Requirements:
 * - Board:      Teensy 4.0 (or compatible board with separate I2C and UART)
 * - Hardware:   u-blox ZED-F9P (via UART) and NEO-D9C (via I2C)
 *
 * How to use:
 * 1) Wire the Teensy, NEO-D9C, and ZED-F9P as described in the code comments.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - NEO-D9C configuration status messages.
 * - QZSS-L6 message receipt notifications and forwarding confirmations.
 * - ZED-F9P position and fix type from NAV PVT once corrections are active.
 *
 * Notes:
 * - Early NEO-D9C modules use I2C address 0x42 instead of the expected 0x43.
 *   This is why UART is used for the ZED-F9P connection in this example.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS; // ZED-F9x
Soldered_GNSS myQZSS; // NEO-D9C

#define OK(ok) (ok ? F("  ->  OK") : F("  ->  ERROR!")) // Convert uint8_t into OK/ERROR

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printRXMQZSSL6 will be called when new QZSS-L6 data arrives
// See u-blox_structs.h for the full definition of UBX_RXM_QZSSL6_message_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setRXMQZSSL6messageCallbackPtr
//        /                  _____  This _must_ be UBX_RXM_QZSSL6_message_data_t
//        |                 /                        _____ You can use any name you like for the struct
//        |                 |                       /
//        |                 |                       |
void printRXMQZSSL6(UBX_RXM_QZSSL6_message_data_t *qzssL6Data)
{
  Serial.println(F("New QZSS-L6 data received:"));

  Serial.print(F("Message version:      "));
  Serial.println(qzssL6Data->payload[0]);
  
  Serial.print(F("Satellite Identifier: "));
  Serial.println(qzssL6Data->payload[1]);
  
  Serial.print(F("Carrier / Noise:      "));
  double cno = (0.00390625 * ((double)qzssL6Data->payload[2])) + ((double)qzssL6Data->payload[3]);
  Serial.println(cno, 1);
  
  Serial.print(F("Bit Errors Corrected: "));
  Serial.println(qzssL6Data->payload[9]);
  
  uint16_t chInfo = (((uint16_t)qzssL6Data->payload[11]) << 8) | qzssL6Data->payload[10];
  uint16_t errStatus = ((chInfo >> 12) & 0x3);
  Serial.print(F("Receiver Channel:     "));
  Serial.println((chInfo >> 8) & 0x3);
  Serial.print(F("Message Name:         L6"));
  Serial.println(((chInfo >> 10) & 0x1) == 0 ? F("D") : F("E"));
  Serial.print(F("Error Status:         "));
  if (errStatus == 1)
    Serial.println("error-free");
  else if (errStatus == 2)
    Serial.println("erroneous");
  else
    Serial.println("unknown");
  Serial.print(F("Channel Name:         "));
  Serial.println(((chInfo >> 14) & 0x3) == 0 ? F("A") : F("B"));
  
  Serial.println();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printPVTdata will be called when new NAV PVT data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_PVT_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoPVTcallbackPtr
//        /                  _____  This _must_ be UBX_NAV_PVT_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printPVTdata(UBX_NAV_PVT_data_t *ubxDataStruct)
{
  double latitude = ubxDataStruct->lat; // Print the latitude
  Serial.print(F("Lat: "));
  Serial.print(latitude / 10000000.0, 7);

  double longitude = ubxDataStruct->lon; // Print the longitude
  Serial.print(F("  Long: "));
  Serial.print(longitude / 10000000.0, 7);

  double altitude = ubxDataStruct->hMSL; // Print the height above mean sea level
  Serial.print(F("  Height: "));
  Serial.print(altitude / 1000.0, 3);

  uint8_t fixType = ubxDataStruct->fixType; // Print the fix type
  Serial.print(F("  Fix: "));
  Serial.print(fixType);
  if (fixType == 0)
    Serial.print(F(" (None)"));
  else if (fixType == 1)
    Serial.print(F(" (Dead Reckoning)"));
  else if (fixType == 2)
    Serial.print(F(" (2D)"));
  else if (fixType == 3)
    Serial.print(F(" (3D)"));
  else if (fixType == 3)
    Serial.print(F(" (GNSS + Dead Reckoning)"));
  else if (fixType == 5)
    Serial.print(F(" (Time Only)"));
  else
    Serial.print(F(" (UNKNOWN)"));

  uint8_t carrSoln = ubxDataStruct->flags.bits.carrSoln; // Print the carrier solution
  Serial.print(F("  Carrier Solution: "));
  Serial.print(carrSoln);
  if (carrSoln == 0)
    Serial.print(F(" (None)"));
  else if (carrSoln == 1)
    Serial.print(F(" (Floating)"));
  else if (carrSoln == 2)
    Serial.print(F(" (Fixed)"));
  else
    Serial.print(F(" (UNKNOWN)"));

  uint32_t hAcc = ubxDataStruct->hAcc; // Print the horizontal accuracy estimate
  Serial.print(F("  Horizontal Accuracy Estimate: "));
  Serial.print(hAcc);
  Serial.print(F(" (mm)"));

  Serial.println();    
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printRXMCOR will be called when new RXM COR data arrives
// See u-blox_structs.h for the full definition of UBX_RXM_COR_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setRXMCORcallbackPtr
//        /                  _____  This _must_ be UBX_RXM_COR_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printRXMCOR(UBX_RXM_COR_data_t *ubxDataStruct)
{
  Serial.print(F("UBX-RXM-COR:  ebno: "));
  Serial.print((double)ubxDataStruct->ebno / 8, 3); //Convert to dB

  Serial.print(F("  protocol: "));
  if (ubxDataStruct->statusInfo.bits.protocol == 1)
    Serial.print(F("RTCM3"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 2)
    Serial.print(F("SPARTN"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 29)
    Serial.print(F("PMP (SPARTN)"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 30)
    Serial.print(F("QZSSL6"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  errStatus: "));
  if (ubxDataStruct->statusInfo.bits.errStatus == 1)
    Serial.print(F("Error-free"));
  else if (ubxDataStruct->statusInfo.bits.errStatus == 2)
    Serial.print(F("Erroneous"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgUsed: "));
  if (ubxDataStruct->statusInfo.bits.msgUsed == 1)
    Serial.print(F("Not used"));
  else if (ubxDataStruct->statusInfo.bits.msgUsed == 2)
    Serial.print(F("Used"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgEncrypted: "));
  if (ubxDataStruct->statusInfo.bits.msgEncrypted == 1)
    Serial.print(F("Not encrypted"));
  else if (ubxDataStruct->statusInfo.bits.msgEncrypted == 2)
    Serial.print(F("Encrypted"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgDecrypted: "));
  if (ubxDataStruct->statusInfo.bits.msgDecrypted == 1)
    Serial.print(F("Not decrypted"));
  else if (ubxDataStruct->statusInfo.bits.msgDecrypted == 2)
    Serial.print(F("Successfully decrypted"));
  else
    Serial.print(F("Unknown"));

  Serial.println();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  Serial.begin(115200);
  Serial.println(F("NEO-D9C Corrections"));

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Begin and configure the ZED-F9x

  Serial1.begin(38400); // The ZED-F9P is connected via Serial1 to UART1

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  while (myGNSS.begin(Serial1) == false) //Connect to the u-blox module using Serial1 and UART1
  {
    Serial.println(F("u-blox GNSS module not detected. Please check wiring."));
    delay(2000);
  }
  Serial.println(F("u-blox GNSS module connected"));

  uint8_t ok = myGNSS.setUART1Output(COM_TYPE_UBX); //Turn off NMEA noise
  
  if (ok) ok = myGNSS.setPortInput(COM_PORT_UART2, COM_TYPE_UBX | COM_TYPE_RTCM3 | COM_TYPE_SPARTN); //Be sure SPARTN input is enabled on UART2

  if (ok) ok = myGNSS.setDGNSSConfiguration(SFE_UBLOX_DGNSS_MODE_FIXED); // Set the differential mode - ambiguities are fixed whenever possible

  if (ok) ok = myGNSS.setNavigationFrequency(1); //Set output in Hz.
  
  if (ok) ok = myGNSS.setVal8(UBLOX_CFG_MSGOUT_UBX_RXM_COR_UART1, 1); // Enable UBX-RXM-COR messages on UART1
  
  //if (ok) ok = myGNSS.saveConfiguration(VAL_CFG_SUBSEC_IOPORT | VAL_CFG_SUBSEC_MSGCONF); //Optional: Save the ioPort and message settings to NVM
  
  Serial.print(F("GNSS: configuration "));
  Serial.println(OK(ok));

  myGNSS.setAutoPVTcallbackPtr(&printPVTdata); // Enable automatic NAV PVT messages with callback to printPVTdata so we can watch the carrier solution go to fixed

  myGNSS.setRXMCORcallbackPtr(&printRXMCOR); // Print the contents of UBX-RXM-COR messages so we can check if the QZSS-L6 data is being decrypted successfully

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Begin and configure the NEO-D9C QZSS-L6 receiver

  Wire.begin(); //Start I2C

  //myQZSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  // For the initial NEO-D9C's: connect using address 0x42; set the assumeSuccess parameter to true
  while (myQZSS.begin(Wire, 0x42, 1100, true) == false)
  // For newer NEO-D9C's: use address 0x43; leave assumeSuccess set to false (default)
  //while (myQZSS.begin(Wire, 0x43) == false)
  {
    Serial.println(F("u-blox NEO-D9C not detected at selected I2C address. Please check wiring and I2C address."));
    delay(2000);
  }
  Serial.println(F("u-blox NEO-D9C connected"));

  ok = myQZSS.setVal8(UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_I2C,   1);     // Output QZSS-L6 message on the I2C port 

  Serial.print(F("QZSS-L6: I2C configuration "));
  Serial.println(OK(ok));

  if (ok) ok = myQZSS.setVal8(UBLOX_CFG_UART1OUTPROT_UBX,            1);     // Enable UBX output on UART1
  if (ok) ok = myQZSS.setVal8(UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_UART1, 1);     // Output QZSS-L6 message on UART1
  if (ok) ok = myQZSS.setVal32(UBLOX_CFG_UART1_BAUDRATE,            38400); // Match UART1 baudrate with ZED

  Serial.print(F("QZSS-L6: UART1 configuration "));
  Serial.println(OK(ok));

  if (ok) ok = myQZSS.setVal8(UBLOX_CFG_UART2OUTPROT_UBX,            1);     // Enable UBX output on UART2
  if (ok) ok = myQZSS.setVal8(UBLOX_CFG_MSGOUT_UBX_RXM_QZSSL6_UART2, 1);     // Output QZSS-L6 message on UART2
  if (ok) ok = myQZSS.setVal32(UBLOX_CFG_UART2_BAUDRATE,            38400); // Match UART2 baudrate with ZED

  Serial.print(F("QZSS-L6: UART2 configuration "));
  Serial.println(OK(ok));

  myQZSS.setRXMQZSSL6messageCallbackPtr(&printRXMQZSSL6); // Call printRXMQZSSL6 when new QZSS-L6 data arrives
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new GNSS data and process it.
  myGNSS.checkCallbacks(); // Check if any GNSS callbacks are waiting to be processed.

  myQZSS.checkUblox(); // Check for the arrival of new QZSS-L6 data and process it.
  myQZSS.checkCallbacks(); // Check if any LBand callbacks are waiting to be processed.
}
