/**
 **************************************************
 * @file        FactoryDefaultsviaSerial.ino
 * @brief       Tests baud-rate auto-detection, hard reset, and factory
 *              reset of a u-blox GNSS module over UART.
 *
 * @details     Steps through four states: (0) auto-detect baud rate and
 *              switch to 38400, (1) issue hardReset() and verify
 *              reconnection, (2) issue factoryReset() and verify
 *              reconnection at the default baud rate, (3) print the
 *              protocol version. Uses SoftwareSerial (pins 10/11) by
 *              default; switch to Serial1 by uncommenting the define.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with a hardware or software
 *               serial port
 * - Hardware:   Any u-blox GNSS module connected via UART
 *
 * How to use:
 * 1) Connect the u-blox UART TX to pin 10 (RX) and RX to pin 11 (TX), or
 *    use Serial1 by uncommenting the appropriate define.
 * 2) Set defaultRate to 9600 for M8 modules or 38400 for F9 modules.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - State-machine progress messages for each reset step, ending with the
 *   protocol version and "Freezing...".
 *
 * Notes:
 * - On ATmega328P (UNO) with ZED-F9P, getProtocolVersion may fail because
 *   the response exceeds the serial buffer size.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Thorsten von Eicken @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

#include <SoftwareSerial.h>

//#define mySerial Serial1 // Uncomment this line to connect via Serial1
// - or -
SoftwareSerial mySerial(10, 11); // Uncomment this line to connect via SoftwareSerial(RX, TX). Connect pin 10 to GNSS TX pin.

#define defaultRate 9600 // Uncomment this line if you are using an M8 - which defaults to 9600 Baud on UART1
// - or -
//#define defaultRate 38400 // Uncomment this line if you are using an F9 - which defaults to 38400 Baud on UART1

int state = 0; // steps through auto-baud, reset, etc states

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial
}

void loop()
{
    Serial.print("===== STATE ");
    Serial.println(state);
    switch (state) {
    case 0: // auto-baud connection, then switch to 38400 and save config
        do {
            Serial.println("GNSS: trying 38400 baud");
            mySerial.begin(38400);
            if (myGNSS.begin(mySerial)) break;

            delay(100);
            Serial.println("GNSS: trying 9600 baud");
            mySerial.begin(9600);
            if (myGNSS.begin(mySerial)) {
                Serial.println("GNSS: connected at 9600 baud, switching to 38400");
                myGNSS.setSerialRate(38400);
                delay(100);
            } else {
                delay(2000); //Wait a bit before trying again to limit the Serial output flood
            }
        } while(1);
        myGNSS.setUART1Output(COM_TYPE_UBX); //Set the UART port to output UBX only
        myGNSS.saveConfiguration(); //Save the current settings to flash and BBR
        Serial.println("GNSS serial connected, saved config");
        state++;
        break;
    case 1: // hardReset, expect to see GNSS back at 38400 baud
        Serial.println("Issuing hardReset (cold start)");
        myGNSS.hardReset();
        delay(2000);
        mySerial.begin(38400);
        if (myGNSS.begin(mySerial)) {
            Serial.println("Success.");
            state++;
        } else {
            Serial.println("*** GNSS did not respond at 38400 baud, starting over.");
            state = 0;
        }
        break;
    case 2: // factoryReset, expect to see GNSS back at defaultRate baud
        Serial.println("Issuing factoryReset");
        myGNSS.factoryReset();
        delay(5000); // takes more than one second... a loop to resync would be best
        mySerial.begin(defaultRate);
        if (myGNSS.begin(mySerial)) {
            Serial.println("Success.");
            state++;
        } else {
            Serial.println("*** GNSS did not come back at defaultRate baud, starting over.");
            state = 0;
        }
        break;
    case 3: // print version info
        // Note: this may fail on boards like the UNO (ATmega328P) with modules like the ZED-F9P
        // because getProtocolVersion returns a lot of data - more than the UNO's serial buffer can hold
        Serial.print("GNSS protocol version: ");
        Serial.print(myGNSS.getProtocolVersionHigh());
        Serial.print('.');
        Serial.println(myGNSS.getProtocolVersionLow());
        Serial.println("All finished! Freezing...");
        while(1);
    }
    delay(1000);
}
