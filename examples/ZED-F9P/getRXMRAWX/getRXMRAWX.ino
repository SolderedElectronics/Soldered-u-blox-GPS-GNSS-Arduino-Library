/**
 **************************************************
 * @file        getRXMRAWX.ino
 * @brief       Poll UBX-RXM-RAWX raw measurement reports over I2C.
 *
 * @details     Requests RAWX measurement messages from the GNSS and prints
 *              the content to Serial. Because RAWX packets can exceed 2 KB,
 *              the sketch increases packetCfg payload size before polling.
 *              Useful for advanced GNSS analysis and logging workflows.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox GNSS module supporting UBX-RXM-RAWX (e.g. ZED-F9P)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - RXM-RAWX report information printed after each successful poll.
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

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  // Because we are polling RAWX, we need to increase the size of packetCfg.payload
  // RAWX packets can be over 2K bytes so let's allocate 3K bytes
  if (!myGNSS.setPacketCfgPayloadSize(3000))
  {
    Serial.println(F("setPacketCfgPayloadSize failed. You will not be able to poll RAWX data. Freezing."));
    while (1); // Do nothing more
  }

  while (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Retrying..."));
    delay(1000);
  }

  Serial.println(F("u-blox GNSS detected."));

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  myGNSS.setNavigationFrequency(1); //Produce one solution per second (RAWX produces a _lot_ of data!)
}

void loop()
{
  if (myGNSS.getRXMRAWX()) // Poll RAWX data
  {
    // Print the RAWX data, using a pointer to the RXM RAWX data stored in packetUBXRXMRAWX
    printRAWX(&myGNSS.packetUBXRXMRAWX->data);
  }
}

void printRAWX(UBX_RXM_RAWX_data_t *ubxDataStruct)
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
