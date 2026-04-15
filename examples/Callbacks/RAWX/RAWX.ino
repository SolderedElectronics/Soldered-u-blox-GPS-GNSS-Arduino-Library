/**
 **************************************************
 * @file        RAWX.ino
 * @brief       Receive RXM-RAWX raw GNSS measurement data automatically using a callback.
 *
 * @details     This example configures the GNSS to produce RXM-RAWX messages at 1 Hz
 *              and registers a callback that prints the number of measurement blocks,
 *              GNSS ID, SV ID, pseudorange (m) and carrier phase (cycles) for each
 *              visible satellite. The 0x7F byte check is disabled because RAWX data
 *              can legitimately contain that value.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox ZED-F9P (or other module that supports RXM-RAWX)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Number of measurement blocks per epoch, with GNSS ID, SV ID, pseudorange
 *   and carrier phase for each satellite printed each second.
 *
 * Notes:
 * - RAWX messages produce a large amount of data; navigation rate is set to
 *   1 Hz to keep the I2C bus manageable.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GPS

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

// Callback: newRAWX will be called when new RXM RAWX data arrives
// See u-blox_structs.h for the full definition of UBX_RXMRAWX_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoRXMRAWXcallback
//        /             _____  This _must_ be UBX_RXM_RAWX_data_t
//        |            /                _____ You can use any name you like for the struct
//        |            |               /
//        |            |               |
void newRAWX(UBX_RXM_RAWX_data_t *ubxDataStruct)
{
  Serial.println();

  Serial.print(F("New RAWX data received. It contains "));
  Serial.print(ubxDataStruct->header.numMeas); // Print numMeas (Number of measurements / blocks)
  Serial.println(F(" data blocks:"));

  for (uint8_t block = 0; block < ubxDataStruct->header.numMeas; block++) // For each block
  {
    Serial.print(F("GNSS ID: "));
    if (ubxDataStruct->blocks[block].gnssId < 100) Serial.print(F(" ")); // Align the gnssId
    if (ubxDataStruct->blocks[block].gnssId < 10) Serial.print(F(" ")); // Align the gnssId
    Serial.print(ubxDataStruct->blocks[block].gnssId);
    Serial.print(F("  SV ID: "));
    if (ubxDataStruct->blocks[block].svId < 100) Serial.print(F(" ")); // Align the svId
    if (ubxDataStruct->blocks[block].svId < 10) Serial.print(F(" ")); // Align the svId
    Serial.print(ubxDataStruct->blocks[block].svId);

    if (sizeof(double) == 8) // Check if our processor supports 64-bit double
    {
      // Convert prMes from uint8_t[8] to 64-bit double
      // prMes is little-endian
      double pseudorange;
      memcpy(&pseudorange, &ubxDataStruct->blocks[block].prMes, 8);
      Serial.print(F("  PR: "));
      Serial.print(pseudorange, 3);

      // Convert cpMes from uint8_t[8] to 64-bit double
      // cpMes is little-endian
      double carrierPhase;
      memcpy(&carrierPhase, &ubxDataStruct->blocks[block].cpMes, 8);
      Serial.print(F(" m  CP: "));
      Serial.print(carrierPhase, 3);
      Serial.print(F(" cycles"));
    }
    Serial.println();
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  myGNSS.disableUBX7Fcheck(); // RAWX data can legitimately contain 0x7F, so we need to disable the "7F" check in checkUbloxI2C

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  myGNSS.setNavigationFrequency(1); //Produce one solution per second (RAWX produces a _lot_ of data!)

  myGNSS.setAutoRXMRAWXcallbackPtr(&newRAWX); // Enable automatic RXM RAWX messages with callback to newRAWX
}

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.

  Serial.print(".");
  delay(50);
}
