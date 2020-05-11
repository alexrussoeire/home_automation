// NodeMCU 1.0 (ESP-12E Module)
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <QueueList.h>
#include "UserConfig.h"
#include "Arduino.h"
#include "DHT.h"

//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_PRINT(msg)      Serial.print(msg)
  #define DEBUG_PRINT_CR(msg)   Serial.println(msg)
#else
  #define DEBUG_PRINT(msg)
  #define DEBUG_PRINT_CR(msg)
#endif

#define MAX_RELAYS            (2)
#define RELAY_OFF             (HIGH)
#define RELAY_ON              (LOW)

#define DHTPIN D1
#define DHTTYPE DHT21
#define DHT_READING_PERIOD_MS (1000)
DHT dht(DHTPIN, DHTTYPE);

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
  {"propagator/light",  SetRelayState, D7, RELAY_OFF},
  {"propagator/heater", SetRelayState, D8, RELAY_OFF},
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

  DEBUG_PRINT("payload: "); DEBUG_PRINT_CR(payload);
  DEBUG_PRINT("state: ");
  // Convert the payload value into a value.
  if('1' == payload) {
    state = RELAY_ON;
    DEBUG_PRINT("RELAY_ON");
  } else {
    state = RELAY_OFF;
    DEBUG_PRINT("RELAY_OFF");
  }
  
  DEBUG_PRINT_CR("");

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
      DEBUG_PRINT("humidity: ");
      DEBUG_PRINT_CR(humidity);
      DEBUG_PRINT("temperature: ");
      DEBUG_PRINT_CR(temperature);

      client.publish("propagator/humidity",    String(humidity).c_str(), true);
      client.publish("propagator/temperature", String(temperature).c_str(), true);
  }
}
