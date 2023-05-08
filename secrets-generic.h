//

#ifndef __secrets_h__
#define __secrets_h__

#define WIFI_SSID wifi-ssid
#define WIFI_PASSWORD wifi-password

#define MQTT_SERVER mqtt-broker
#define MQTT_PORT 1883
#define MQTT_USER mqtt-user
#define MQTT_PASSWORD mqtt-paswword

// Used physical pins

#define PIN_SONAR_TRIG D0
#define PIN_SONAR_ECHO D1

// Water tank specific information

#define DESALINATED_WATER_TANK_AREA             28.3  // Square dm
#define DESALINATED_WATER_TANK_HEIGHT           14    // Height of the water tank in dm
#define DESALINATED_WATER_TANK_ALARM_LOW_VOLUME 14    // Height of the water tank in dm

// mqtt topics

#define MQTT_TOPIC_STATUS "ssa174/tank/desal/status"
#define MQTT_TOPIC_ALARM "ssa174/tank/desal/alarm"
#define MQTT_TOPIC_DATA "ssa174/tank/desal/volume"

#endif
