#include <Arduino.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <time.h>

// DS18B20
const int oneWireBus = 15;

// Wifi
const char* ssid = "";
const char* password = "";

// NTP
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset = 3600; // seconds
const int   daylightOffset = 0; // seconds

// MQTT
const char* mqttServer = "";
const char* mqttClientId = "arduino";
const int mqttPort = 1883;
const int mqttRetryCount = 10;

// Deep sleep
const int timeToSleep = 60; // seconds
const int measurements = 60; // count per WiFi connection

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
RTC_DATA_ATTR float temperatures[measurements];
RTC_DATA_ATTR long measurementTimes[measurements];

WiFiClient net;
MQTTClient client(4096);
RTC_DATA_ATTR int bootCount = 0;

void myDelay(unsigned long ms)
{
  unsigned long start = millis();
  while (millis() - start < ms)
    ;
}

void sleep() {
  client.disconnect();
  WiFi.disconnect();

  Serial.printf("Going to deep sleep for %lu seconds.\n", timeToSleep - (millis() / 1000));
  Serial.flush();

  esp_sleep_enable_timer_wakeup((timeToSleep * 1000000) - micros());
  esp_deep_sleep_start();
}

void logWiFiEvent(WiFiEvent_t event) {
  Serial.print("[WiFi-event] ");

  switch (event) {
    case SYSTEM_EVENT_WIFI_READY:
      Serial.println("WiFi interface ready");
      break;
    case SYSTEM_EVENT_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case SYSTEM_EVENT_STA_START:
      Serial.println("WiFi client started");
      break;
    case SYSTEM_EVENT_STA_STOP:
      Serial.println("WiFi clients stopped");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      Serial.println("Lost IP address and IP address is reset to 0");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case SYSTEM_EVENT_AP_START:
      Serial.println("WiFi access point started");
      break;
    case SYSTEM_EVENT_AP_STOP:
      Serial.println("WiFi access point  stopped");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Client connected");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Client disconnected");
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      Serial.println("Assigned IP address to client");
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      Serial.println("Received probe request");
      break;
    case SYSTEM_EVENT_GOT_IP6:
      Serial.println("IPv6 is preferred");
      break;
    case SYSTEM_EVENT_ETH_START:
      Serial.println("Ethernet started");
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("Ethernet stopped");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("Ethernet connected");
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("Ethernet disconnected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.println("Obtained IP address");
      break;
    default: break;
}}

void logWakeUpCause(esp_sleep_wakeup_cause_t wakeup_cause) {
  switch(wakeup_cause)
  {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_cause);
      break;
  }
}

void syncTime() {
  Serial.println("Sending request to NTP server.");
  configTime(gmtOffset, daylightOffset, ntpServer);

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to sync time with NTP server.");
    return;
  }
}

long getCurrentTime() {
  time_t now;
  time(&now);

  return now;
}

void readSensorValue() {
  int index = (bootCount + measurements - 2) % measurements;

  measurementTimes[index] = getCurrentTime();

  sensors.begin();
  sensors.requestTemperatures();
  temperatures[index] = sensors.getTempCByIndex(0);

  Serial.printf("Index: %i\n", index);
  Serial.printf("Measurement time: %ld\n", measurementTimes[index]);
  Serial.printf("Temperature: %.3f°C\n", temperatures[index] );
}

String createMqttMessage(int startIndex) {
  String message = String("[");
  char buffer[50];
  for (int i = startIndex; i < measurements; i++) {
    sprintf(buffer, "{\"time\":%ld,\"temperature\":%.3f}", measurementTimes[i], temperatures[i]);
    if (i != startIndex)
      message += ",";
    message += buffer;
  }
  message += "]";

  return message;
}

void onWiFiGotIp(WiFiEvent_t event, WiFiEventInfo_t info) {
  int connectionRetryCount = mqttRetryCount;

  syncTime();

  Serial.println("Connecting to MQTT");
  client.begin(mqttServer, mqttPort, net);

  while (!client.connect(mqttClientId) && connectionRetryCount > 0) {
    myDelay(1000);
    connectionRetryCount--;
  }

  if (connectionRetryCount == 0) {
    Serial.println("Connecting to MQTT failed.");
    sleep();
  }

  int startIndex = 0;

  // Reread measurement on the first boot
  if (bootCount == 1) {
    readSensorValue();
    startIndex = measurements - 1;
  }

  String message = createMqttMessage(startIndex);
  int publishRetryCount = mqttRetryCount;

  // Send MQTT message
  Serial.print("Sending message to MQTT: ");
  Serial.println(message);
  while (!client.publish("temperature", message) && publishRetryCount > 0) {
    myDelay(1000);
    publishRetryCount--;
  }

  if (publishRetryCount == 0) {
    Serial.println("Sending MQTT message failed.");
  }

  myDelay(1000);
  sleep();
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi.");

  WiFi.mode(WIFI_STA);
  WiFi.onEvent(logWiFiEvent);
  WiFi.onEvent(onWiFiGotIp, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.begin(ssid, password);
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.printf("Boot count: %d\n", ++bootCount);

  readSensorValue();

  if (bootCount % measurements == 1) {
    connectToWiFi();

    // Wait for WiFi related tasks
    myDelay(10000);
  }

  sleep();
}

void loop() { }