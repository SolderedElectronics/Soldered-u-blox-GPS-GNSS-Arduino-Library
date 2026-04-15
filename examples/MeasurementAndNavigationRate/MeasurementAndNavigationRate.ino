/**
 **************************************************
 * @file        MeasurementAndNavigationRate.ino
 * @brief       Demonstrates how to read and set the measurement rate and
 *              navigation rate on a u-blox GNSS module.
 *
 * @details     The measurement rate is the interval between GNSS
 *              measurements (e.g. 1000 ms = 1 Hz). The navigation rate is
 *              the ratio of measurements to navigation solutions (e.g. 12
 *              means one solution every 12 measurements). This example sets
 *              the measurement interval to 5 s and the navigation ratio to
 *              12 (one solution per minute), then prints each new PVT
 *              solution together with the elapsed interval. Tested on
 *              ZED-F9P and ZOE-M8Q.
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
 * - Current and new measurement interval (ms) and navigation ratio printed
 *   at startup, then latitude, longitude, and elapsed interval (s) printed
 *   once per navigation solution (~60 s with these settings).
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

unsigned long lastTime = 0; //Simple local timer. Used to calc the message interval.

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

  // Uncomment the next line if you need to completely reset your module
  //myGNSS.factoryDefault(); delay(5000); // Reset everything and wait while the module restarts

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)

  // Begin by printing the current measurement rate and navigation rate

  uint16_t rate = myGNSS.getMeasurementRate(); //Get the measurement rate of this module
  Serial.print("Current measurement interval (ms): ");
  Serial.println(rate);

  rate = myGNSS.getNavigationRate(); //Get the navigation rate of this module
  Serial.print("Current navigation ratio (cycles): ");
  Serial.println(rate);

  // The measurement rate is the elapsed time between GNSS measurements, which defines the rate
  // e.g. 100 ms => 10 Hz, 1000 ms => 1 Hz, 10000 ms => 0.1 Hz.
  // Let's set the measurement rate (interval) to 5 seconds = 5000 milliseconds
  if (myGNSS.setMeasurementRate(5000) == false)
  {
    Serial.println(F("Could not set the measurement rate. Freezing."));
    while (1);
  }

  // setMeasurementRate will set i2cPollingWait to a quarter of the interval
  // Let's override that so we can poll the module more frequently and avoid timeouts
  myGNSS.setI2CpollingWait(25); // Set i2cPollingWait to 25ms

  // The navigation rate is the ratio between the number of measurements and the number of navigation solutions
  // e.g. 5 means five measurements for every navigation solution. Maximum value is 127
  // Let's set the navigation rate (ratio) to 12 to produce a solution every minute
  if (myGNSS.setNavigationRate(12) == false)
  {
    Serial.println(F("Could not set the navigation rate. Freezing."));
    while (1);
  }

  // Read and print the updated measurement rate and navigation rate

  rate = myGNSS.getMeasurementRate(); //Get the measurement rate of this module
  Serial.print("New measurement interval (ms): ");
  Serial.println(rate);

  rate = myGNSS.getNavigationRate(); //Get the navigation rate of this module
  Serial.print("New navigation ratio (cycles): ");
  Serial.println(rate);

  lastTime = millis();
}

void loop()
{
  // i2cPollingWait will prevent us from thrashing the I2C bus

  if (myGNSS.getPVT()) //Check for new Position, Velocity, Time data. getPVT returns true if new data is available.
  {    
      long latitude = myGNSS.getLatitude();
      Serial.print(F("Lat: "));
      Serial.print(latitude);

      long longitude = myGNSS.getLongitude();
      Serial.print(F(" Long: "));
      Serial.print(longitude);

      //Calculate the interval since the last message
      Serial.print(F(" Interval: "));
      Serial.print(((float)(millis() - lastTime)) / 1000.0, 2);
      Serial.print(F("s"));

      Serial.println();

      lastTime = millis(); //Update lastTime
  }
}
