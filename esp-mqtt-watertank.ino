//*************************************************************************************
//*
//*************************************************************************************
//*************************************************************************************
//*
//*************************************************************************************
#include "main.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <string.h>

#include "secrets.h"
#include "WaterTank.h"


//*************************************************************************************
//* Constant declarations
//*************************************************************************************

const char* wifi_ssid = WIFI_SSID;
const char* wifi_password = WIFI_PASSWORD;

const char* mqtt_server = MQTT_SERVER;
const char* mqtt_username = MQTT_USER;
const char* mqtt_password = MQTT_PASSWORD;

const char* mqtt_subscribe_topic = MQTT_TOPIC_SUBSCRIBE;
const char* mqtt_data_topic = MQTT_TOPIC_DATA;
const char* mqtt_alarm_topic = MQTT_TOPIC_ALARM;
const char* mqtt_status_topic = MQTT_TOPIC_STATUS;

//*************************************************************************************
//*
//*************************************************************************************

void watertankAlarmlowLevelCallback(boolean, float);

//*************************************************************************************
//* Global variable declarations - MQTT/WIFI
//*************************************************************************************

WiFiClient wifiEspClient;
  
PubSubClient client(wifiEspClient);

char mqttPayloadMsgBuffer[MQTT_PAYLOAD_MSG_BUFFER_SIZE];

//*************************************************************************************
//* Global variable declarations - WaterTank
//*************************************************************************************

WaterTank desalinatedWaterTank(PIN_SONAR_TRIG, 
                               PIN_SONAR_ECHO, 
                               DESALINATED_WATER_TANK_HEIGHT, 
                               DESALINATED_WATER_TANK_AREA,
                               DESALINATED_WATER_TANK_ALARM_LOW_VOLUME,
                               watertankAlarmlowLevelCallback);

//*************************************************************************************
//*
//*************************************************************************************

//*************************************************************************************
//*
//*************************************************************************************

  void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
  
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
/*
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is active low on the ESP-01)
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
*/
  }

//*************************************************************************************
//*
//*************************************************************************************

  void mqttReconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Create a random client ID
      String clientId = "WaterTank-";
      clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (client.connect(clientId.c_str()), mqtt_username, mqtt_password) {
        Serial.println("connected");
        // Once connected, publish an announcement...
        client.publish(mqtt_status_topic, "Up");
        // ... and resubscribe
        client.subscribe("inTopic");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }

//*************************************************************************************
//*
//*************************************************************************************

  void mqttPublish(char* topicPtr, char* payloadPtr) {
    if (!client.connected()) {
      mqttReconnect();
    }
    client.loop();
    
    client.publish(topicPtr,payloadPtr);  
  }
  
//*************************************************************************************
//*
//*************************************************************************************

void mqttWifiSetup() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

//*************************************************************************************
//*
//*************************************************************************************

void mqttSetup() {
  mqttWifiSetup();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(mqttCallback);
  client.subscribe(mqtt_subscribe_topic);
}

//*************************************************************************************
//*
//*************************************************************************************

void watertankAlarmlowLevelCallback(boolean activeAlarm, float tankVolume) {
  char mqttPayload[31];

  Serial.print("Watertank low level alarm ");
  if (activeAlarm) {
    Serial.print("active");
  } else {
    Serial.print("ceased");    
  }
  Serial.print(" Volume=");
  Serial.print(tankVolume);
  Serial.println(" Liter");

  snprintf(mqttPayload,31,"{type:\"LowLevel\",status:\"%s\",volume:%.0f}",(activeAlarm? "Active":"Ceased"),tankVolume);

  mqttPublish(mqtt_alarm_topic,mqttPayload);
}
//*************************************************************************************
//*
//*************************************************************************************

void getVolume() {
  char mqttPayload[24];

  float tankVolume;

  Serial.print("Water volume = ");
  if ((tankVolume = desalinatedWaterTank.getTankVolume()) >= 0) {
    snprintf(mqttPayload,24,"%.0f",tankVolume);

    mqttPublish(mqtt_data_topic,mqttPayload);
  } else {
    // There are a fault in the measurement
    snprintf(mqttPayload,24,"{volume:%f;status:\"suspect\"}",tankVolume);

    mqttPublish(mqtt_status_topic,mqttPayload);
  } 

  Serial.println(mqttPayload);
}


//*************************************************************************************
//*
//*************************************************************************************
void waterTankSetup() {
  char mqttPayload[24];

  snprintf(mqttPayload,24,"{status:\"started\"}");

  mqttPublish(mqtt_status_topic,mqttPayload);

  
}

//*************************************************************************************
//*
//*************************************************************************************

unsigned long timeForLastMsg = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial.println("MQTT water tank server starting");
  mqttSetup();
  waterTankSetup();
}

void loop() {
  unsigned long now = millis();
  if (now - timeForLastMsg > 15000) {
    timeForLastMsg = now;

    getVolume();
    

  }
}
