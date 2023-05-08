//

#ifndef __secrets_h__
#define __secrets_h__

#define WIFI_SSID "SSA174-IoT"
#define WIFI_PASSWORD "08Run666T"

#define MQTT_SERVER "broker.mqtt-dashboard.com"
#define MQTT_PORT 1883
#define MQTT_USER "dev@kjeholt.se"
#define MQTT_PASSWORD "qT%37aDr4Xt"

// Used physical pins

#define PIN_SONAR_TRIG D0
#define PIN_SONAR_ECHO D1

// Water tank specific information

#define DESALINATED_WATER_TANK_AREA             28.3  // Square dm
#define DESALINATED_WATER_TANK_HEIGHT           14    // Height of the water tank in dm
#define DESALINATED_WATER_TANK_ALARM_LOW_VOLUME 14    // Height of the water tank in dm

#endif
