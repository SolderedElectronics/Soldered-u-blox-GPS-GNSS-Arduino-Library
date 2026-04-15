/**
 **************************************************
 * @file        AutoPVTviaUart.ino
 * @brief       Configures a u-blox GNSS module to automatically push PVT
 *              navigation reports over UART and reads them non-blocking.
 *
 * @details     Calls setAutoPVT(true) so the module outputs UBX-NAV-PVT at
 *              2 Hz on UART1. getPVT() returns each fresh solution without
 *              blocking. Connects via SoftwareSerial (pins 10/11) at 38400
 *              baud by default; change baudRate to 9600 for M8 modules.
 *              At 38400 baud the 100-byte PVT message arrives in ~26 ms;
 *              call getPVT() at least every 5 ms on ATmega328P boards to
 *              avoid serial buffer overflow.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with SoftwareSerial or Serial2
 * - Hardware:   Any u-blox GNSS module connected via UART
 *
 * How to use:
 * 1) Connect the u-blox UART TX to pin 10 (RX) and RX to pin 11 (TX).
 * 2) Set baudRate to 9600 for M8 modules or leave at 38400 for F9.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - Dots while waiting, then latitude, longitude (degrees * 10^-7),
 *   altitude (mm), and satellites in view on each received PVT message.
 *
 * Notes:
 * - On ATmega328P boards call getPVT() every 5 ms to avoid serial buffer
 *   overflow at 38400 baud.
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

//#define mySerial Serial2 // Uncomment this line to connect via Serial2
// - or -
SoftwareSerial mySerial(10, 11); // Uncomment this line to connect via SoftwareSerial(RX, TX). Connect pin 10 to GNSS TX pin.

//#define baudRate 9600 // Uncomment this line to select 9600 Baud for the M8
// - or -
#define baudRate 38400 // Uncomment this line to select 38400 Baud for the F9

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  mySerial.begin(baudRate); // Start the Serial port

  if (myGNSS.begin(mySerial) == false) //Connect to the u-blox module using Serial
  {
    Serial.println(F("u-blox GNSS not detected. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setUART1Output(COM_TYPE_UBX); //Set the UART1 port to output UBX only (turn off NMEA noise)
  myGNSS.setNavigationFrequency(2); //Produce two solutions per second
  myGNSS.setAutoPVT(true); //Tell the GNSS to "send" each solution
  //myGNSS.saveConfiguration(); //Optional: Save the current settings to flash and BBR
}

void loop()
{
  // getPVT will return true if there actually is a fresh navigation solution available.
  // Important note: the PVT message is 100 bytes long. We need to call getPVT often enough
  // to prevent serial buffer overflows on boards like the original RedBoard / UNO.
  // At 38400 Baud, the 100 PVT bytes will arrive in 26ms.
  // On the RedBoard, we need to call getPVT every 5ms to keep up.
  if (myGNSS.getPVT())
  {
    Serial.println();

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
  else
  {
    delay(5); // Delay for 5ms only

    static int counter = 0; // Print a dot every 50ms
    counter++;
    if (counter > 10)
    {
      Serial.print(".");
      counter = 0;
    }
  }
}
