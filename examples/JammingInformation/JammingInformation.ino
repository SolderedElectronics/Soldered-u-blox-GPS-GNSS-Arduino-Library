/**
 **************************************************
 * @file        JammingInformation.ino
 * @brief       Enables the jamming/interference monitor and continuously
 *              reads jamming state and RF information from a u-blox module.
 *
 * @details     Reads and enables the UBX-CFG-ITFM jamming monitor if it is
 *              not already active. In the loop, UBX-MON-HW is polled for
 *              the overall jamming state, noise level, AGC count, and CW
 *              jamming indicator. UBX-MON-RF is also polled (with a 2 s
 *              timeout) for per-band RF information; this message is only
 *              available on multi-band modules such as the ZED-F9P.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module; ZED-F9P for per-band RF info
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Jamming state (unknown/ok/warning/critical), noise level, AGC monitor,
 *   and CW jamming indicator from UBX-MON-HW printed each loop. Per-band
 *   RF block data printed if the module supports UBX-MON-RF.
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

  // Enable the jamming / interference monitor
  UBX_CFG_ITFM_data_t jammingConfig; // Create storage for the jamming configuration
  if (myGNSS.getJammingConfiguration(&jammingConfig)) // Read the jamming configuration
  {
    Serial.print(F("The jamming / interference monitor is "));
    if (jammingConfig.config.bits.enable == 0) // Check if the monitor is already enabled
      Serial.print(F("not "));
    Serial.println(F("enabled"));

    if (jammingConfig.config.bits.enable == 0) // Check if the monitor is already enabled
    {
      Serial.print(F("Enabling the jamming / interference monitor: "));
      (jammingConfig.config.bits.enable = 1); // Enable the monitor
      if (myGNSS.setJammingConfiguration(&jammingConfig)) // Set the jamming configuration
        Serial.println(F("success"));
      else
        Serial.println(F("failed!"));
    }
  }

}

void loop()
{
  // Create storage to hold the hardware status
  // See the definition of UBX_MON_HW_data_t in u-blox_structs.h for more details
  UBX_MON_HW_data_t hwStatus;

  if (myGNSS.getHWstatus(&hwStatus)) // Read the hardware status
  {
    Serial.println(F("Hardware status (UBX_MON_HW):"));

    Serial.print(F("Jamming state: "));
    Serial.print(hwStatus.flags.bits.jammingState);
    if (hwStatus.flags.bits.jammingState == 0)
      Serial.println(F(" = unknown / disabled"));
    else if (hwStatus.flags.bits.jammingState == 1)
      Serial.println(F(" = ok"));
    else if (hwStatus.flags.bits.jammingState == 2)
      Serial.println(F(" = warning"));
    else // if (hwStatus.flags.bits.jammingState == 3)
      Serial.println(F(" = critical!"));

    Serial.print(F("Noise level: "));
    Serial.println(hwStatus.noisePerMS);
    
    Serial.print(F("AGC monitor: "));
    Serial.println(hwStatus.agcCnt);
    
    Serial.print(F("CW jamming indicator: "));
    Serial.println(hwStatus.jamInd);

    Serial.println();
  }

  // Create storage to hold the RF information from a ZED-F9
  // See the definition of UBX_MON_RF_data_t in u-blox_structs.h for more details
  UBX_MON_RF_data_t rfInformation;

  // Read the RF information from the ZED-F9n. Allow 2 seconds for the data to be returned. Will time out on M8 modules
  if (myGNSS.getRFinformation(&rfInformation, 2000))
  {
    Serial.print(F("The UBX_MON_RF message contains "));
    Serial.print(rfInformation.header.nBlocks); // Print how many information blocks were returned. Should be 0, 1 or 2
    Serial.println(F(" information blocks"));

    for (uint8_t block = 0; block < rfInformation.header.nBlocks; block++)
    {
      Serial.print(F("Block ID: "));
      Serial.print(rfInformation.blocks[block].blockId);
      if (rfInformation.blocks[block].blockId == 0)
        Serial.println(F(" = L1"));
      else // if (rfInformation.blocks[block].blockId == 1)
        Serial.println(F(" = L2 / L5"));
        
      Serial.print(F("Jamming state: "));
      Serial.print(rfInformation.blocks[block].flags.bits.jammingState);
      if (rfInformation.blocks[block].flags.bits.jammingState == 0)
        Serial.println(F(" = unknown / disabled"));
      else if (rfInformation.blocks[block].flags.bits.jammingState == 1)
        Serial.println(F(" = ok"));
      else if (rfInformation.blocks[block].flags.bits.jammingState == 2)
        Serial.println(F(" = warning"));
      else // if (rfInformation.blocks[block].flags.bits.jammingState == 3)
        Serial.println(F(" = critical!"));

      Serial.print(F("Noise level: "));
      Serial.println(rfInformation.blocks[block].noisePerMS);
      
      Serial.print(F("AGC monitor: "));
      Serial.println(rfInformation.blocks[block].agcCnt);
      
      Serial.print(F("CW jamming indicator: "));
      Serial.println(rfInformation.blocks[block].jamInd);      
    }

    Serial.println();
  }
}
