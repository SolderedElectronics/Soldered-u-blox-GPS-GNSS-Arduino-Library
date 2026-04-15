/**
 **************************************************
 * @file        GetLeapSecondInfo.ino
 * @brief       Retrieves leap second event information and maps it to an
 *              SNTP Leap Indicator from a u-blox GNSS module.
 *
 * @details     Calls getLeapIndicator() to obtain the SNTP-style leap
 *              indicator (no warning, +1 s, -1 s, or alarm), and
 *              getCurrentLeapSeconds() to get the total number of leap
 *              seconds since the GPS epoch (6 Jan 1980). Both the NTP LI
 *              value and the time to the next (or since the last) leap
 *              second event are printed every second.
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
 * - Unix epoch (rounded and exact with microseconds), NTP Leap Indicator
 *   value with description, time to/since leap second event, and leap
 *   second count with source, printed once per second.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      UT2UH @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h>
Soldered_GNSS myGNSS;

typedef enum { 
  LI_NO_WARNING,      //Time leaping not scheduled
  LI_LAST_MINUTE_61_SEC,  //Last minute has 61 seconds
  LI_LAST_MINUTE_59_SEC,  //Last minute has 59 seconds
  LI_ALARM_CONDITION    //The NTP server's clock not synchronized
} ntp_LI_e;


long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.
  
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("SparkFun u-blox Example");

  Wire.begin();
  
  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  // Uncomment the next line if you need to completely reset your module
  //myGNSS.factoryDefault(); delay(5000); // Reset everything and wait while the module restarts

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfiguration();        //Optional: Save the current settings to flash and BBR

  Serial.println(F("Compare Unix Epoch given with reference one from https://www.epochconverter.com/"));
  
}

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer

    // getUnixEpoch marks the PVT data as stale so you will get Unix time and PVT time on alternate seconds

    uint32_t us;  //microseconds returned by getUnixEpoch()
    uint32_t epoch = myGNSS.getUnixEpoch();
    Serial.print(F("Unix Epoch rounded: "));
    Serial.print(epoch, DEC);    
    epoch = myGNSS.getUnixEpoch(us);
    Serial.print(F("  Exact Unix Epoch: "));
    Serial.print(epoch, DEC);
    Serial.print(F("  micros: "));
    Serial.println(us, DEC);
    int32_t timeToLeapSecEvent;
    ntp_LI_e leapIndicator = (ntp_LI_e)myGNSS.getLeapIndicator(timeToLeapSecEvent);
    Serial.print(F("NTP LI: "));
    Serial.print(leapIndicator, DEC);
    switch (leapIndicator){
      case LI_NO_WARNING:
        Serial.print(F(" - No event scheduled"));
        break;
      case LI_LAST_MINUTE_61_SEC:
        Serial.print(F(" - last minute will end at 23:60"));
        break;
      case LI_LAST_MINUTE_59_SEC:
        Serial.print(F(" - last minute will end at 23:58"));
        break; 
      case LI_ALARM_CONDITION:
      default:
        Serial.print(F(" - Unknown (clock not synchronized)"));
        break; 
    }
    if (timeToLeapSecEvent < 0)
    {
      Serial.print(F(". Time since the last leap second event: "));
      Serial.println(timeToLeapSecEvent * -1, DEC);
    }
    else
    {
      Serial.print(F(". Time to the next leap second event: "));
      Serial.println(timeToLeapSecEvent, DEC);
    }

    sfe_ublox_ls_src_e leapSecSource;
    Serial.print(F("Leap seconds since GPS Epoch (Jan 6th, 1980): "));
    Serial.print(myGNSS.getCurrentLeapSeconds(leapSecSource), DEC);
    switch (leapSecSource){
      case SFE_UBLOX_LS_SRC_DEFAULT:
        Serial.print(F(" - hardcoded"));
        break;
      case SFE_UBLOX_LS_SRC_GLONASS:
        Serial.print(F(" - derived from GPS and GLONASS time difference"));
        break;
      case SFE_UBLOX_LS_SRC_GPS:
        Serial.print(F(" - according to GPS"));
        break; 
      case SFE_UBLOX_LS_SRC_SBAS:
        Serial.print(F(" - according to SBAS"));
        break;
      case SFE_UBLOX_LS_SRC_BEIDOU:
        Serial.print(F(" - according to BeiDou"));
        break;
      case SFE_UBLOX_LS_SRC_GALILEO:
        Serial.print(F(" - according to Galileo"));
        break;
      case SFE_UBLOX_LS_SRC_AIDED:
        Serial.print(F(" - last minute will end at 23:58"));
        break; 
      case SFE_UBLOX_LS_SRC_CONFIGURED:
        Serial.print(F(" - as configured)"));
        break;
      case SFE_UBLOX_LS_SRC_UNKNOWN:
      default:
        Serial.print(F(" - source unknown"));
        break;
    }
    Serial.println();
  }
  Serial.println();
}
