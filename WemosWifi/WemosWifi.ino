#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <QueueList.h>
#include "UserConfig.h"
#include "Arduino.h"
#include "DHT.h"

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_PRINT(msg)    Serial.print(msg)
  #define DEBUG_PRINT_CR()    Serial.println()
#else
  #define DEBUG_PRINT(msg)
  #define DEBUG_PRINT_CR()
#endif

#define MAX_RELAYS            (8)
#define RELAY_OFF             (LOW)
#define RELAY_ON              (HIGH)

#define DHTPIN D1
#define DHTTYPE DHT21
#define DHT_READING_PERIOD_MS (1000)
DHT dht(DHTPIN, DHTTYPE);

/*
typedef enum
{
  RELAY_0 = D0,
  RELAY_1 = D1,
  RELAY_2 = D2,
  RELAY_3 = D3,
  RELAY_4 = D4,
  RELAY_5 = D5,
  RELAY_6 = D6,
  RELAY_7 = D7
} relay_id_t;

// MQTT Topics for the grow towers to subscribed to.
#define MQTT_TOPIC_WEMOS_D_0  "wemos/dout/0"
#define MQTT_TOPIC_WEMOS_D_1  "wemos/dout/1"
#define MQTT_TOPIC_WEMOS_D_2  "wemos/dout/2"
#define MQTT_TOPIC_WEMOS_D_3  "wemos/dout/3"
#define MQTT_TOPIC_WEMOS_D_4  "wemos/dout/4"
#define MQTT_TOPIC_WEMOS_D_5  "wemos/dout/5"
#define MQTT_TOPIC_WEMOS_D_6  "wemos/dout/6"
#define MQTT_TOPIC_WEMOS_D_7  "wemos/dout/7"
*/

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
  {"wemos/dout/0", SetRelayState, D0, RELAY_OFF},
  {"wemos/dout/1", SetRelayState, D1, RELAY_OFF},
  {"wemos/dout/2", SetRelayState, D2, RELAY_OFF},
  {"wemos/dout/3", SetRelayState, D3, RELAY_OFF},
  {"wemos/dout/4", SetRelayState, D4, RELAY_OFF},
  {"wemos/dout/5", SetRelayState, D5, RELAY_OFF},
  {"wemos/dout/6", SetRelayState, D6, RELAY_OFF},
  {"wemos/dout/7", SetRelayState, D7, RELAY_OFF},
};

// Humidity and temperature sensors latest 
unsigned long readTime = 0;
unsigned long displayTime = 0;
float humidity = 0.0;
float temperature = 0.0;

// Initialisation of the IO Expander boards.
void InitRelayBoards(void)
{
  uint8_t index = 0;
  relay_t* prelay = relay_table;

  for(index = 0; index < MAX_RELAYS; index++, prelay++){
      pinMode(prelay->channel, OUTPUT);
      prelay->pRelayFn(prelay->channel, RELAY_OFF);
  }
}

// Set a new state to the selected output of the IO expander board.
void SetRelayState(uint8_t channel, uint8_t state) {
  digitalWrite(channel, state);
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
    if (client.connect(mqttDeviceID, ssid, password)) {
      Serial.println("connected");
      // Subscribe to all the grow tower topics
      for(index = 0; index < MAX_RELAYS; index++, prelay++){
        client.subscribe(prelay->topic.c_str());
        Serial.println(prelay->topic.c_str());
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Attempt to connect to WiFi network:
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      // Connect to WPA/WPA2 network. Change this line if using open or WEP  network:
      // Wait 5 seconds for connection:
      delay(5000);
    }
  
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP
  }
}

void setup()
{
  Serial.begin(9600);

  // Initialisation of the IO expander boards.
  InitRelayBoards();

  // Initialisation of the humidity/temperature sensor.
  dht.begin();

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

  // Read from the humidity and temperature sensor.
  if(millis() >= readTime + DHT_READING_PERIOD_MS){
      readTime +=DHT_READING_PERIOD_MS;
      humidity = dht.readHumidity();
      temperature = dht.readTemperature();
  }

  if(millis() >= displayTime + (5*DHT_READING_PERIOD_MS)){
      displayTime +=(DHT_READING_PERIOD_MS*5);
      Serial.print("humidity: ");
      Serial.println(humidity);
      Serial.print("temperature: ");
      Serial.println(temperature);

      client.publish("wemos/humidity", String(humidity).c_str(), true);
      client.publish("wemos/temperature", String(temperature).c_str(), true);
  }
}
