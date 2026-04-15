/**
 **************************************************
 * @file        Great_Circle_Distance.ino
 * @brief       Calculates the great-circle distance and bearing from the
 *              current GNSS position to a fixed target location.
 *
 * @details     Reads latitude and longitude once per second, then uses
 *              Haversine-based distanceBetween() and courseTo() helper
 *              functions (adapted from TinyGPSPlus by Mikal Hart) to
 *              compute the distance in metres and the course in degrees to
 *              a hard-coded target position. Lat/long values are integers
 *              scaled by 10^7; divide by 10,000,000 for decimal degrees.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox GNSS module
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Edit TARGET_LAT and TARGET_LON in the sketch to your desired target.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Current latitude, longitude (degrees * 10^-7), distance to target (m),
 *   and bearing to target (degrees) printed once per second.
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

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

//#include <math.h> //Uncomment if required. May be needed for sqrt, atan2, etc..

double distanceBetween(long lat1_l, long long1_l, long lat2_l, long long2_l)
{
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double lat1 = (double)lat1_l / 10000000.0; // Convert lat and long to degrees
  double long1 = (double)long1_l / 10000000.0;
  double lat2 = (double)lat2_l / 10000000.0;
  double long2 = (double)long2_l / 10000000.0;
  double delta = radians(long1-long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795;
}

double courseTo(long lat1_l, long long1_l, long lat2_l, long long2_l)
{
  // returns course in degrees (North=0, West=270) from position 1 to position 2,
  // both specified as signed decimal-degrees latitude and longitude.
  // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
  // Courtesy of Maarten Lamers
  double lat1 = (double)lat1_l / 10000000.0; // Convert lat and long to degrees
  double long1 = (double)long1_l / 10000000.0;
  double lat2 = (double)lat2_l / 10000000.0;
  double long2 = (double)long2_l / 10000000.0;
  double dlon = radians(long2-long1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double a1 = sin(dlon) * cos(lat2);
  double a2 = sin(lat1) * cos(lat2) * cos(dlon);
  a2 = cos(lat1) * sin(lat2) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0)
  {
    a2 += TWO_PI;
  }
  return degrees(a2);
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
    Serial.println(F(" (degrees * 10^-7)"));

    static const long TARGET_LAT = 400909142, TARGET_LON = -1051849833; // SparkFun's location: degrees * 10^-7 (40.091 N, 105.185 W)
    
    double distanceToTarget = distanceBetween(
    latitude,
    longitude,
    TARGET_LAT, 
    TARGET_LON);

    Serial.print(F("Distance to target: "));
    Serial.print(distanceToTarget, 2);
    Serial.print(F(" (m)  "));
    
    double courseToTarget = courseTo(
    latitude,
    longitude,
    TARGET_LAT, 
    TARGET_LON);

    Serial.print(F("Course to target: "));
    Serial.print(courseToTarget, 1);
    Serial.println(F(" (degrees)"));
  }
}
