/**
 **************************************************
 * @file        ModuleInfo.ino
 * @brief       Reads and prints full module hardware/software version and
 *              extension strings using a custom UBX-MON-VER command.
 *
 * @details     Extends the library class to add a getModuleInfo() method
 *              that sends UBX-MON-VER as a custom packet and parses the
 *              response into software version, hardware version, and up to
 *              ten extension strings. MAX_PAYLOAD_SIZE is overridden to 384
 *              bytes to accommodate the full 348-byte response.
 *              setPacketCfgPayloadSize() is called before begin() to avoid
 *              extra RAM allocation on ATmega328P boards.
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
 * - Software version, hardware version, number of extensions, and each
 *   extension string printed once at startup.
 *
 * Notes:
 * - The minfo structure uses 341 bytes of RAM; on ATmega328P (UNO) this
 *   leaves very little free heap. MAX_PAYLOAD_SIZE must be defined before
 *   the library include to take effect.
 *
 * Support:      https://docs.soldered.com/
 *
 * @author      mayopan @ SparkFun Electronics
 * @author      Modified by Soldered
 * @date        14-04-2026
 **************************************************/

#include <Wire.h> //Needed for I2C to GNSS

#define MAX_PAYLOAD_SIZE 384 // Override MAX_PAYLOAD_SIZE for getModuleInfo which can return up to 348 bytes

#include <Soldered-GNSS.h>

// Extend the class for getModuleInfo
class SFE_UBLOX_GPS_ADD : public SFE_UBLOX_GNSS
{
public:
    bool getModuleInfo(uint16_t maxWait = 1100); //Queries module, texts

    struct minfoStructure // Structure to hold the module info (uses 341 bytes of RAM)
    {
        char swVersion[30];
        char hwVersion[10];
        uint8_t extensionNo = 0;
        char extension[10][30];
    } minfo;
};

SFE_UBLOX_GPS_ADD myGNSS;

void setup()
{
    Serial.begin(115200); // You may need to increase this for high navigation rates!
    while (!Serial)
        ; //Wait for user to open terminal
    Serial.println(F("SparkFun u-blox Example"));

    Wire.begin();

    //myGNSS.enableDebugging(); // Uncomment this line to enable debug messages

    // setPacketCfgPayloadSize tells the library how many bytes our customPayload can hold.
    // If we call it after the .begin, the library will attempt to resize the existing 256 byte payload buffer
    // by creating a new buffer, copying across the contents of the old buffer, and then delete the old buffer.
    // This uses a lot of RAM and causes the code to fail on the ATmega328P. (We are also allocating another 341 bytes for minfo.)
    // To keep the code ATmega328P compliant - don't call setPacketCfgPayloadSize after .begin. Call it here instead.    
    myGNSS.setPacketCfgPayloadSize(MAX_PAYLOAD_SIZE);

    if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
    {
        Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
        while (1)
            ;
    }

    myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)

    Serial.print(F("Polling module info"));
    if (myGNSS.getModuleInfo(1100) == false) // Try to get the module info
    {
        Serial.print(F("getModuleInfo failed! Freezing..."));
        while (1)
            ;
    }

    Serial.println();
    Serial.println();
    Serial.println(F("Module Info : "));
    Serial.print(F("Soft version: "));
    Serial.println(myGNSS.minfo.swVersion);
    Serial.print(F("Hard version: "));
    Serial.println(myGNSS.minfo.hwVersion);
    Serial.print(F("Extensions:"));
    Serial.println(myGNSS.minfo.extensionNo);
    for (int i = 0; i < myGNSS.minfo.extensionNo; i++)
    {
        Serial.print("  ");
        Serial.println(myGNSS.minfo.extension[i]);
    }
    Serial.println();
    Serial.println(F("Done!"));
}

void loop()
{
}

bool SFE_UBLOX_GPS_ADD::getModuleInfo(uint16_t maxWait)
{
    myGNSS.minfo.hwVersion[0] = 0;
    myGNSS.minfo.swVersion[0] = 0;
    for (int i = 0; i < 10; i++)
        myGNSS.minfo.extension[i][0] = 0;
    myGNSS.minfo.extensionNo = 0;

    // Let's create our custom packet
    uint8_t customPayload[MAX_PAYLOAD_SIZE]; // This array holds the payload data bytes

    // setPacketCfgPayloadSize tells the library how many bytes our customPayload can hold.
    // If we call it here, after the .begin, the library will attempt to resize the existing 256 byte payload buffer
    // by creating a new buffer, copying across the contents of the old buffer, and then delete the old buffer.
    // This uses a lot of RAM and causes the code to fail on the ATmega328P. (We are also allocating another 341 bytes for minfo.)
    // To keep the code ATmega328P compliant - don't call setPacketCfgPayloadSize here. Call it before .begin instead.
    //myGNSS.setPacketCfgPayloadSize(MAX_PAYLOAD_SIZE);

    // The next line creates and initialises the packet information which wraps around the payload
    ubxPacket customCfg = {0, 0, 0, 0, 0, customPayload, 0, 0, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED, SFE_UBLOX_PACKET_VALIDITY_NOT_DEFINED};

    // The structure of ubxPacket is:
    // uint8_t cls           : The message Class
    // uint8_t id            : The message ID
    // uint16_t len          : Length of the payload. Does not include cls, id, or checksum bytes
    // uint16_t counter      : Keeps track of number of overall bytes received. Some responses are larger than 255 bytes.
    // uint16_t startingSpot : The counter value needed to go past before we begin recording into payload array
    // uint8_t *payload      : The payload
    // uint8_t checksumA     : Given to us by the module. Checked against the rolling calculated A/B checksums.
    // uint8_t checksumB
    // sfe_ublox_packet_validity_e valid            : Goes from NOT_DEFINED to VALID or NOT_VALID when checksum is checked
    // sfe_ublox_packet_validity_e classAndIDmatch  : Goes from NOT_DEFINED to VALID or NOT_VALID when the Class and ID match the requestedClass and requestedID

    // sendCommand will return:
    // SFE_UBLOX_STATUS_DATA_RECEIVED if the data we requested was read / polled successfully
    // SFE_UBLOX_STATUS_DATA_SENT     if the data we sent was writted successfully (ACK'd)
    // Other values indicate errors. Please see the sfe_ublox_status_e enum for further details.

    // Referring to the u-blox M8 Receiver Description and Protocol Specification we see that
    // the module information can be read using the UBX-MON-VER message. So let's load our
    // custom packet with the correct information so we can read (poll / get) the module information.

    customCfg.cls = UBX_CLASS_MON; // This is the message Class
    customCfg.id = UBX_MON_VER;    // This is the message ID
    customCfg.len = 0;             // Setting the len (length) to zero let's us poll the current settings
    customCfg.startingSpot = 0;    // Always set the startingSpot to zero (unless you really know what you are doing)

    // Now let's send the command. The module info is returned in customPayload

    if (sendCommand(&customCfg, maxWait) != SFE_UBLOX_STATUS_DATA_RECEIVED)
        return (false); //If command send fails then bail

    // Now let's extract the module info from customPayload

    uint16_t position = 0;
    for (int i = 0; i < 30; i++)
    {
        minfo.swVersion[i] = customPayload[position];
        position++;
    }
    for (int i = 0; i < 10; i++)
    {
        minfo.hwVersion[i] = customPayload[position];
        position++;
    }

    while (customCfg.len >= position + 30)
    {
        for (int i = 0; i < 30; i++)
        {
            minfo.extension[minfo.extensionNo][i] = customPayload[position];
            position++;
        }
        minfo.extensionNo++;
        if (minfo.extensionNo > 9)
            break;
    }

    return (true); //Success!
}
