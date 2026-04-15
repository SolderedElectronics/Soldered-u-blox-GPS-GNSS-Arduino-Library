/**
 **************************************************
 * @file        TIM_TM2.ino
 * @brief       Capture TIM-TM2 pulse-edge timing with nanosecond resolution using a callback.
 *
 * @details     This example configures the GNSS to produce TIM-TM2 messages and
 *              registers a callback to print the rising/falling edge flags, rising
 *              edge counter, and Time Of Week of each edge in milliseconds plus
 *              sub-millisecond nanoseconds. Connecting the PPS pin to the INT pin
 *              produces one TIM-TM2 message per second that can be used to study
 *              PPS timing with nanosecond precision.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module with a TIM-TM2 INT pin
 *               (e.g. ZED-F9P, NEO-M8P-2, NEO-M9N)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Connect the module's PPS pin to its INT pin with a jumper wire.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - newFallingEdge, newRisingEdge flags, rising edge counter, towMsR,
 *   towSubMsR (ns), towMsF and towSubMsF (ns) printed on each pulse edge.
 *
 * Notes:
 * - TIM-TM2 captures only one rising and one falling edge per navigation
 *   solution; only the most recent edges are reported if the INT signal is
 *   faster than the navigation rate. Rising-edge counts are still accumulated.
 * - Messages stop if the PPS-to-INT wire is disconnected.
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

int dotsPrinted = 0; // Print dots in rows of 50 while waiting for a TIM TM2 message

// Callback: printTIMTM2data will be called when new TIM TM2 data arrives
// See u-blox_structs.h for the full definition of UBX_TIM_TM2_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoTIMTM2callback
//        /                  _____  This _must_ be UBX_TIM_TM2_data_t
//        |                 /                   _____ You can use any name you like for the struct
//        |                 |                  /
//        |                 |                  |
void printTIMTM2data(UBX_TIM_TM2_data_t *ubxDataStruct)
{
    Serial.println();

    Serial.print(F("newFallingEdge: ")); // 1 if a new falling edge was detected
    Serial.print(ubxDataStruct->flags.bits.newFallingEdge);

    Serial.print(F(" newRisingEdge: ")); // 1 if a new rising edge was detected
    Serial.print(ubxDataStruct->flags.bits.newRisingEdge);

    Serial.print(F(" Rising Edge Counter: ")); // Rising edge counter
    Serial.print(ubxDataStruct->count);

    Serial.print(F(" towMsR: ")); // Time Of Week of rising edge (ms)
    Serial.print(ubxDataStruct->towMsR);

    Serial.print(F(" towSubMsR: ")); // Millisecond fraction of Time Of Week of rising edge in nanoseconds
    Serial.print(ubxDataStruct->towSubMsR);

    Serial.print(F(" towMsF: ")); // Time Of Week of falling edge (ms)
    Serial.print(ubxDataStruct->towMsF);

    Serial.print(F(" towSubMsF: ")); // Millisecond fraction of Time Of Week of falling edge in nanoseconds
    Serial.println(ubxDataStruct->towSubMsF);

    dotsPrinted = 0; // Reset dotsPrinted
}

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  myGNSS.setNavigationFrequency(1); //Produce one solution per second

  myGNSS.setAutoTIMTM2callbackPtr(&printTIMTM2data); // Enable automatic TIM TM2 messages with callback to printTIMTM2data
}

void loop()
{
  myGNSS.checkUblox(); // Check for the arrival of new data and process it.
  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed.

  Serial.print(".");
  delay(50);
  if (++dotsPrinted > 50)
  {
    Serial.println();
    dotsPrinted = 0;
  }
}
