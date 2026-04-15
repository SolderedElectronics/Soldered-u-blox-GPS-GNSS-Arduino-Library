/**
 **************************************************
 * @file        GetPosition.ino
 * @brief       Reads latitude, longitude, altitude, and SIV from a u-blox
 *              GNSS module over SPI using UBX binary commands.
 *
 * @details     Connects via SPI at 4 MHz, sets the SPI port to UBX-only
 *              output (disabling NMEA to reduce traffic), and polls for
 *              position once per second. The DSEL jumper on the u-blox
 *              board must be soldered closed to enable SPI. Some modules
 *              require the input protocol to be set to UBX-only via
 *              u-center before SPI communication works. Lat/long are
 *              integers scaled by 10^7; divide by 10,000,000 for decimal
 *              degrees.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with SPI
 * - Hardware:   Any u-blox GNSS module with SPI interface (e.g. ZED-F9P,
 *               NEO-M9N) — DSEL jumper must be closed
 *
 * How to use:
 * 1) Solder the DSEL/SPI jumper on the u-blox board.
 * 2) Connect SPI pins (CS=pin 10, COPI, CIPO, SCK) between the board and
 *    the u-blox module.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Latitude, longitude (degrees * 10^-7), altitude (mm), and satellites
 *   in view printed once per second.
 *
 * Notes:
 * - If SPI does not respond, connect via USB/u-center and set the SPI
 *   input protocol to UBX only.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Andrew Berridge @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <SPI.h> //Needed for SPI to GNSS

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

// #########################################

// Instantiate an instance of the SPI class. 
// Your configuration may be different, depending on the microcontroller you are using!

#define spiPort SPI // This is the SPI port on standard Ardino boards. Comment this line if you want to use a different port.

//SPIClass spiPort (HSPI); // This is the default SPI interface on some ESP32 boards. Uncomment this line if you are using ESP32.

// #########################################

const uint8_t csPin = 10; // On ATmega328 boards, SPI Chip Select is usually pin 10. Change this to match your board.

// #########################################

long lastTime = 0; //Simple local timer. Limits amount of SPI traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println(F("SparkFun u-blox Example"));

  spiPort.begin(); // begin the SPI port

  //myGNSS.enableDebugging(); // Uncomment this line to see helpful debug messages on Serial

  // Connect to the u-blox module using SPI port, csPin and speed setting
  // ublox devices generally work up to 5MHz. We'll use 4MHz for this example:
  if (myGNSS.begin(spiPort, csPin, 4000000) == false) 
  {
    Serial.println(F("u-blox GNSS not detected on SPI bus. Please check wiring. Freezing."));
    while (1);
  }
  
  //myGNSS.factoryDefault(); delay(5000); // Uncomment this line to reset the module back to its factory defaults

  myGNSS.setPortOutput(COM_PORT_SPI, COM_TYPE_UBX); //Set the SPI port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
}

void loop()
{
  //Query module only every second. Doing it more often will just cause SPI traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer
    
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
    Serial.print(F(" (mm)"));

    byte SIV = myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.println();
  }
}
