/**
 **************************************************
 * @file        End.ino
 * @brief       Demonstrates the end() function that frees library RAM and
 *              stops automatic GNSS message processing.
 *
 * @details     Calls end() to release all dynamically allocated memory
 *              (PVT buffer, file buffer, etc.), then re-initialises the
 *              module with a small 128-byte file buffer each loop iteration.
 *              This is useful on memory-constrained boards where RAM must be
 *              reclaimed between GNSS reads.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - File buffer size printed each cycle, followed by latitude, longitude,
 *   and altitude from a fresh PVT query.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

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
  //myGNSS.saveConfiguration(); //Optional: Save the current settings to flash and BBR

  myGNSS.end(); // Call end now just because we can - it won't do much as we haven't used any automatic messages
}

void loop()
{
  // Allocate 128 bytes for file storage - this checks that issue #20 has been resolved
  // Allocating only 128 bytes will let this code run on the ATmega328P
  // If your processor has plenty of RAM, you can increase this to something useful like 16KB
  myGNSS.setFileBufferSize(128);
  
  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected. Freezing."));
    while (1);
  }

  Serial.print(F("The file buffer size is: "));
  Serial.println(myGNSS.getFileBufferSize());    

  // Request Position, Velocity, Time
  // RAM will be allocated for PVT message processing
  // getPVT will return true is fresh PVT data was received and processed
  if (myGNSS.getPVT())
  {
    long latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.println(F(" (mm)"));
  }

  // Calling end will free the RAM allocated for file storage and PVT processing
  // Calling end is optional. You can comment the next line if you want to.
  myGNSS.end();
}
