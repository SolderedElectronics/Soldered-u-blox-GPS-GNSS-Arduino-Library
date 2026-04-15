/**
 **************************************************
 * @file        GetProtocolVersion_Serial.ino
 * @brief       Reads and prints the UBX protocol version from a u-blox
 *              GNSS module over UART (Serial1 or SoftwareSerial).
 *
 * @details     Auto-detects the baud rate by trying 38400 then 9600 bps.
 *              Calls getProtocolVersionHigh() and getProtocolVersionLow()
 *              to retrieve the major.minor protocol version.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with a hardware or software
 *               serial port
 * - Hardware:   Any u-blox GNSS module connected via UART
 *
 * How to use:
 * 1) Connect the u-blox UART TX pin to the board's RX pin (pin 10 for
 *    SoftwareSerial, or Serial1 RX for hardware serial).
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Baud rate detection messages, then the protocol version string
 *   e.g. "Version: 27.30".
 *
 * Notes:
 * - On ATmega328P (UNO) boards with the ZED-F9P, getProtocolVersion may
 *   fail because the response is larger than the serial buffer can hold.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <SoftwareSerial.h>

#define mySerial Serial1 // Uncomment this line to connect via Serial1
// - or -
//SoftwareSerial mySerial(10, 11); // Uncomment this line to connect via SoftwareSerial(RX, TX). Connect pin 10 to GNSS TX pin.

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Serial.println("Trying 38400 baud");
  mySerial.begin(38400);
  if (myGNSS.begin(mySerial))
  {
    Serial.println("GNSS connected at 38400 baud");
  }
  else
  {
    Serial.println("Trying 9600 baud");
    mySerial.begin(9600);
    if (myGNSS.begin(mySerial))
    {
      Serial.println("GNSS connected at 9600 baud");
    }
    else
    {
      Serial.println("Could not connect to GNSS. Freezing...");
      while(1); // Do nothing more
    }
  }

  Serial.print(F("Version: "));
  byte versionHigh = myGNSS.getProtocolVersionHigh();
  Serial.print(versionHigh);
  Serial.print(".");
  byte versionLow = myGNSS.getProtocolVersionLow();
  Serial.print(versionLow);
}

void loop()
{
  //Do nothing
}
