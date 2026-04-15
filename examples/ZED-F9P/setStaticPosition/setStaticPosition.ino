/**
 **************************************************
 * @file        setStaticPosition.ino
 * @brief       Set a fixed ECEF position for a GNSS base receiver.
 *
 * @details     Loads a known Earth-Centered, Earth-Fixed (ECEF) position into
 *              the receiver so it can operate as a static base immediately,
 *              without performing a new survey-in. This is useful when you
 *              already have a long-survey reference position.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox ZED-F9P (or compatible high-precision module)
 *
 * How to use:
 * 1) Replace the example ECEF coordinates with your surveyed base location.
 * 2) Connect the u-blox module to your board via I2C.
 * 3) Upload the sketch and open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Confirmation that static position was applied and base output enabled.
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
  Serial.begin(115200); // You may need to increase this for high navigation rates!
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println(F("SparkFun u-blox Example"));

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable debug messages

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)

  bool success = true;

  //-1280208.308,-4716803.847,4086665.811 is SparkFun HQ so...

  //Units are cm so 1234 = 12.34m
  //success &= myGNSS.setStaticPosition(-128020831, -471680385, 408666581);

  //Units are cm with a high precision extension so -1234.5678 should be called: (-123456, -78)
  success &= myGNSS.setStaticPosition(-128020830, -80, -471680384, -70, 408666581, 10); //With high precision 0.1mm parts

  //We can also set via lat/long
  //40.09029751,-105.18507900,1560.238
  //success &= myGNSS.setStaticPosition(400902975, -1051850790, 156024, true); //True at end enables lat/long input
  //success &= myGNSS.setStaticPosition(400902975, 10, -1051850790, 0, 156023, 80, true);

  if (!success) Serial.println(F("At least one call to setStaticPosition failed!"));

  //Now let's use getVals to read back the data
  //long ecefX = myGNSS.getVal32(0x40030003);
  //Serial.print("ecefX: ");
  //Serial.println(ecefX);

  Serial.println(F("Done!"));
}

void loop()
{
}
