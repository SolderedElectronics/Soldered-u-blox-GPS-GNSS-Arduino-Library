/**
 **************************************************
 * @file        HNR.ino
 * @brief       Read High Navigation Rate (HNR) attitude, dynamics and PVT data using callbacks on a NEO-M8U.
 *
 * @details     This example configures the NEO-M8U at 10 Hz HNR and registers
 *              callbacks for HNR-ATT (roll, pitch, heading), HNR-INS (accelerations)
 *              and HNR-PVT (high-rate position, velocity and time). Data is processed
 *              automatically without polling.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox NEO-M8U running UDR firmware >= 1.31
 *
 * How to use:
 * 1) Connect the NEO-M8U to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Roll, pitch and heading in degrees from HNR-ATT.
 * - X/Y/Z accelerations from HNR-INS.
 * - Nanoseconds, latitude and longitude from HNR-PVT at 10 Hz.
 *
 * Notes:
 * - Requires NEO-M8U UDR firmware >= 1.31; update via u-center if needed.
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

// Callback: printHNRATTdata will be called when new HNR ATT data arrives
// See u-blox_structs.h for the full definition of UBX_HNR_ATT_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoHNRATTcallback
//        /                  _____  This _must_ be UBX_HNR_ATT_data_t
//        |                 /                   _____ You can use any name you like for the struct
//        |                 |                  /
//        |                 |                  |
void printHNRATTdata(UBX_HNR_ATT_data_t *ubxDataStruct)
{
  Serial.println();
  Serial.print(F("Roll: ")); // Print selected data
  Serial.print((float)ubxDataStruct->roll / 100000.0, 2); // Convert roll to degrees
  Serial.print(F(" Pitch: "));
  Serial.print((float)ubxDataStruct->pitch / 100000.0, 2); // Convert pitch to degrees
  Serial.print(F(" Heading: "));
  Serial.println((float)ubxDataStruct->heading / 100000.0, 2); // Convert heading to degrees
}

// Callback: printHNRINSdata will be called when new HNR INS data arrives
// See u-blox_structs.h for the full definition of UBX_HNR_INS_data_t
void printHNRINSdata(UBX_HNR_INS_data_t *ubxDataStruct)
{
  Serial.print(F("xAccel: ")); // Print selected data
  Serial.print(ubxDataStruct->xAccel);
  Serial.print(F(" yAccel: "));
  Serial.print(ubxDataStruct->yAccel);
  Serial.print(F(" zAccel: "));
  Serial.println(ubxDataStruct->zAccel);
}

// Callback: printHNRPVTdata will be called when new HNR PVT data arrives
// See u-blox_structs.h for the full definition of UBX_HNR_PVT_data_t
void printHNRPVTdata(UBX_HNR_PVT_data_t *ubxDataStruct)
{
  Serial.print(F("ns: ")); // Print selected data
  Serial.print(ubxDataStruct->nano);
  Serial.print(F(" Lat: "));
  Serial.print(ubxDataStruct->lat);
  Serial.print(F(" Lon: "));
  Serial.println(ubxDataStruct->lon);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println(F("SparkFun u-blox Example"));

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable debug messages on Serial

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  if (myGNSS.setHNRNavigationRate(10) == true) //Set the High Navigation Rate to 10Hz
    Serial.println(F("setHNRNavigationRate was successful"));
  else
    Serial.println(F("setHNRNavigationRate was NOT successful"));

  if (myGNSS.setAutoHNRATTcallbackPtr(&printHNRATTdata) == true) // Enable automatic HNR ATT messages with callback to printHNRATTdata
    Serial.println(F("setAutoHNRATTcallback successful"));

  if (myGNSS.setAutoHNRINScallbackPtr(&printHNRINSdata) == true) // Enable automatic HNR INS messages with callback to printHNRINSdata
    Serial.println(F("setAutoHNRINScallback successful"));

  if (myGNSS.setAutoHNRPVTcallbackPtr(&printHNRPVTdata) == true) // Enable automatic HNR PVT messages with callback to printHNRPVTdata
    Serial.println(F("setAutoHNRPVTcallback successful"));
}

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.

  Serial.print(".");
  delay(25);
}
