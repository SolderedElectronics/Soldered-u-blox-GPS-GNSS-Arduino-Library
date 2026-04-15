/**
 **************************************************
 * @file        BaseWithLCD.ino
 * @brief       Configure a ZED-F9P as an RTK base station and show survey-in progress on an LCD.
 *
 * @details     Performs all steps to set up a ZED-F9P as an RTK base station:
 *              starts a Survey-In, waits for 2 m accuracy over 300 s, then
 *              enables six RTCM messages and begins outputting RTCM data over
 *              I2C. Progress and status are displayed on a SerLCD I2C display
 *              and on the Serial Monitor.
 *
 * Requirements:
 * - Board:      Any Arduino-compatible board with I2C
 * - Hardware:   u-blox ZED-F9P
 * - Extra:      SerLCD I2C display (address 0x72)
 *
 * How to use:
 * 1) Connect the ZED-F9P and SerLCD to the I2C bus.
 * 2) Upload the sketch.
 * 3) Open Serial Monitor at 115200 baud or watch the LCD.
 *
 * Expected output:
 * - Survey-In progress (mean accuracy, observation count) on LCD and Serial.
 * - "Survey-in complete!" when the survey finishes.
 * - RTCM output begins automatically after survey-in.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Nathan Seidle @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        2019-01-09
 **************************************************/

#define STAT_LED 13

#include <Wire.h> //Needed for I2C to GNSS

#include <Soldered-GNSS.h> //Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS
Soldered_GNSS myGNSS;

#include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
SerLCD lcd;         // Initialize the library with default I2C address 0x72

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println(F("u-blox GNSS I2C Test"));

  Wire.begin();

  pinMode(STAT_LED, OUTPUT);
  digitalWrite(STAT_LED, LOW);

  lcd.begin(Wire);            //Set up the LCD for Serial communication at 9600bps
  lcd.setBacklight(0x4B0082); //indigo, a kind of dark purplish blue
  lcd.clear();
  lcd.print(F("LCD Ready"));

  myGNSS.begin(Wire);
  if (myGNSS.isConnected() == false)
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    lcd.setCursor(0, 1);
    lcd.print(F("No GNSS detected"));
    while (1)
      ;
  }

  Wire.setClock(400000); //Increase I2C clock speed to 400kHz

  lcd.setCursor(0, 1);
  lcd.print("GNSS Detected");

  myGNSS.setI2COutput(COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_RTCM3); // Ensure RTCM3 is enabled
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save the communications port settings to flash and BBR

  bool response = true;
  response &= myGNSS.enableRTCMmessage(UBX_RTCM_1005, COM_PORT_I2C, 1); //Enable message 1005 to output through I2C port, message every second
  response &= myGNSS.enableRTCMmessage(UBX_RTCM_1074, COM_PORT_I2C, 1);
  response &= myGNSS.enableRTCMmessage(UBX_RTCM_1084, COM_PORT_I2C, 1);
  response &= myGNSS.enableRTCMmessage(UBX_RTCM_1094, COM_PORT_I2C, 1);
  response &= myGNSS.enableRTCMmessage(UBX_RTCM_1124, COM_PORT_I2C, 1);
  response &= myGNSS.enableRTCMmessage(UBX_RTCM_1230, COM_PORT_I2C, 10); //Enable message every 10 seconds
  if (response == true)
  {
    Serial.println(F("RTCM messages enabled"));
  }
  else
  {
    Serial.println(F("RTCM failed to enable. Are you sure you have an ZED-F9P? Freezing."));
    while (1)
      ; //Freeze
  }

  //Check if Survey is in Progress before initiating one
  // From v2.0, the data from getSurveyStatus (UBX-NAV-SVIN) is returned in UBX_NAV_SVIN_t packetUBXNAVSVIN
  // Please see u-blox_structs.h for the full definition of UBX_NAV_SVIN_t
  // You can either read the data from packetUBXNAVSVIN directly
  // or can use the helper functions: getSurveyInActive; getSurveyInValid; getSurveyInObservationTime; and getSurveyInMeanAccuracy
  response = myGNSS.getSurveyStatus(2000); //Query module for SVIN status with 2000ms timeout (request can take a long time)
  if (response == false)
  {
    Serial.println(F("Failed to get Survey In status. Freezing."));
    while (1)
      ; //Freeze
  }

  if (myGNSS.getSurveyInActive() == true) // Use the helper function
  {
    Serial.print(F("Survey already in progress."));
    lcd.setCursor(0, 2);
    lcd.print(F("Survey already going"));
  }
  else
  {
    //Start survey
    response = myGNSS.enableSurveyMode(60, 5.000); //Enable Survey in, 60 seconds, 5.0m
    if (response == false)
    {
      Serial.println(F("Survey start failed"));
      lcd.setCursor(0, 3);
      lcd.print(F("Survey start failed. Freezing."));
      while (1)
        ;
    }
    Serial.println(F("Survey started. This will run until 60s has passed and less than 5m accuracy is achieved."));
  }

  while (Serial.available())
    Serial.read(); //Clear buffer

  lcd.clear();
  lcd.print(F("Survey in progress"));

  //Begin waiting for survey to complete
  while (myGNSS.getSurveyInValid() == false) // Call the helper function
  {
    if (Serial.available())
    {
      byte incoming = Serial.read();
      if (incoming == 'x')
      {
        //Stop survey mode
        response = myGNSS.disableSurveyMode(); //Disable survey
        Serial.println(F("Survey stopped"));
        break;
      }
    }

    // From v2.0, the data from getSurveyStatus (UBX-NAV-SVIN) is returned in UBX_NAV_SVIN_t packetUBXNAVSVIN
    // Please see u-blox_structs.h for the full definition of UBX_NAV_SVIN_t
    // You can either read the data from packetUBXNAVSVIN directly
    // or can use the helper functions: getSurveyInActive; getSurveyInValid; getSurveyInObservationTime; and getSurveyInMeanAccuracy
    response = myGNSS.getSurveyStatus(2000); //Query module for SVIN status with 2000ms timeout (req can take a long time)
    if (response == true)
    {
      Serial.print(F("Press x to end survey - "));
      Serial.print(F("Time elapsed: "));
      Serial.print((String)myGNSS.getSurveyInObservationTime()); // Call the helper function

      lcd.setCursor(0, 1);
      lcd.print(F("Elapsed: "));
      lcd.print((String)myGNSS.getSurveyInObservationTime()); // Call the helper function

      Serial.print(F(" Accuracy: "));
      Serial.print((String)myGNSS.getSurveyInMeanAccuracy()); // Call the helper function
      Serial.println();

      lcd.setCursor(0, 2);
      lcd.print(F("Accuracy: "));
      lcd.print((String)myGNSS.getSurveyInMeanAccuracy()); // Call the helper function
    }
    else
    {
      Serial.println(F("SVIN request failed"));
    }

    delay(1000);
  }
  Serial.println(F("Survey valid!"));

  Serial.println(F("Base survey complete! RTCM now broadcasting."));
  lcd.clear();
  lcd.print(F("Transmitting RTCM"));

  myGNSS.setI2COutput(COM_TYPE_UBX | COM_TYPE_RTCM3); //Set the I2C port to output UBX and RTCM sentences (not really an option, turns on NMEA as well)
  
}

void loop()
{
  myGNSS.checkUblox(); //See if new data is available. Process bytes as they come in.

  //Do anything you want. Call checkUblox() every second. ZED-F9P has TX buffer of 4k bytes.

  delay(250); //Don't pound too hard on the I2C bus
}

//This function gets called from the SparkFun u-blox Arduino Library.
//As each RTCM byte comes in you can specify what to do with it
//Useful for passing the RTCM correction data to a radio, Ntrip broadcaster, etc.
void SFE_UBLOX_GNSS::processRTCM(uint8_t incoming)
{
  //Let's just pretty-print the HEX values for now
  if (myGNSS.rtcmFrameCounter % 16 == 0)
    Serial.println();
  Serial.print(" ");
  if (incoming < 0x10)
    Serial.print("0");
  Serial.print(incoming, HEX);
}
