/**
 **************************************************
 * @file        GetPosition.ino
 * @brief       Queries a u-blox GNSS module for latitude, longitude,
 *              altitude, and satellites in view using UBX commands over I2C.
 *
 * @details     Polls the module once per second via UBX-only I2C output
 *              (NMEA disabled to reduce bus traffic). Latitude and longitude
 *              are returned as integers scaled by 10^7; divide by 10,000,000
 *              to obtain decimal degrees.
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
 * - Latitude (degrees * 10^-7), longitude (degrees * 10^-7), altitude (mm),
 *   and satellites in view printed once per second.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Josip Šimun Kuči @ soldered.com
 * @date        14-04-2026
 **************************************************/

#include <Wire.h>
#include <Soldered-GNSS.h>

Soldered_GNSS myGNSS;

long lastTime = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Soldered u-blox GNSS Example 2 - Get Position");

    Wire.begin();

    if (myGNSS.begin() == false)
    {
        Serial.println("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing.");
        while (1);
    }

    // Use UBX-only output to reduce I2C traffic
    myGNSS.setI2COutput(COM_TYPE_UBX);
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);
}

void loop()
{
    // Query the module only once per second
    if (millis() - lastTime > 1000)
    {
        lastTime = millis();

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
        Serial.println(SIV);
    }
}
