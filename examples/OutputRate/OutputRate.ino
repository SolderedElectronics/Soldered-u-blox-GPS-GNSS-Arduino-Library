/**
 **************************************************
 * @file        OutputRate.ino
 * @brief       Increases the GNSS navigation output rate to 5 Hz and
 *              measures the actual achieved update rate.
 *
 * @details     Calls setNavigationFrequency(5) and raises the I2C clock to
 *              400 kHz. Polls the module every 25 ms for fresh position
 *              data, counts updates, and prints the rolling actual rate in
 *              Hz alongside latitude and longitude. The maximum achievable
 *              rate depends on the module model.
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
 * - Confirmed update rate, then latitude, longitude (degrees * 10^-7), and
 *   rolling actual rate (Hz) printed at up to 5 Hz.
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

unsigned long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.
unsigned long startTime = 0; //Used to calc the actual update rate.
unsigned long updateCount = 0; //Used to calc the actual update rate.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  // Increase I2C clock speed to 400kHz to cope with the high navigation rate
  // (We normally recommend running the bus at 100kHz)
  Wire.setClock(400000);
  
  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.setNavigationFrequency(5); //Set output to 5 times a second

  uint8_t rate = myGNSS.getNavigationFrequency(); //Get the update rate of this module
  Serial.print("Current update rate: ");
  Serial.println(rate);

  startTime = millis();
}

void loop()
{
  //Query module every 25 ms. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available. This is defined
  //by the update freq.
  if (millis() - lastTime > 25)
  {
    lastTime = millis(); //Update the timer
    
    long latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);

    updateCount++;

    //Calculate the actual update rate based on the sketch start time and the 
    //number of updates we've received.
    Serial.print(F(" Rate: "));
    Serial.print( updateCount / ((millis() - startTime) / 1000.0), 2);
    Serial.print(F("Hz"));

    Serial.println();
  }
}
