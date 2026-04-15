/**
 **************************************************
 * @file        Geofence.ino
 * @brief       Sets up four concentric geofences around the current position
 *              and monitors entry/exit status using a u-blox GNSS module.
 *
 * @details     Waits for a valid 3D fix, then records the current latitude
 *              and longitude. Four geofences are defined at radii of 5 m,
 *              10 m, 15 m, and 20 m with 95 % confidence. The built-in LED
 *              is lit when the device is inside the combined (outermost)
 *              geofence. Tested on the ZOE-M8Q.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   Any u-blox M8 GNSS module (e.g. ZOE-M8Q, SAM-M8Q)
 *
 * How to use:
 * 1) Connect the u-blox module to your board via I2C.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud.
 * 4) Take the device outside and wait for a 3D fix — geofences will be set
 *    around your starting position automatically.
 *
 * Expected output:
 * - Fix status updates until 3D fix is acquired, then repeated lines
 *   showing geofence state (Unknown / Inside / Outside) and individual
 *   fence states. The LED reflects the combined geofence state.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark (PaulZC) @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#define LED LED_BUILTIN // Change this if your LED is on a different pin

#include <Wire.h> // Needed for I2C

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

void setup()
{
  pinMode(LED, OUTPUT);

  // Set up the I2C pins
  Wire.begin();

  // Start the console serial port
  Serial.begin(115200);
  while (!Serial); // Wait for the user to open the serial monitor
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println(F("u-blox M8 geofence example"));
  Serial.println();
  Serial.println();

  delay(1000); // Let the GNSS power up

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  //myGNSS.enableDebugging(); // Enable debug messages
  myGNSS.setI2COutput(COM_TYPE_UBX); // Limit I2C output to UBX (disable the NMEA noise)

  Serial.println(F("Waiting for a 3D fix..."));

  byte fixType = 0;

  while (fixType < 3)
  {
    fixType = myGNSS.getFixType(); // Get the fix type
    Serial.print(F("Fix: ")); // Print it
    Serial.print(fixType);
    if(fixType == 0) Serial.print(F(" = No fix"));
    else if(fixType == 1) Serial.print(F(" = Dead reckoning"));
    else if(fixType == 2) Serial.print(F(" = 2D"));
    else if(fixType == 3) Serial.print(F(" = 3D"));
    else if(fixType == 4) Serial.print(F(" = GNSS + Dead reckoning"));
    else if(fixType == 5) Serial.print(F(" = Time only"));
    Serial.println();
    delay(1000);
  }

  Serial.println(F("3D fix found!"));

  long latitude = myGNSS.getLatitude(); // Get the latitude in degrees * 10^-7
  Serial.print(F("Lat: "));
  Serial.print(latitude);

  long longitude = myGNSS.getLongitude(); // Get the longitude in degrees * 10^-7
  Serial.print(F("   Long: "));
  Serial.println(longitude);

  uint32_t radius = 500; // Set the radius to 5m (radius is in m * 10^-2 i.e. cm)

  byte confidence = 2; // Set the confidence level: 0=none, 1=68%, 2=95%, 3=99.7%, 4=99.99%

  // Call clearGeofences() to clear all existing geofences.
  Serial.print(F("Clearing any existing geofences. clearGeofences returned: "));
  Serial.println(myGNSS.clearGeofences());

  // It is possible to define up to four geofences.
  // Call addGeofence up to four times to define them.
  Serial.println(F("Setting the geofences:"));

  Serial.print(F("addGeofence for geofence 1 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));

  radius = 1000; // 10m
  Serial.print(F("addGeofence for geofence 2 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));

  radius = 1500; // 15m
  Serial.print(F("addGeofence for geofence 3 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));

  radius = 2000; // 20m
  Serial.print(F("addGeofence for geofence 4 returned: "));
  Serial.println(myGNSS.addGeofence(latitude, longitude, radius, confidence));
}

void loop()
{
  geofenceState currentGeofenceState; // Create storage for the geofence state

  bool result = myGNSS.getGeofenceState(currentGeofenceState);

  Serial.print(F("getGeofenceState returned: ")); // Print the combined state
  Serial.print(result); // Get the geofence state

  if (!result) // If getGeofenceState did not return true
  {
    Serial.println(F(".")); // Tidy up
    return; // and go round the loop again
  }

  Serial.print(F(". status is: ")); // Print the status
  Serial.print(currentGeofenceState.status);

  Serial.print(F(". numFences is: ")); // Print the numFences
  Serial.print(currentGeofenceState.numFences);

  Serial.print(F(". combState is: ")); // Print the combined state
  Serial.print(currentGeofenceState.combState);

  if (currentGeofenceState.combState == 0)
  {
    Serial.print(F(" = Unknown"));
    digitalWrite(LED, LOW);
  }
  if (currentGeofenceState.combState == 1)
  {
    Serial.print(F(" = Inside"));
    digitalWrite(LED, HIGH);
  }
  else if (currentGeofenceState.combState == 2)
  {
    Serial.print(F(" = Outside"));
    digitalWrite(LED, LOW);
  }

  Serial.print(F(". The individual states are: ")); // Print the state of each geofence
  for(int i = 0; i < currentGeofenceState.numFences; i++)
  {
    if (i > 0) Serial.print(F(","));
    Serial.print(currentGeofenceState.states[i]);
  }
  Serial.println();

  delay(1000);
}
