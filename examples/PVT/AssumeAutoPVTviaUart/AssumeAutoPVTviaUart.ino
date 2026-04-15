/**
 **************************************************
 * @file        AssumeAutoPVTviaUart.ino
 * @brief       Reads automatic PVT messages from a u-blox module over a
 *              single RX-only UART wire using assumeAutoPVT().
 *
 * @details     Uses assumeAutoPVT(true, true) to tell the library to expect
 *              the module to push PVT messages without being polled, and to
 *              handle parsing implicitly when getPVT() is called. Only the
 *              RX pin is required — no TX needed. The module must already be
 *              configured to output cyclic PVT messages (run the AutoPVTviaUart
 *              example first if needed). At 38400 baud, 100 PVT bytes arrive
 *              in ~26 ms; call getPVT() at least every 5 ms on low-RAM boards.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with SoftwareSerial or Serial2
 * - Hardware:   Any u-blox GNSS module with UART1 TX connected to board RX
 *
 * How to use:
 * 1) Pre-configure the module to output automatic PVT (run AutoPVTviaUart
 *    first).
 * 2) Connect only the module's UART TX pin to pin 10 (SoftwareSerial RX).
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

  //Use any Serial port with at least a RX Pin connected or a receive only version of SoftwareSerial here
  //Assume that the U-Blox GNSS is running at baudRate baud
  mySerial.begin(baudRate);
  // No need to check return value as internal call to isConnected() will not succeed
  myGNSS.begin(mySerial);

  // Tell the library we are expecting the module to send PVT messages by itself to our Rx pin.
  // You can set second parameter to "false" if you want to control the parsing and eviction of the data (need to call checkUblox cyclically)
  myGNSS.assumeAutoPVT(true, true);

}

void loop()
{
  // If implicit updates are enabled, calling getPVT will trigger parsing of the incoming messages
  // and return true once a PVT message has been parsed.
  // In case you want to use explicit updates, wrap this in a timer and call checkUblox()
  // as often as needed, not to overflow your UART buffers.
  //
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
