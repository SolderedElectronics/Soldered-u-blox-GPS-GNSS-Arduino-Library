/**
 **************************************************
 * @file        AutoPVT.ino
 * @brief       Configures a u-blox GNSS module over SPI to automatically
 *              send PVT navigation reports and reads them via getPVT().
 *
 * @details     Calls setAutoPVT(true) so the module pushes PVT packets
 *              automatically, eliminating the blocking wait in getPVT().
 *              The SPI port is set to UBX-only output at 4 MHz. The DSEL
 *              jumper on the u-blox board must be soldered closed to enable
 *              SPI. Some modules require the input protocol to be set to
 *              UBX-only via u-center before SPI communication works.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with SPI
 * - Hardware:   Any u-blox GNSS module with SPI interface (e.g. ZED-F9P,
 *               NEO-M8P, NEO-M9N) — DSEL jumper must be closed
 *
 * How to use:
 * 1) Solder the DSEL/SPI jumper on the u-blox board.
 * 2) Connect SPI pins (CS=pin 10, COPI, CIPO, SCK) between the board and
 *    the u-blox module.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Latitude, longitude (degrees * 10^-7), altitude (mm), SIV, PDOP, NED
 *   velocities (mm/s), and accuracy estimates printed for each valid fix.
 *
 * Notes:
 * - If SPI does not respond, connect via USB/u-center and set the SPI
 *   input protocol to UBX only.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
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

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

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

  myGNSS.setNavigationFrequency(2); //Produce two solutions per second
  myGNSS.setAutoPVT(true); //Tell the GNSS to "send" each solution
  //myGNSS.saveConfiguration(); //Optional: Save _all_ the current settings to flash and BBR
}

void loop()
{
  // Calling getPVT returns true if there actually is a fresh navigation solution available.
  // Start the reading only when valid LLH is available
  if (myGNSS.getPVT() && (myGNSS.getInvalidLlh() == false))
  {
    Serial.println();
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

    int PDOP = myGNSS.getPDOP();
    Serial.print(F(" PDOP: "));
    Serial.print(PDOP);
    Serial.print(F(" (10^-2)"));

    int nedNorthVel = myGNSS.getNedNorthVel();
    Serial.print(F(" VelN: "));
    Serial.print(nedNorthVel);
    Serial.print(F(" (mm/s)"));

    int nedEastVel = myGNSS.getNedEastVel();
    Serial.print(F(" VelE: "));
    Serial.print(nedEastVel);
    Serial.print(F(" (mm/s)"));

    int nedDownVel = myGNSS.getNedDownVel();
    Serial.print(F(" VelD: "));
    Serial.print(nedDownVel);
    Serial.print(F(" (mm/s)"));

    int verticalAccEst = myGNSS.getVerticalAccEst();
    Serial.print(F(" VAccEst: "));
    Serial.print(verticalAccEst);
    Serial.print(F(" (mm)"));

    int horizontalAccEst = myGNSS.getHorizontalAccEst();
    Serial.print(F(" HAccEst: "));
    Serial.print(horizontalAccEst);
    Serial.print(F(" (mm)"));

    int speedAccEst = myGNSS.getSpeedAccEst();
    Serial.print(F(" SpeedAccEst: "));
    Serial.print(speedAccEst);
    Serial.print(F(" (mm/s)"));

    int headAccEst = myGNSS.getHeadingAccEst();
    Serial.print(F(" HeadAccEst: "));
    Serial.print(headAccEst);
    Serial.print(F(" (degrees * 10^-5)"));

    if (myGNSS.getHeadVehValid() == true) {
      int headVeh = myGNSS.getHeadVeh();
      Serial.print(F(" HeadVeh: "));
      Serial.print(headVeh);
      Serial.print(F(" (degrees * 10^-5)"));

      int magDec = myGNSS.getMagDec();
      Serial.print(F(" MagDec: "));
      Serial.print(magDec);
      Serial.print(F(" (degrees * 10^-2)"));

      int magAcc = myGNSS.getMagAcc();
      Serial.print(F(" MagAcc: "));
      Serial.print(magAcc);
      Serial.print(F(" (degrees * 10^-2)"));
    }

    Serial.println();
  } else {
    Serial.print(".");
    delay(50);
  }
}
