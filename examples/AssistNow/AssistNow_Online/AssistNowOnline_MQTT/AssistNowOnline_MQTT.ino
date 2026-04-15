/**
 **************************************************
 * @file        AssistNowOnline_MQTT.ino
 * @brief       Receive AssistNow Online MGA data from u-blox PointPerfect via MQTT and push it to a ZED-F9x.
 *
 * @details     This example uses ESP32 WiFi to connect to the u-blox PointPerfect
 *              MQTT broker (AWS IoT) using TLS certificates. It subscribes to the
 *              AssistNow MGA topic and forwards received data directly to a ZED-F9x
 *              GNSS module over I2C using pushRawData. The connection times out after
 *              10 seconds of inactivity. Press any key in Serial Monitor to start or
 *              stop the MQTT client.
 *
 *              For more information about MQTT, SPARTN and PointPerfect Correction
 *              Services see: https://www.u-blox.com/en/product/pointperfect
 *
 * Requirements:
 * - Board:      ESP32
 * - Hardware:   u-blox ZED-F9x GNSS module
 * - Extra:      WiFi; valid u-blox Thingstream / PointPerfect account with
 *               AWS IoT certificates (secrets.h)
 *
 * How to use:
 * 1) Update secrets.h with your WiFi credentials, AWS IoT endpoint, port,
 *    client ID, MQTT topic, CA cert, device cert, and private key.
 * 2) Connect the ZED-F9x to your board via I2C.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor at 115200 baud, then press any key to start the MQTT client.
 *
 * Expected output:
 * - WiFi IP address, MQTT broker connection confirmation.
 * - Topic name and "Pushing data" messages as MGA packets arrive.
 * - "Timeout. Disconnecting..." after 10 s of no data.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      Paul Clark @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoMqttClient.h> // Click here to get the library: http://librarymanager/All#ArduinoMqttClient
#include "secrets.h"

#include <Soldered-GNSS.h> // Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS
Soldered_GNSS myGNSS;
    
//Global variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastReceived_ms = 0; //5 RTCM messages take approximately ~300ms to arrive at 115200bps
int maxTimeBeforeHangup_ms = 10000; //If we fail to get a complete RTCM frame after 10s, then disconnect from caster
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("PointPerfect AssistNow testing"));

  Wire.begin(); //Start I2C

  if (myGNSS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("u-blox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
 
  Serial.println(F("u-blox module connected"));
  myGNSS.setI2COutput(COM_TYPE_UBX); //Turn off NMEA noise
  myGNSS.setPortInput(COM_PORT_I2C, COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_SPARTN);
   
  myGNSS.setNavigationFrequency(1); //Set output in Hz.
  
  Serial.print(F("Connecting to local WiFi"));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.print(F("WiFi connected with IP: "));
  Serial.println(WiFi.localIP());
  
  while (Serial.available()) Serial.read();
}

void loop()
{
  if (Serial.available())
  {
    beginClient();
    while (Serial.available()) Serial.read(); //Empty buffer of any newline chars
  }

  Serial.println(F("Press any key to start MQTT Client."));

  delay(1000);
}

WiFiClientSecure wifiClient = WiFiClientSecure();
MqttClient mqttClient(wifiClient);

void mqttMessageHandler(int messageSize)
{
  const uint16_t mqttLimit = 512;
  uint8_t *mqttData = new uint8_t[mqttLimit]; // Allocate memory to hold the MQTT data
  if (mqttData == NULL)
  {
    Serial.println(F("Memory allocation for mqttData failed!"));
    return;
  }

  Serial.print(F("Pushing data from "));
  Serial.print(mqttClient.messageTopic());
  Serial.println(F(" topic to ZED"));

  while (mqttClient.available())
  {
    uint16_t mqttCount = 0;

    while (mqttClient.available())
    {
      char ch = mqttClient.read();
      //Serial.write(ch); //Pipe to serial port is fine but beware, it's a lot of binary data
      mqttData[mqttCount++] = ch;
    
      if (mqttCount == mqttLimit)
        break;
    }

    if (mqttCount > 0)
    {
      //Push KEYS or SPARTN data to GNSS module over I2C
      myGNSS.pushRawData(mqttData, mqttCount, false);
      lastReceived_ms = millis();
    }
  }

  delete[] mqttData;
}

//Connect to MQTT broker, receive MGA, and push to ZED module over I2C
void beginClient()
{
  Serial.println(F("Subscribing to Broker. Press key to stop"));
  delay(10); //Wait for any serial to arrive
  while (Serial.available()) Serial.read(); //Flush

  while (Serial.available() == 0)
  {
    //Connect if we are not already
    if (wifiClient.connected() == false)
    {
      // Connect to AWS IoT
      wifiClient.setCACert(AWS_CERT_CA);
      wifiClient.setCertificate(AWS_CERT_CRT);
      wifiClient.setPrivateKey(AWS_CERT_PRIVATE);
      mqttClient.setId(MQTT_CLIENT_ID);
      mqttClient.setKeepAliveInterval(60*1000);
      mqttClient.setConnectionTimeout( 5*1000);
      if (!mqttClient.connect(AWS_IOT_ENDPOINT, AWS_IOT_PORT)) {
        Serial.print(F("MQTT connection failed! Error code = "));
        Serial.println(mqttClient.connectError());
        return;
      } else {
        Serial.println(F("You're connected to the PointPerfect MQTT broker: "));
        Serial.println(AWS_IOT_ENDPOINT);
        // Subscribe to MQTT and register a callback
        Serial.println(F("Subscribe to Topics")); 
        mqttClient.onMessage(mqttMessageHandler);
        mqttClient.subscribe(MQTT_TOPIC_ASSISTNOW);
        lastReceived_ms = millis();
      } //End attempt to connect
    } //End connected == false
    else {
      mqttClient.poll();
    }
    //Close socket if we don't have new data for 10s
    if (millis() - lastReceived_ms > maxTimeBeforeHangup_ms)
    {
      Serial.println(F("Timeout. Disconnecting..."));
      if (mqttClient.connected() == true)
        mqttClient.stop();
      return;
    }

    delay(10);
  }

  Serial.println(F("User pressed a key"));
  Serial.println(F("Disconnecting..."));
  wifiClient.stop();
}
