/**
 **************************************************
 * @file        FactoryDefaultviaI2C.ino
 * @brief       Resets a u-blox GNSS module to factory defaults over I2C
 *              after user confirmation.
 *
 * @details     Prompts the user to press a key in the Serial Monitor, then
 *              calls factoryReset() which clears baud rate, I2C address,
 *              update rate, and all stored settings. Waits 5 seconds for
 *              the module to restart, then reconnects and prints a
 *              confirmation message.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) Press any key to trigger the factory reset.
 *
 * Expected output:
 * - Prompt to press a key, then "Unit has now been factory reset.
 *   Freezing..." after the reset completes.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  while (Serial.available()) Serial.read(); //Trash any incoming chars
  Serial.println("Press a key to reset module to factory defaults");
  while (Serial.available() == false) ; //Wait for user to send character

  myGNSS.factoryReset(); //Reset everything: baud rate, I2C address, update rate, everything.

  delay(5000); // Wait while the module restarts

  while (myGNSS.begin() == false) //Attempt to re-connect
  {
    delay(1000);
    Serial.println(F("Attempting to re-connect to u-blox GNSS..."));
  }

  Serial.println("Unit has now been factory reset. Freezing...");
  while(1); // Do nothing more
}

void loop()
{

}
