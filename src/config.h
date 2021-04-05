#ifndef CONFIG_H
#define CONFIG_H

// Measurements are cached before sending to MQTT server
#define CACHE_SIZE 10

// DS18B20
#define ONE_WIRE_BUS 15

// Wifi
#define SSID ""
#define PASSWORD ""

// NTP
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET 3600 // seconds
#define DAYLIGHT_OFFSET 0 // seconds

// MQTT
#define MQTT_SERVER ""
#define MQTT_CLIENT_ID ""
#define MQTT_PORT 1883
#define MQTT_RETRY_COUNT 10

// Deep sleep
#define TIME_TO_SLEEP 60 // seconds

#endif