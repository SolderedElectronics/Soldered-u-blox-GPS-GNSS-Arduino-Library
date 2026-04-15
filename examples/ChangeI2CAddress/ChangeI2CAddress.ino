/**
 **************************************************
 * @file        ChangeI2CAddress.ino
 * @brief       Changes the I2C address of a u-blox GNSS module and saves it
 *              to non-volatile memory.
 *
 * @details     Connects to the module at its current address (default 0x42),
 *              prompts the user to confirm, then calls setI2CAddress() to
 *              assign a new address. The new address is stored in flash/BBR
 *              and survives power cycles. If the address change fails an I2C
 *              bus scan is performed to help diagnose the issue.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Edit oldAddress and newAddress in the sketch if needed.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud and press any key to trigger the
 *    address change.
 *
 * Expected output:
 * - Confirmation that the GNSS was found at the old address, followed by
 *   confirmation that the address was successfully changed (or an I2C scan
 *   result if it failed).
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

long lastTime = 0; //Tracks the passing of 2000ms (2 seconds)

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  byte oldAddress = 0x42; //The default address for u-blox modules is 0x42
  byte newAddress = 0x3F; //Address you want to change to. Valid is 0x08 to 0x77.

  while (Serial.available()) Serial.read(); //Trash any incoming chars
  Serial.print("Press a key to change address to 0x");
  Serial.println(newAddress, HEX);
  while (Serial.available() == false) ; //Wait for user to send character

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial
  
  if (myGNSS.begin(Wire, oldAddress) == true) //Connect to the u-blox module using Wire port and the old address
  {
    Serial.print("GNSS found at address 0x");
    Serial.println(oldAddress, HEX);

    myGNSS.setI2CAddress(newAddress); //Change I2C address of this device
    //Device's I2C address is stored to memory and loaded on each power-on

    delay(2000); // Allow time for the change to take

    if (myGNSS.begin(Wire, newAddress) == true)
    {
      myGNSS.saveConfiguration(); //Save the current settings to flash and BBR
      
      Serial.print("Address successfully changed to 0x");
      Serial.println(newAddress, HEX);
      Serial.print("Now load another example sketch using .begin(Wire, 0x");
      Serial.print(newAddress, HEX);
      Serial.println(") to use this GPS module");
      Serial.println("Freezing...");
      while (1);
    }
  }

  //Something went wrong, begin looking for the I2C device
  Serial.println("Address change probably failed. Beginning an I2C scan.");

  Wire.begin();
}

void loop() {

  byte address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
//    else if (error == 4)
//    {
//      Serial.print("Unknown error at address 0x");
//      if (address < 16)
//        Serial.print("0");
//      Serial.println(address, HEX);
//    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}
