/**
 **************************************************
 * @file        AutoPVT.ino
 * @brief       Configures a u-blox GNSS module to automatically push PVT
 *              navigation reports over I2C and reads them non-blocking.
 *
 * @details     Calls setAutoPVT(true) so the module sends UBX-NAV-PVT
 *              automatically at 2 Hz. getPVT() returns immediately with the
 *              latest queued solution instead of blocking while waiting for
 *              a fresh one. Only valid LLH positions (getInvalidLlh() ==
 *              false) are printed. Over I2C the module queues PVT packets
 *              in its internal ~4 KB buffer for retrieval on the next poll.
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
 * - Dots while waiting for a valid fix, then latitude, longitude (degrees *
 *   10^-7), altitude (mm), SIV, PDOP, NED velocities (mm/s), and
 *   accuracy estimates printed for each valid PVT solution.
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

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.setNavigationFrequency(2); //Produce two solutions per second
  myGNSS.setAutoPVT(true); //Tell the GNSS to "send" each solution
  //myGNSS.saveConfiguration(); //Optional: Save the current settings to flash and BBR
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
