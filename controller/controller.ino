#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "Adafruit_MCP23008.h"
#include "UserConfig.h"

// I2C Relayboard test
// connect VDD to power 5V
// connect GND to power GND
// connect SDA to analog 4 (I2C DATA)
// connect SCL to analog 5 (I2C CLOCK)

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_MCP23008 mcp;

#define RELAY_ON  (HIGH)
#define RELAY_OFF (LOW)

#define DOUT_0  (0)
#define DOUT_1  (1)
#define DOUT_2  (2)
#define DOUT_3  (3)
#define DOUT_4  (4)
#define DOUT_5  (5)
#define DOUT_6  (6)
#define DOUT_7  (7)
#define DOUT_8  (8)

// Definition of the outputs
#define AON_PLANE           (DOUT_0)
#define CLONER_LIGHT        (DOUT_1)
#define CLONER_PUMP         (DOUT_2)
#define CLONER_HEAT         (DOUT_3)
#define LIGHTS_SIDE         (DOUT_4)
#define LIGHTS_BACK         (DOUT_5)
#define VERTICAL_VALVE      (DOUT_6)
#define VERTICAL_PUMP       (DOUT_7)
#define FISH_TANK_HEATER    (DOUT_8)
#define FISH_TANK_COVER     (DOUT_9)
#define FAN                 (DOUT_10)
#define GROW_BED_PUMP       (DOUT_11)

// Definition of the inputs
#define CLONER_WATER_LEVEL (DIN_12)
#define GROW_BED_WATER_LEVEL_SUMP_1 (DIN_13)
#define GROW_BED_WATER_LEVEL_SUMP_2 (DIN_14)

// MQTT Topics subscribed to.
// Test
#define MQTT_TOPIC_TEST_0 "greenhouse/test/0"
#define MQTT_TOPIC_TEST_1 "greenhouse/test/1"
#define MQTT_TOPIC_TEST_2 "greenhouse/test/2"
#define MQTT_TOPIC_TEST_3 "greenhouse/test/3"
#define MQTT_TOPIC_TEST_4 "greenhouse/test/4"
#define MQTT_TOPIC_TEST_5 "greenhouse/test/5"
#define MQTT_TOPIC_TEST_6 "greenhouse/test/6"
#define MQTT_TOPIC_TEST_7 "greenhouse/test/7"

// Cloner
#define MQTT_TOPIC_CLONER_LIGHT "greenhouse/cloner/light"
#define MQTT_TOPIC_CLONER_PUMP "greenhouse/cloner/pump"
#define MQTT_TOPIC_CLONER_HEATER "greenhouse/cloner/heater"
#define MQTT_TOPIC_CLONER "greenhouse/cloner/water_level"

// Lights
#define MQTT_TOPIC_LIGHTS_SIDE "greenhouse/lights/side"
#define MQTT_TOPIC_LIGHTS_BACK "greenhouse/lights/back"

// Vertical towers
#define MQTT_TOPIC_VERTICAL_VALVE "greenhouse/vertical/valve"
#define MQTT_TOPIC_VERTICAL_PUMP "greenhouse/vertical/pump"

// Fish tank
#define MQTT_TOPIC_FISH_TANK_HEATER "greenhouse/fish_tank/heater"
#define MQTT_TOPIC_FISH_TANK_COVER "greenhouse/fish_tank/cover"

// Fan
#define MQTT_TOPIC_FAN_FAN "greenhouse/fan/fan"

// Grow bed
#define MQTT_TOPIC_GROW_BED_PUMP "greenhouse/grow_bed/pump"
#define MQTT_TOPIC_GROW_BED_WATER_LEVEL_SUMP_1 "greenhouse/grow_bed/water_level_sump_1"
#define MQTT_TOPIC_GROW_BED_WATER_LEVEL_SUMP_2 "greenhouse/grow_bed/water_level_sump_2"

#define DOUT_0  (0)
#define DOUT_1  (1)
#define DOUT_2  (2)
#define DOUT_3  (3)

typedef enum {
  I2C_RELAY_CH_0 = 0,
  I2C_RELAY_CH_1,
  I2C_RELAY_CH_2,
  I2C_RELAY_CH_3,
  I2C_RELAY_CH_4,
  I2C_RELAY_CH_5,
  I2C_RELAY_CH_6,
  I2C_RELAY_CH_7
} i2c_relay_channel_t;

const byte RelayPin0 = 14;
const byte RelayPin1 = 12;
const byte RelayPin2 = 13;
const byte RelayPin3 = 15;

static uint8_t i2cRelay8ChStatus = 0;

#define MAX_RELAYS 4
const byte RelayPins[MAX_RELAYS] = {14, 12, 13, 15};

// Initialisation of the 4 channel 5v relay board.
void setup_4_channels_5v_relay_board()
{
    unsigned int index = 0;

    for(index=0; index<MAX_RELAYS; index++) {
        pinMode(RelayPins[index], OUTPUT);
        digitalWrite(RelayPins[index], HIGH);
    }
}

// Initialisation of the 8 channel I2C 12v relay board.
void setup_8_channels_i2c_12v_relay_board()
{
    mcp.begin(0); // address = 0 (valid: 0-7)
    mcp.writeGPIO(RELAY_OFF); // set OLAT to 0
    mcp.pinMode(0, OUTPUT); // set IODIR to 0
    mcp.pinMode(1, OUTPUT);
    mcp.pinMode(2, OUTPUT);
    mcp.pinMode(3, OUTPUT);
    mcp.pinMode(4, OUTPUT);
    mcp.pinMode(5, OUTPUT);
    mcp.pinMode(6, OUTPUT);
    mcp.pinMode(7, OUTPUT);
}

// Update the I2C relay board channel number with the new value.
void updateI2c8ChRelayBoard(i2c_relay_channel_t channel, uint8_t state)
{
    if (state) {
        i2cRelay8ChStatus |= (1 << channel);
    } else {
        i2cRelay8ChStatus &= (~(1 << channel));
    }
    // Send the updated value to the i2c relay board.
    mcp.writeGPIO(i2cRelay8ChStatus);
}

void callback(char* topic, byte* payload, unsigned int length) {
    unsigned char index = 0;
    char rxChar = (char)payload[0];

    //String msgString = payload;
    
    Serial.print("Topic: ");
    Serial.print(topic);
    Serial.print(" Msg: ");
    Serial.print(rxChar);
    Serial.println();
/*
    if (msgString == "Z1ON")
    {
      digitalWrite(channel1, HIGH);
      delay(250);
    }
    else if (msgString == "Z1OFF")
    {
      digitalWrite(channel1, LOW);
      delay(250);
    }
*/
    if (strcmp(topic,MQTT_TOPIC_VERTICAL_VALVE)==0){
        if (rxChar == '1') {
            digitalWrite(RelayPin0, HIGH);
        } else {
            digitalWrite(RelayPin0, LOW);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_VERTICAL_PUMP)==0){
        if (rxChar == '1') {
            digitalWrite(RelayPin1, HIGH);
        } else {
            digitalWrite(RelayPin1, LOW);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_FISH_TANK_HEATER)==0){
        if (rxChar == '1') {
            digitalWrite(RelayPin2, HIGH);
        } else {
            digitalWrite(RelayPin2, LOW);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_FISH_TANK_COVER)==0){
        if (rxChar == '1') {
            digitalWrite(RelayPin3, HIGH);
        } else {
            digitalWrite(RelayPin3, LOW);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_0)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_0, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_0, RELAY_OFF);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_1)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_1, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_1, RELAY_OFF);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_2)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_2, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_2, RELAY_OFF);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_3)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_3, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_3, RELAY_OFF);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_4)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_4, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_4, RELAY_OFF);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_5)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_5, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_5, RELAY_OFF);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_6)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_6, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_6, RELAY_OFF);
        }
    }

    if (strcmp(topic,MQTT_TOPIC_TEST_7)==0){
        if (rxChar == '1') {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_7, RELAY_ON);
        } else {
            updateI2c8ChRelayBoard(I2C_RELAY_CH_7, RELAY_OFF);
        }
    }
}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //if (client.connect("ESP8266 Greenhouse Client", "greenhouse", "greenhouse")) {
    if (client.connect(mqttDeviceID)) {
            Serial.println("connected");
            // ... and subscribe to topic
            client.subscribe(MQTT_TOPIC_VERTICAL_VALVE);
            client.subscribe(MQTT_TOPIC_VERTICAL_PUMP);
            client.subscribe(MQTT_TOPIC_FISH_TANK_HEATER);
            client.subscribe(MQTT_TOPIC_FISH_TANK_COVER);
            client.subscribe(MQTT_TOPIC_TEST_0);
/*
            client.subscribe(MQTT_TOPIC_TEST_1);
            client.subscribe(MQTT_TOPIC_TEST_2);
            client.subscribe(MQTT_TOPIC_TEST_3);
            client.subscribe(MQTT_TOPIC_TEST_4);
            client.subscribe(MQTT_TOPIC_TEST_5);
            client.subscribe(MQTT_TOPIC_TEST_6);
            client.subscribe(MQTT_TOPIC_TEST_7);
*/
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(9600);

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    // Initialisation of the 4 channel 5v relay board.
    setup_4_channels_5v_relay_board();

    // Initialisation of the 8 channel I2C 12v relay board.
    setup_8_channels_i2c_12v_relay_board();
}

void loop()
{
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}
