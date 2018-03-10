#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <QueueList.h>
#include "Adafruit_MCP23008.h"
#include "UserConfig.h"

// I2C Relayboard test
// connect VDD to power 5V
// connect GND to power GND
// connect SDA to D2 (I2C DATA)
// connect SCL to D1 (I2C CLOCK)

//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_PRINT(msg)    Serial.print(msg)
  #define DEBUG_PRINT_CR()    Serial.println()
#else
  #define DEBUG_PRINT(msg)
  #define DEBUG_PRINT_CR()
#endif

#define MAX_I2C_RELAY_BOARDS  (2)
#define MAX_RELAYS            (17)
#define MAX_5V_RELAYS         (4)
#define RELAY_5V_OFF          (HIGH)
#define RELAY_5V_ON           (LOW)
#define RELAY_OFF             (0)
#define RELAY_ON              (1)

// Definition of the inputs
#define CLONER_WATER_LEVEL (DIN_12)
#define GROW_BED_WATER_LEVEL_SUMP_1 (DIN_13)
#define GROW_BED_WATER_LEVEL_SUMP_2 (DIN_14)

typedef enum
{
  // 5v relay output channels
  RELAY_5V_MISC_PSU_220V_12V = D5,
  RELAY_5V_VERTICAL_PUMP = D6,
  RELAY_5V_VERTICAL_VALVE = D7,
  RELAY_5V_GROW_BED_PUMP = D8,
  // I2C relay output channels
  RELAY_I2C_CLONER_LIGHT = 0,
  RELAY_I2C_CLONER_PUMP,
  RELAY_I2C_CLONER_HEATER,
  RELAY_I2C_LIGHTS_SIDE,
  RELAY_I2C_LIGHTS_BACK,
  RELAY_I2C_MISC_FAN,
  RELAY_I2C_FISH_TANK_PC,
  RELAY_I2C_FISH_TANK_HEATER,
  RELAY_I2C_FISH_TANK_COVER,
  RELAY_I2C_WATERFALL_PUMP,
  RELAY_I2C_WATERFALL_LIGHT,
  RELAY_I2C_LIGHTS_PIR,
  RELAY_I2C_MISC_CAMERA,
  RELAY_I2C_NC_1,
  RELAY_I2C_NC_2,
  RELAY_I2C_NC_3
} relay_id_t;

// MQTT Topics for the greenhouse subscribed to.

// Grow bed
#define MQTT_TOPIC_GROW_BED_PUMP "gh/grow_bed/pump"

// Vertical towers
#define MQTT_TOPIC_VERTICAL_PUMP "gh/vertical/pump"
#define MQTT_TOPIC_VERTICAL_VALVE "gh/vertical/valve"

// Cloner
#define MQTT_TOPIC_CLONER_LIGHT "gh/cloner/light"
#define MQTT_TOPIC_CLONER_PUMP "gh/cloner/pump"
#define MQTT_TOPIC_CLONER_HEATER "gh/cloner/heater"
#define MQTT_TOPIC_CLONER_WATER_LEVEL "gh/cloner/water_level"

// Lights
#define MQTT_TOPIC_LIGHTS_SIDE "gh/lights/side"
#define MQTT_TOPIC_LIGHTS_BACK "gh/lights/back"
#define MQTT_TOPIC_LIGHTS_PIR "gh/lights/pir"

// Fish tank
#define MQTT_TOPIC_FISH_TANK_HEATER "gh/fish_tank/heater"
#define MQTT_TOPIC_FISH_TANK_COVER "gh/fish_tank/cover"
#define MQTT_TOPIC_FISH_TANK_PC "gh/fish_tank/pc"

// Miscellaneous
#define MQTT_TOPIC_MISC_FAN "gh/mics/fan"
#define MQTT_TOPIC_MISC_PSU_220V_12V "gh/mics/psu_220v_12v"
#define MQTT_TOPIC_MISC_CAMERA "gh/mics/camera"

// Waterfall
#define MQTT_TOPIC_WATERFALL_PUMP "gh/waterfall/pump"
#define MQTT_TOPIC_WATERFALL_LIGHT "gh/waterfall/light"

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

void SetRelay5vState(uint8_t channel, uint8_t state);
void SetRelayI2cState(uint8_t channel, uint8_t state);

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_MCP23008 mcp[MAX_I2C_RELAY_BOARDS];

// create a queue of strings messages.
QueueList <mqtt_msg_t> mqtt_msg_queue;

// Indicate that the power is ON.
uint8_t power_on = false;

// Table of relays
relay_t relay_table[MAX_RELAYS] =
{
  // 5v relay output channels
  {MQTT_TOPIC_MISC_PSU_220V_12V, SetRelay5vState, RELAY_5V_MISC_PSU_220V_12V, RELAY_OFF},
  {MQTT_TOPIC_VERTICAL_PUMP, SetRelay5vState, RELAY_5V_VERTICAL_PUMP, RELAY_OFF},
  {MQTT_TOPIC_VERTICAL_VALVE, SetRelay5vState, RELAY_5V_VERTICAL_VALVE, RELAY_OFF},
  {MQTT_TOPIC_GROW_BED_PUMP, SetRelay5vState, RELAY_5V_GROW_BED_PUMP, RELAY_OFF},
  // I2C relay output channels
  {MQTT_TOPIC_CLONER_LIGHT, SetRelayI2cState, RELAY_I2C_CLONER_LIGHT, RELAY_OFF},
  {MQTT_TOPIC_CLONER_PUMP, SetRelayI2cState, RELAY_I2C_CLONER_PUMP, RELAY_OFF},
  {MQTT_TOPIC_CLONER_HEATER, SetRelayI2cState, RELAY_I2C_CLONER_HEATER, RELAY_OFF},
  {MQTT_TOPIC_LIGHTS_SIDE, SetRelayI2cState, RELAY_I2C_LIGHTS_SIDE, RELAY_OFF},
  {MQTT_TOPIC_LIGHTS_BACK, SetRelayI2cState, RELAY_I2C_LIGHTS_BACK, RELAY_OFF},
  {MQTT_TOPIC_MISC_FAN, SetRelayI2cState, RELAY_I2C_MISC_FAN, RELAY_OFF},
  {MQTT_TOPIC_FISH_TANK_PC, SetRelayI2cState, RELAY_I2C_FISH_TANK_PC, RELAY_OFF},
  {MQTT_TOPIC_FISH_TANK_HEATER, SetRelayI2cState, RELAY_I2C_FISH_TANK_HEATER, RELAY_OFF},
  {MQTT_TOPIC_FISH_TANK_COVER, SetRelayI2cState, RELAY_I2C_FISH_TANK_COVER, RELAY_OFF},
  {MQTT_TOPIC_WATERFALL_PUMP, SetRelayI2cState, RELAY_I2C_WATERFALL_PUMP, RELAY_OFF},
  {MQTT_TOPIC_WATERFALL_LIGHT, SetRelayI2cState, RELAY_I2C_WATERFALL_LIGHT, RELAY_OFF},
  {MQTT_TOPIC_LIGHTS_PIR, SetRelayI2cState, RELAY_I2C_LIGHTS_PIR, RELAY_OFF},
  {MQTT_TOPIC_MISC_CAMERA, SetRelayI2cState, RELAY_I2C_MISC_CAMERA, RELAY_OFF}
};

// Initialisation of the 5v relay board.
void Init5vRelayBoard(void)
{
  uint8_t index = 0;
  uint8_t RelayPins[MAX_5V_RELAYS] = {
    RELAY_5V_MISC_PSU_220V_12V,
    RELAY_5V_VERTICAL_PUMP,
    RELAY_5V_VERTICAL_VALVE,
    RELAY_5V_GROW_BED_PUMP,
  };

  for(index=0; index<MAX_5V_RELAYS; index++) {
    pinMode(RelayPins[index], OUTPUT);
    SetRelay5vState(RelayPins[index], RELAY_OFF);
  }
}

// Initialisation of the I2C relay boards.
void InitI2cRelayBoards(void)
{
  int mcp_board = 0;
  int index = 0;

  for (mcp_board = 0; mcp_board < MAX_I2C_RELAY_BOARDS; mcp_board++) {
    mcp[mcp_board].begin(mcp_board); // board address: 0 or 1.
    mcp[mcp_board].writeGPIO(0); // set OLAT to 0

    // Set each IO as outputs.
    for (index = 0; index < 8; index++) {
      mcp[mcp_board].pinMode(index, OUTPUT); // set IODIR to output.
    }
  }
}

// Set a new state to the selected output of the 5v relay.
void SetRelay5vState(uint8_t channel, uint8_t state) {
  // 5V relay output state is inverted, i.e RELAY_OFF = Relay open.
  digitalWrite(channel, !state);
}

// Set a new state to the selected output of the I2C relay.
void SetRelayI2cState(uint8_t channel, uint8_t state)
{
  static uint8_t i2cRelay8ChStatus[MAX_I2C_RELAY_BOARDS] = {0};
  uint8_t board_id = 0;

  if (channel > 7) {
    board_id = 1;
    channel -= 8;
  }

  if (state) {
      i2cRelay8ChStatus[board_id] |= (1 << channel);
  } else {
      i2cRelay8ChStatus[board_id] &= (~(1 << channel));
  }

  DEBUG_PRINT("I2C Board: ");
  DEBUG_PRINT(board_id);
  DEBUG_PRINT(", Status: ");
  DEBUG_PRINT(i2cRelay8ChStatus[board_id]);
  DEBUG_PRINT_CR();

  // Send the updated value to the i2c relay board.
  mcp[board_id].writeGPIO(i2cRelay8ChStatus[board_id]);
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

  DEBUG_PRINT("payload: "); DEBUG_PRINT(payload); DEBUG_PRINT_CR();
  DEBUG_PRINT("state: "); DEBUG_PRINT(state); DEBUG_PRINT_CR();

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
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttDeviceID)) {
      Serial.println("connected");
      // Subscribe to all the greenhouse topics
      for(index = 0; index < MAX_RELAYS; index++, prelay++){
        client.subscribe(prelay->topic.c_str());
      }

      // Check if the power is ON or running from
      // the battery backup system.
      if(false == power_on) {
        // Indicate that the power is ON.
        power_on = true;
        // Enable 12v output to the I2C relay boards.
        //SetRelay5vState(RELAY_5V_MISC_PSU_220V_12V, RELAY_ON);
      }
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

  // Initialisation of the 5v relay board.
  Init5vRelayBoard();

  // Initialisation of the I2C relay boards.
  InitI2cRelayBoards();

  // Enable 12v output to the I2C relay boards.
  SetRelay5vState(RELAY_5V_MISC_PSU_220V_12V, RELAY_ON);

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

  if(false == power_on) {
    // TODO - Power OFF logic to implement.
  } else {
    // Check for new MQTT message.
    CheckNewMQTTMessage();

    // Check if the PIR sensor detected some motion.
    // TODO.
  }
}
