/**
 **************************************************
 * @file        DynamicModel.ino
 * @brief       Sets the u-blox dynamic platform model and reads position
 *              using the chosen model.
 *
 * @details     Calls setDynamicModel() to switch the navigation engine to
 *              PORTABLE mode (configurable), then verifies the change with
 *              getDynamicModel(). Supported models are: PORTABLE, STATIONARY,
 *              PEDESTRIAN, AUTOMOTIVE, SEA, AIRBORNE1g, AIRBORNE2g,
 *              AIRBORNE4g, WRIST, BIKE. Latitude and longitude are returned
 *              as integers scaled by 10^7; divide by 10,000,000 to get
 *              decimal degrees.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Optionally change DYN_MODEL_PORTABLE to another model constant.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Confirmation of the new dynamic model number, then latitude, longitude,
 *   and altitude (mm) printed once per second.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark (PaulZC) @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
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

  // If we are going to change the dynamic platform model, let's do it here.
  // Possible values are:
  // PORTABLE, STATIONARY, PEDESTRIAN, AUTOMOTIVE, SEA, AIRBORNE1g, AIRBORNE2g, AIRBORNE4g, WRIST, BIKE

  if (myGNSS.setDynamicModel(DYN_MODEL_PORTABLE) == false) // Set the dynamic model to PORTABLE
  {
    Serial.println(F("*** Warning: setDynamicModel failed ***"));
  }
  else
  {
    Serial.println(F("Dynamic platform model changed successfully!"));
  }

  // Let's read the new dynamic model to see if it worked
  uint8_t newDynamicModel = myGNSS.getDynamicModel();
  if (newDynamicModel == DYN_MODEL_UNKNOWN)
  {
    Serial.println(F("*** Warning: getDynamicModel failed ***"));
  }
  else
  {
    Serial.print(F("The new dynamic model is: "));
    Serial.println(newDynamicModel);
  }

  //myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_NAVCONF); //Uncomment this line to save only the NAV settings to flash and BBR
}

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
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

    Serial.println();
  }
}
