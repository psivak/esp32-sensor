#include <WiFi.h>
#include <wifiService.h>
#include <config.h>
#include <timeService.h>

void WifiService::subscribeLogEvent()
{
    WiFi.onEvent(WifiService::logWiFiEvent);
}

void WifiService::logWiFiStatus()
{
    switch (WiFi.status())
    {
    case WL_CONNECTED:
        Serial.println("WL_CONNECTED");
        break;
    case WL_NO_SHIELD:
        Serial.println("WL_NO_SHIELD");
        break;
    case WL_IDLE_STATUS:
        Serial.println("WL_IDLE_STATUS");
        break;
    case WL_NO_SSID_AVAIL:
        Serial.println("WL_NO_SSID_AVAIL");
        break;
    case WL_SCAN_COMPLETED:
        Serial.println("WL_SCAN_COMPLETED");
        break;
    case WL_CONNECT_FAILED:
        Serial.println("WL_CONNECT_FAILED");
        break;
    case WL_CONNECTION_LOST:
        Serial.println("WL_CONNECTION_LOST");
        break;
    case WL_DISCONNECTED:
        Serial.println("WL_DISCONNECTED");
        break;
    }
}

void WifiService::disconnect()
{
    Serial.printf("[%lu] Disconnecting from WiFi\n", millis());
    WiFi.disconnect();
    TimeService::myDelay(10, false);
}

void WifiService::logWiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[%lu] WiFi-event: ", millis());

    switch (event)
    {
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
        Serial.print("Connected to access point, RSSI: ");
        Serial.println(WiFi.RSSI());
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
    default:
        break;
    }
}