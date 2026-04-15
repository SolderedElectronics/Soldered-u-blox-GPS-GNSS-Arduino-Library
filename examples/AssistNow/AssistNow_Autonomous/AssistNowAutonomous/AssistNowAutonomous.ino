/**
 **************************************************
 * @file        AssistNowAutonomous.ino
 * @brief       Enable and monitor AssistNow Autonomous data collection using callbacks.
 *
 * @details     This example shows how to enable AssistNow Autonomous data collection
 *              on the module. A callback monitors data availability (aopAvail flag)
 *              for each satellite via UBX-NAV-SAT. A second callback prints the
 *              AOPSTATUS status, and a third callback prints NAV-PVT data so you
 *              can observe how quickly a fix is obtained after the module has built
 *              up its Autonomous database.
 *
 *              If your GNSS board has battery-backed RAM, you can wait until
 *              Autonomous data is available for a few satellites, power-cycle the
 *              board, and watch the dramatically faster time-to-first-fix.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C and sufficient RAM
 * - Hardware:   u-blox GNSS module (not ZED-F9P — that module supports
 *               AssistNow Online only)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - UBX-NAV-SAT SV count and number of SVs with Autonomous data available.
 * - AOPSTATUS status value printed each navigation cycle.
 * - Latitude, longitude, altitude and fix type printed each second.
 *
 * Notes:
 * - This example requires a board with plenty of RAM; UBX-NAV-SAT data
 *   can occupy several kBytes.
 * - Not compatible with the ZED-F9P (AssistNow Online only on that module).
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printSATdata will be called when new NAV SAT data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_SAT_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoNAVSATcallback
//        /                  _____  This _must_ be UBX_NAV_SAT_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printSATdata(UBX_NAV_SAT_data_t *ubxDataStruct)
{
  //Serial.println();
  
  Serial.print(F("UBX-NAV-SAT contains data for "));
  Serial.print(ubxDataStruct->header.numSvs);
  if (ubxDataStruct->header.numSvs == 1)
    Serial.println(F(" SV"));
  else
    Serial.println(F(" SVs"));

  uint16_t numAopAvail = 0; // Count how many SVs have AssistNow Autonomous data available
    
  for (uint16_t block = 0; block < ubxDataStruct->header.numSvs; block++) // For each SV
  {
    if (ubxDataStruct->blocks[block].flags.bits.aopAvail == 1) // If the aopAvail bit is set
      numAopAvail++; // Increment the number of SVs
  }

  Serial.print(F("AssistNow Autonomous data is available for "));
  Serial.print(numAopAvail);
  if (numAopAvail == 1)
    Serial.println(F(" SV"));
  else
    Serial.println(F(" SVs"));
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printAOPstatus will be called when new NAV AOPSTATUS data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_AOPSTATUS_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoNAVAOPSTATUScallback
//        /                  _____  This _must_ be UBX_NAV_AOPSTATUS_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printAOPstatus(UBX_NAV_AOPSTATUS_data_t *ubxDataStruct)
{
  //Serial.println();
  
  Serial.print(F("AOPSTATUS status is "));
  Serial.println(ubxDataStruct->status);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printPVTdata will be called when new NAV PVT data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_PVT_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoPVTcallback
//        /                  _____  This _must_ be UBX_NAV_PVT_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printPVTdata(UBX_NAV_PVT_data_t *ubxDataStruct)
{
  // Print the UBX-NAV-PVT data so we can see how quickly the fixType goes to 3D
  
  Serial.println();

  long latitude = ubxDataStruct->lat; // Print the latitude
  Serial.print(F("Lat: "));
  Serial.print(latitude);

  long longitude = ubxDataStruct->lon; // Print the longitude
  Serial.print(F(" Long: "));
  Serial.print(longitude);
  Serial.print(F(" (degrees * 10^-7)"));

  long altitude = ubxDataStruct->hMSL; // Print the height above mean sea level
  Serial.print(F(" Alt: "));
  Serial.print(altitude);
  Serial.print(F(" (mm)"));

  byte fixType = ubxDataStruct->fixType; // Print the fix type
  Serial.print(F(" Fix: "));
  if(fixType == 0) Serial.print(F("No fix"));
  else if(fixType == 1) Serial.print(F("Dead reckoning"));
  else if(fixType == 2) Serial.print(F("2D"));
  else if(fixType == 3) Serial.print(F("3D"));
  else if(fixType == 4) Serial.print(F("GNSS + Dead reckoning"));
  else if(fixType == 5) Serial.print(F("Time only"));

  Serial.println();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  delay(1000);

  Serial.begin(115200);
  Serial.println(F("AssistNow Example"));

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Start I2C. Connect to the GNSS.

  Wire.begin(); //Start I2C

  //myGNSS.enableDebugging(Serial, true); // Uncomment this line to see the 'major' debug messages on Serial

  if (myGNSS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("u-blox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
  Serial.println(F("u-blox module connected"));

  myGNSS.setI2COutput(COM_TYPE_UBX); //Turn off NMEA noise
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Enable AssistNow Autonomous data collection.

  if (myGNSS.setAopCfg(1) == true)
  {
    Serial.println(F("aopCfg enabled"));
  }
  else
  {
    Serial.println(F("Could not enable aopCfg. Please check wiring. Freezing."));
    while (1);
  }

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Enable automatic UBX-NAV-SAT and UBX-NAV-AOPSTATUS messages and set up the callbacks

  myGNSS.setNavigationFrequency(1); //Produce one solution per second

  myGNSS.setAutoNAVSATcallbackPtr(&printSATdata); // Enable automatic NAV SAT messages with callback to printSATdata
  myGNSS.setAutoAOPSTATUScallbackPtr(&printAOPstatus); // Enable automatic NAV AOPSTATUS messages with callback to printAOPstatus
  myGNSS.setAutoPVTcallbackPtr(&printPVTdata); // Enable automatic NAV PVT messages with callback to printPVTdata
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.

  Serial.print(".");
  delay(50);
}
