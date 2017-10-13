#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "UserConfig.h"

// Connect to the WiFi
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* mqtt_server = MQTT_SERVER;

WiFiClient espClient;
PubSubClient client(espClient);

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

const byte RelayPin0 = 14;
const byte RelayPin1 = 12;
const byte RelayPin2 = 13;
const byte RelayPin3 = 15;

#define MAX_RELAYS 4
const byte RelayPins[MAX_RELAYS] = {14, 12, 13, 15};

void callback(char* topic, byte* payload, unsigned int length) {
    unsigned char index = 0;
    char rxChar = (char)payload[0];
    
    Serial.print("Topic: ");
    Serial.print(topic);
    Serial.println();

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
}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266 Greenhouse Client", "greenhouse", "greenhouse")) {
            Serial.println("connected");
            // ... and subscribe to topic
            client.subscribe(MQTT_TOPIC_VERTICAL_VALVE);
            client.subscribe(MQTT_TOPIC_VERTICAL_PUMP);
            client.subscribe(MQTT_TOPIC_FISH_TANK_HEATER);
            client.subscribe(MQTT_TOPIC_FISH_TANK_COVER);
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
    unsigned int index = 0; 

    Serial.begin(9600);

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    for(index=0; index<MAX_RELAYS; index++) {
        pinMode(RelayPins[index], OUTPUT);
    }
}

void loop()
{
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}
