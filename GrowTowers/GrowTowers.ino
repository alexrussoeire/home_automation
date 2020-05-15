// NodeMCU 1.0 (ESP-12E Module)
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <QueueList.h>
#include "UserConfig.h"
#include "Arduino.h"
#include "PCF8574.h"

// I2C Relayboard test
// connect VDD to power 5V
// connect GND to power GND
// connect SDA to D2 (I2C DATA)
// connect SCL to D1 (I2C CLOCK)

#define FW_VERSION          "FW_VERSION: 1.0"
#define FW_DATE             "FW_DATE   : 04/04/2020"

//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_PRINT(msg)      Serial.print(msg)
  #define DEBUG_PRINT_CR(msg)   Serial.println(msg)
#else
  #define DEBUG_PRINT(msg)
  #define DEBUG_PRINT_CR(msg)
#endif

#define MAX_RELAYS            (16)
#define RELAY_OFF             (LOW)
#define RELAY_ON              (HIGH)

// Set i2c address for the two IO expanders.
PCF8574 pcf8574_0(0x38);
PCF8574 pcf8574_1(0x39);

typedef enum
{
  RELAY_TOWER_1 = 0,
  RELAY_TOWER_2,
  RELAY_TOWER_3,
  RELAY_TOWER_4,
  RELAY_TOWER_5,
  RELAY_TOWER_6,
  RELAY_TOWER_7,
  RELAY_TOWER_8,
  RELAY_TOWER_9,
  RELAY_TOWER_10,
  RELAY_TOWER_11,
  RELAY_TOWER_12,
  RELAY_TOWER_13,
  RELAY_TOWER_14,
  RELAY_TOWER_15,
  RELAY_TOWER_16
} relay_id_t;

// MQTT Topics for the grow towers to subscribed to.
#define MQTT_TOPIC_TOWER_1  "gh/tower/1"
#define MQTT_TOPIC_TOWER_2  "gh/tower/2"
#define MQTT_TOPIC_TOWER_3  "gh/tower/3"
#define MQTT_TOPIC_TOWER_4  "gh/tower/4"
#define MQTT_TOPIC_TOWER_5  "gh/tower/5"
#define MQTT_TOPIC_TOWER_6  "gh/tower/6"
#define MQTT_TOPIC_TOWER_7  "gh/tower/7"
#define MQTT_TOPIC_TOWER_8  "gh/tower/8"
#define MQTT_TOPIC_TOWER_9  "gh/tower/9"
#define MQTT_TOPIC_TOWER_10 "gh/tower/10"
#define MQTT_TOPIC_TOWER_11 "gh/tower/11"
#define MQTT_TOPIC_TOWER_12 "gh/tower/12"
#define MQTT_TOPIC_TOWER_13 "gh/tower/13"
#define MQTT_TOPIC_TOWER_14 "gh/tower/14"
#define MQTT_TOPIC_TOWER_15 "gh/tower/15"
#define MQTT_TOPIC_TOWER_16 "gh/tower/16"

typedef void (*ptrRelayState)(uint8_t, uint8_t);

typedef struct {
  String topic;
  char payload;
} mqtt_msg_t;

typedef struct {
    String topic;
    ptrRelayState pRelayFn;
    uint8_t channel;
    uint8_t state;
} relay_t;

void SetRelayState(uint8_t channel, uint8_t state);

WiFiClient espClient;
PubSubClient client(espClient);

// create a queue of strings messages.
QueueList <mqtt_msg_t> mqtt_msg_queue;

// Table of relays
relay_t relay_table[MAX_RELAYS] =
{
  // 5v relay output channels
  {MQTT_TOPIC_TOWER_1, SetRelayState, RELAY_TOWER_1, RELAY_OFF},
  {MQTT_TOPIC_TOWER_2, SetRelayState, RELAY_TOWER_2, RELAY_OFF},
  {MQTT_TOPIC_TOWER_3, SetRelayState, RELAY_TOWER_3, RELAY_OFF},
  {MQTT_TOPIC_TOWER_4, SetRelayState, RELAY_TOWER_4, RELAY_OFF},
  {MQTT_TOPIC_TOWER_5, SetRelayState, RELAY_TOWER_5, RELAY_OFF},
  {MQTT_TOPIC_TOWER_6, SetRelayState, RELAY_TOWER_6, RELAY_OFF},
  {MQTT_TOPIC_TOWER_7, SetRelayState, RELAY_TOWER_7, RELAY_OFF},
  {MQTT_TOPIC_TOWER_8, SetRelayState, RELAY_TOWER_8, RELAY_OFF},
  {MQTT_TOPIC_TOWER_9, SetRelayState, RELAY_TOWER_9, RELAY_OFF},
  {MQTT_TOPIC_TOWER_10,SetRelayState, RELAY_TOWER_10,RELAY_OFF},
  {MQTT_TOPIC_TOWER_11,SetRelayState, RELAY_TOWER_11,RELAY_OFF},
  {MQTT_TOPIC_TOWER_12,SetRelayState, RELAY_TOWER_12,RELAY_OFF},
  {MQTT_TOPIC_TOWER_13,SetRelayState, RELAY_TOWER_13,RELAY_OFF},
  {MQTT_TOPIC_TOWER_14,SetRelayState, RELAY_TOWER_14,RELAY_OFF},
  {MQTT_TOPIC_TOWER_15,SetRelayState, RELAY_TOWER_15,RELAY_OFF},
  {MQTT_TOPIC_TOWER_16,SetRelayState, RELAY_TOWER_16,RELAY_OFF},
};

// Initialisation of the IO Expander boards.
void InitRelayBoards(void)
{
  uint8_t index = 0;
  relay_t* prelay = relay_table;

  // Set pinMode to OUTPUT for expander 0.
  pcf8574_0.pinMode(P0, OUTPUT);
  pcf8574_0.pinMode(P1, OUTPUT);
  pcf8574_0.pinMode(P2, OUTPUT);
  pcf8574_0.pinMode(P3, OUTPUT);
  pcf8574_0.pinMode(P4, OUTPUT);
  pcf8574_0.pinMode(P5, OUTPUT);
  pcf8574_0.pinMode(P6, OUTPUT);
  pcf8574_0.pinMode(P7, OUTPUT);
  pcf8574_0.begin();

  // Set pinMode to OUTPUT for expander 1.
  pcf8574_1.pinMode(P0, OUTPUT);
  pcf8574_1.pinMode(P1, OUTPUT);
  pcf8574_1.pinMode(P2, OUTPUT);
  pcf8574_1.pinMode(P3, OUTPUT);
  pcf8574_1.pinMode(P4, OUTPUT);
  pcf8574_1.pinMode(P5, OUTPUT);
  pcf8574_1.pinMode(P6, OUTPUT);
  pcf8574_1.pinMode(P7, OUTPUT);
  pcf8574_1.begin();

  for(index = 0; index < MAX_RELAYS; index++, prelay++){
      prelay->pRelayFn(prelay->channel, RELAY_OFF);
  }
}

// Set a new state to the selected output of the IO expander board.
void SetRelayState(uint8_t channel, uint8_t state) {
  switch(channel)
  {
    case RELAY_TOWER_1:
      pcf8574_0.digitalWrite(P0, state);
      break;
    case RELAY_TOWER_2:
      pcf8574_0.digitalWrite(P1, state);
      break;
    case RELAY_TOWER_3:
      pcf8574_0.digitalWrite(P2, state);
      break;
    case RELAY_TOWER_4:
      pcf8574_0.digitalWrite(P3, state);
      break;
    case RELAY_TOWER_5:
      pcf8574_0.digitalWrite(P4, state);
      break;
    case RELAY_TOWER_6:
      pcf8574_0.digitalWrite(P5, state);
      break;
    case RELAY_TOWER_7:
      pcf8574_0.digitalWrite(P6, state);
      break;
    case RELAY_TOWER_8:
      pcf8574_0.digitalWrite(P7, state);
      break;
    case RELAY_TOWER_9:
      pcf8574_1.digitalWrite(P0, state);
      break;
    case RELAY_TOWER_10:
      pcf8574_1.digitalWrite(P1, state);
      break;
    case RELAY_TOWER_11:
      pcf8574_1.digitalWrite(P2, state);
      break;
    case RELAY_TOWER_12:
      pcf8574_1.digitalWrite(P3, state);
      break;
    case RELAY_TOWER_13:
      pcf8574_1.digitalWrite(P4, state);
      break;
    case RELAY_TOWER_14:
      pcf8574_1.digitalWrite(P5, state);
      break;
    case RELAY_TOWER_15:
      pcf8574_1.digitalWrite(P6, state);
      break;
    case RELAY_TOWER_16:
      pcf8574_1.digitalWrite(P7, state);
      break;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  mqtt_msg_t msg;

  msg.topic = topic;
  msg.payload = payload[0];
  mqtt_msg_queue.push(msg);
}

// Parse the MQTT message.
void ParseMQTTMessage(String topic, byte payload) {
  uint8_t index = 0;
  uint8_t state = 0;
  relay_t* prelay = relay_table;

  // Convert the payload value into a value.
  if('1' == payload) {
    state = RELAY_ON;
  } else {
    state = RELAY_OFF;
  }

  DEBUG_PRINT("topic  : "); DEBUG_PRINT_CR(topic);
  DEBUG_PRINT("payload: "); DEBUG_PRINT_CR(payload);
  DEBUG_PRINT("state  : "); DEBUG_PRINT_CR(state);

  for(index = 0; index < MAX_RELAYS; index++, prelay++){
    if(prelay->topic == topic) {
      // Call the corresponding function.
      prelay->pRelayFn(prelay->channel, state);
    }
  }
}

void CheckNewMQTTMessage(void) {
  mqtt_msg_t msg;

  // pop all the string messages from the queues.
  if(!mqtt_msg_queue.isEmpty()) {
    msg = mqtt_msg_queue.pop();

    // Parse the received MQTT packet.
    ParseMQTTMessage(msg.topic, msg.payload);
  }
}

void reconnect() {
  uint8_t index = 0;
  relay_t* prelay = relay_table;

  // Loop until we're reconnected
  while (!client.connected()) {
    DEBUG_PRINT_CR("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttDeviceID, ssid, password)) {
      DEBUG_PRINT_CR("connected");
      // Subscribe to all the grow tower topics
      for(index = 0; index < MAX_RELAYS; index++, prelay++){
        client.subscribe(prelay->topic.c_str());
        DEBUG_PRINT_CR(prelay->topic.c_str());
      }
    } else {
      DEBUG_PRINT("failed, rc=");
      DEBUG_PRINT(client.state());
      DEBUG_PRINT_CR(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Attempt to connect to WiFi network:
    while (WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINT(".");
      // Connect to WPA/WPA2 network. Change this line if using open or WEP  network:
      // Wait 5 seconds for connection:
      delay(5000);
    }
  
    DEBUG_PRINT_CR("");
    DEBUG_PRINT_CR("WiFi connected");
    DEBUG_PRINT_CR("IP address: ");
    DEBUG_PRINT_CR(WiFi.localIP());   //You can get IP address assigned to ESP

    // Publish message that the GrowTower board is up and running.
    client.publish("gh/tower/status", "On", true);
  }
}

void setup()
{
#ifdef ENABLE_DEBUG
  Serial.begin(9600);
#endif // ENABLE_DEBUG

  // Print the FW version and programming date.
  DEBUG_PRINT_CR();
  DEBUG_PRINT_CR(FW_VERSION);
  DEBUG_PRINT_CR(FW_DATE);

  // Print the board MAC address
  DEBUG_PRINT_CR(WiFi.macAddress());

  // Initialisation of the IO expander boards.
  InitRelayBoards();

  // Start the MQTT server
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{ 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Check for new MQTT message.
  CheckNewMQTTMessage();
}
