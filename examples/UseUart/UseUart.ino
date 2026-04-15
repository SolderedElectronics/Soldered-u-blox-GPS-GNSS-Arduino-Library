/**
 **************************************************
 * @file        UseUart.ino
 * @brief       Connects to a u-blox GNSS module over UART at 38400 baud
 *              and reads position using UBX commands.
 *
 * @details     Auto-detects the baud rate (tries 38400 then 9600) and
 *              switches the module to 38400 bps if needed. Both the UART1
 *              and I2C ports are set to UBX-only output to reduce traffic.
 *              Latitude, longitude (degrees * 10^-7), altitude (mm), and
 *              satellites in view are printed once per second.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with SoftwareSerial or
 *               hardware Serial1
 * - Hardware:   Any u-blox GNSS module connected via UART
 *
 * How to use:
 * 1) Connect the u-blox UART TX to pin 10 (RX) and RX to pin 11 (TX) for
 *    SoftwareSerial, or use Serial1 by modifying the include.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Baud rate detection messages, then latitude, longitude (degrees *
 *   10^-7), altitude (mm), and satellites in view printed once per second.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX. Pin 10 on Uno goes to TX pin on GNSS module.

long lastTime = 0; //Simple local timer. Limits amount of I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  //Assume that the U-Blox GNSS is running at 9600 baud (the default) or at 38400 baud.
  //Loop until we're in sync and then ensure it's at 38400 baud.
  do {
    Serial.println("GNSS: trying 38400 baud");
    mySerial.begin(38400);
    if (myGNSS.begin(mySerial) == true) break;

    delay(100);
    Serial.println("GNSS: trying 9600 baud");
    mySerial.begin(9600);
    if (myGNSS.begin(mySerial) == true) {
        Serial.println("GNSS: connected at 9600 baud, switching to 38400");
        myGNSS.setSerialRate(38400);
        delay(100);
    } else {
        //myGNSS.factoryReset();
        delay(2000); //Wait a bit before trying again to limit the Serial output
    }
  } while(1);
  Serial.println("GNSS serial connected");

  myGNSS.setUART1Output(COM_TYPE_UBX); //Set the UART port to output UBX only
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfiguration(); //Save the current settings to flash and BBR
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

    byte SIV = myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.println();
  }
}
