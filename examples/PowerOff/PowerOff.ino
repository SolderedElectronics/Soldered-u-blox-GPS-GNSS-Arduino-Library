/**
 **************************************************
 * @file        PowerOff.ino
 * @brief       Powers off a u-blox GNSS module for a set duration and then
 *              wakes it using a GPIO pin on the microcontroller.
 *
 * @details     Calls powerOff(20000) to put the module into low-power sleep
 *              for 20 seconds, then drives WAKEUP_PIN low-high-low to
 *              assert a wake-up signal on the module's interrupt pin. The
 *              commented-out powerOffWithInterrupt() variant allows the
 *              module to self-wake on a specified interrupt source. Tested
 *              on SAM-M8Q. While powered off, avoid querying the module as
 *              I2C traffic can inadvertently wake it.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C and a spare GPIO
 * - Hardware:   Any u-blox GNSS module; connect WAKEUP_PIN to the module's
 *               INT0 (or other interrupt) pin for hardware wake-up
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Connect the board's pin 5 (WAKEUP_PIN) to the module's INT0 pin.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud.
 *
 * Expected output:
 * - "Powering off module for 20s", then "waking up module via pin 5" after
 *   10 seconds.
 *
 * Notes:
 * - Querying the module while it is powered off may inadvertently wake it;
 *   pair this with microcontroller sleep for best power savings.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      bjorn @ unsurv.org
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

// define a digital pin capable of driving HIGH and LOW
#define WAKEUP_PIN 5

// Possible GNSS interrupt pins for powerOffWithInterrupt are:
// VAL_RXM_PMREQ_WAKEUPSOURCE_UARTRX  = uartrx
// VAL_RXM_PMREQ_WAKEUPSOURCE_EXTINT0 = extint0 (default)
// VAL_RXM_PMREQ_WAKEUPSOURCE_EXTINT1 = extint1
// VAL_RXM_PMREQ_WAKEUPSOURCE_SPICS   = spics
// These values can be or'd (|) together to enable interrupts on multiple pins

void wakeUp() {

  Serial.print("-- waking up module via pin " + String(WAKEUP_PIN));
  Serial.println(" on your microcontroller --");

  digitalWrite(WAKEUP_PIN, LOW);
  delay(1000);
  digitalWrite(WAKEUP_PIN, HIGH);
  delay(1000);
  digitalWrite(WAKEUP_PIN, LOW);
}


void setup() {

  pinMode(WAKEUP_PIN, OUTPUT);
  digitalWrite(WAKEUP_PIN, LOW);

  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  //myGNSS.enableDebugging(); // Enable debug messages

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  // Powering off for 20s, you should see the power consumption drop.
  Serial.println("-- Powering off module for 20s --");

  myGNSS.powerOff(20000);
  //myGNSS.powerOffWithInterrupt(20000, VAL_RXM_PMREQ_WAKEUPSOURCE_EXTINT0);

  delay(10000);

  // After 10 seconds wake the device via the specified pin on your microcontroller and module.
  wakeUp();
}

void loop() {
  //Do nothing
}
