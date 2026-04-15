/**
 **************************************************
 * @file        NMEARead.ino
 * @brief       Reads raw NMEA sentences from a u-blox GNSS module over I2C
 *              and prints them to the Serial Monitor.
 *
 * @details     Enables combined UBX + NMEA output on the I2C port, then
 *              pipes all incoming NMEA sentences directly to Serial via
 *              setNMEAOutputPort(). checkUblox() is called every 250 ms to
 *              keep the I2C buffer drained.
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
 * - Raw NMEA sentences (e.g. $GNGGA, $GNRMC, etc.) printed continuously
 *   as they arrive from the module.
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

void setup()
{
    Serial.begin(115200);
    Serial.println("Soldered u-blox GNSS Example 1 - NMEA Read");

    Wire.begin();

    if (myGNSS.begin() == false)
    {
        Serial.println("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing.");
        while (1);
    }

    // Output both NMEA and UBX messages over I2C
    myGNSS.setI2COutput(COM_TYPE_UBX | COM_TYPE_NMEA);
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);

    // Pipe all NMEA sentences to the serial port
    myGNSS.setNMEAOutputPort(Serial);
}

void loop()
{
    // Check for new data; process bytes as they come in
    myGNSS.checkUblox();

    delay(250);
}
