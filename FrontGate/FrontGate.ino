#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <QueueList.h>
#include "UserConfig.h"

//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_PRINT(msg)    Serial.print(msg)
  #define DEBUG_PRINT_CR()    Serial.println()
#else
  #define DEBUG_PRINT(msg)
  #define DEBUG_PRINT_CR()
#endif

#define MAX_RELAYS            (4)
#define MAX_BUTTONS           (4)
#define MAX_BLUE_LED          (1)
#define MAX_SENSORS           (3)
#define MAX_IO                (MAX_RELAYS + MAX_BUTTONS + MAX_BLUE_LED + MAX_SENSORS)

#define OUTPUT_OFF            (0)
#define OUTPUT_ON             (1)
#define DEFAULT_STATE         (OUTPUT_OFF)

#define NON_INVERTED          (0)
#define INVERTED              (1)

#define RELAY_1               (D6) //(D12) //GPIO12
#define RELAY_2               (D1) //(D5)  //GPIO5
#define RELAY_3               (D2) //(D4)  //GPIO4
#define RELAY_4               (D8) //(D15) //GPIO15
#define BUTTON_1              (D3) //(D0)  //GPIO0  // (inverted)
#define BUTTON_2              (D11) //(D9)  //GPIO9  // (inverted)
#define BUTTON_3              (D12) //(D10) //GPIO10 // (inverted)
#define BUTTON_4              (D5) //(D14) //GPIO14 // (inverted)
#define BLUE_LED              (D7) //(D13) //GPIO13 // (inverted)
#define SENSOR_1              (D10) //(D1)  //GPIO1
#define SENSOR_2              (D9) //(D3)  //GPIO3
#define SENSOR_3              (D4) //(D2)  //GPIO2

// MQTT Topics for the front gate to subscribed to.
#define MQTT_TOPIC_FRONT_GATE_RELAY_1  "frontgate/relay_1"
#define MQTT_TOPIC_FRONT_GATE_RELAY_2  "frontgate/relay_2"
#define MQTT_TOPIC_FRONT_GATE_RELAY_3  "frontgate/relay_3"
#define MQTT_TOPIC_FRONT_GATE_RELAY_4  "frontgate/relay_4"

#define MQTT_TOPIC_FRONT_GATE_BLUE_LED "frontgate/blue_led"

#define MQTT_TOPIC_FRONT_GATE_BUTTON_1 "frontgate/button_1"
#define MQTT_TOPIC_FRONT_GATE_BUTTON_2 "frontgate/button_2"
#define MQTT_TOPIC_FRONT_GATE_BUTTON_3 "frontgate/button_3"
#define MQTT_TOPIC_FRONT_GATE_BUTTON_4 "frontgate/button_4"

#define MQTT_TOPIC_FRONT_GATE_SENSOR_1 "frontgate/sensor_1"
#define MQTT_TOPIC_FRONT_GATE_SENSOR_2 "frontgate/sensor_2"
#define MQTT_TOPIC_FRONT_GATE_SENSOR_3 "frontgate/sensor_3"

// Prototype for the generic callback function.
typedef uint8_t (*pCallback)(uint8_t, uint8_t*, uint8_t);

typedef struct {
  String topic;
  char payload;
} mqtt_msg_t;

// Definition of the IO.
typedef struct {
    String topic;     // MQTT topic for the input/output.
    pCallback pCb;    // Callback function.
    uint8_t io_dir;   // Input/Output direction.
    uint8_t io_pin;   // Input/Output number.
    uint8_t state;    // Current state of the input or output.
    uint8_t inverted; // 0: Non inverted, 1: inverted.
} iodef_t;

// Input and output function prototypes.
void SetOutputState(uint8_t io_pin, uint8_t* state, uint8_t inverted);
void GetInputState (uint8_t io_pin, uint8_t* state, uint8_t inverted);
void GetSensorState(uint8_t io_pin, uint8_t* state, uint8_t inverted);

WiFiClient espClient;
PubSubClient client(espClient);

// create a queue of strings messages.
QueueList <mqtt_msg_t> mqtt_msg_queue;

// Table of all inputs and outputs.
iodef_t io_table[MAX_IO] =
{
  // Relay output configuration.
  {MQTT_TOPIC_FRONT_GATE_RELAY_1, OUTPUT, SetOutputState, RELAY_1, DEFAULT_STATE, NON_INVERTED},
  {MQTT_TOPIC_FRONT_GATE_RELAY_2, OUTPUT, SetOutputState, RELAY_2, DEFAULT_STATE, NON_INVERTED},
  {MQTT_TOPIC_FRONT_GATE_RELAY_3, OUTPUT, SetOutputState, RELAY_3, DEFAULT_STATE, NON_INVERTED},
  {MQTT_TOPIC_FRONT_GATE_RELAY_4, OUTPUT, SetOutputState, RELAY_4, DEFAULT_STATE, NON_INVERTED},

  // Blue LED configuration.
  {MQTT_TOPIC_FRONT_GATE_BLUE_LED, OUTPUT, SetOutputState, BLUE_LED, DEFAULT_STATE, INVERTED},

  // Input button configuration.
  {MQTT_TOPIC_FRONT_GATE_BUTTON_1, INPUT, GetInputState, BUTTON_1, DEFAULT_STATE, INVERTED},
  {MQTT_TOPIC_FRONT_GATE_BUTTON_2, INPUT, GetInputState, BUTTON_2, DEFAULT_STATE, INVERTED},
  {MQTT_TOPIC_FRONT_GATE_BUTTON_3, INPUT, GetInputState, BUTTON_3, DEFAULT_STATE, INVERTED},
  {MQTT_TOPIC_FRONT_GATE_BUTTON_4, INPUT, GetInputState, BUTTON_4, DEFAULT_STATE, INVERTED},

  // Input sensor configuration.
  {MQTT_TOPIC_FRONT_GATE_SENSOR_1, INPUT, GetSensorState, SENSOR_1, DEFAULT_STATE, NON_INVERTED},
  {MQTT_TOPIC_FRONT_GATE_SENSOR_2, INPUT, GetSensorState, SENSOR_2, DEFAULT_STATE, NON_INVERTED},
  {MQTT_TOPIC_FRONT_GATE_SENSOR_3, INPUT, GetSensorState, SENSOR_3, DEFAULT_STATE, NON_INVERTED}
};

// Initialisation of the inputs and outputs.
void InitIO(void)
{
  uint8_t index = 0;
  iodef_t* pio  = io_table;

  for(index = 0; index < MAX_IO; index++, pio++){
    // Set the pin as input or output.
    pinMode(pio->io_pin, pio->io_dir);

    // Set or get the pin state.
    pio->pCb(pio->io_pin, &(pio->state), pio->inverted);
  }
}

// Set the new state for the selected output.
void SetOutputState(uint8_t io_pin, uint8_t* state, uint8_t inverted) {
  // Check if the input or output is inverted.
  if(0 == inverted) {
      digitalWrite(io_pin, *state);
  } else {
      digitalWrite(io_pin, !(*state));
  }
}

// Get the state of the selected digital input.
void GetInputState(uint8_t io_pin,  uint8_t* state, uint8_t inverted) {
  
}

// Get the state of the selected input.
void GetSensorState(uint8_t io_pin, uint8_t* state, uint8_t inverted) {
  
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
  iodef_t* pio  = io_table;

  // Convert the payload value into a value.
  if('1' == payload) {
    state = OUTPUT_ON;
  } else {
    state = OUTPUT_OFF;
  }

  DEBUG_PRINT("payload: "); DEBUG_PRINT(payload); DEBUG_PRINT_CR();
  DEBUG_PRINT("state: "); DEBUG_PRINT(state); DEBUG_PRINT_CR();

  for(index = 0; index < MAX_IO; index++, pio++){
    if(pio->topic == topic) {
      // Update the state.
      pio->state = state;

      // Call the corresponding function.
      pio->pCb(pio->io_pin, &pio->state, pio->inverted);
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
  iodef_t* pio = io_table;

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttDeviceID)) {
      Serial.println("connected");
      // Subscribe to all the greenhouse topics
      for(index = 0; index < MAX_IO; index++, pio++){
        client.subscribe(pio->topic.c_str());
        //Serial.println(pio->topic.c_str());
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

  // Initialisation of the inputs and outputs.
  InitIO();

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
