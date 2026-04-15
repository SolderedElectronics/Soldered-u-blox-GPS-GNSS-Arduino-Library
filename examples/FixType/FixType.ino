/**
 **************************************************
 * @file        FixType.ino
 * @brief       Reads and displays the GNSS fix type and RTK carrier solution
 *              type from a u-blox module over I2C.
 *
 * @details     Polls getFixType() and getCarrierSolutionType() once per
 *              second and prints human-readable descriptions. Fix types: 0 =
 *              No fix, 1 = Dead reckoning, 2 = 2D, 3 = 3D, 4 = GNSS + Dead
 *              reckoning, 5 = Time only. RTK types: 0 = No solution, 1 =
 *              Floating fix, 2 = Fixed solution.
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
 * - Latitude, longitude, altitude, fix type label, and RTK solution type
 *   printed once per second.
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
    Serial.println("Soldered u-blox GNSS Example 3 - Fix Type");

    Wire.begin();

    if (myGNSS.begin() == false)
    {
        Serial.println("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing.");
        while (1);
    }
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

        long altitude = myGNSS.getAltitude();
        Serial.print(F(" Alt: "));
        Serial.print(altitude);

        byte fixType = myGNSS.getFixType();
        Serial.print(F(" Fix: "));
        if (fixType == 0)      Serial.print(F("No fix"));
        else if (fixType == 1) Serial.print(F("Dead reckoning"));
        else if (fixType == 2) Serial.print(F("2D"));
        else if (fixType == 3) Serial.print(F("3D"));
        else if (fixType == 4) Serial.print(F("GNSS + Dead reckoning"));
        else if (fixType == 5) Serial.print(F("Time only"));

        byte RTK = myGNSS.getCarrierSolutionType();
        Serial.print(F(" RTK: "));
        Serial.print(RTK);
        if (RTK == 0)      Serial.print(F(" (No solution)"));
        else if (RTK == 1) Serial.print(F(" (High precision floating fix)"));
        else if (RTK == 2) Serial.print(F(" (High precision fix)"));

        Serial.println();
    }
}
