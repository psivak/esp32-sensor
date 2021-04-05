#include <wifiService.h>

class ConnectToWiFiTask {
    public:
        ConnectToWiFiTask(WifiService wifiService);
        bool run(const char* ssid, const char* password, int timeout);
    private:
        WifiService wifiService;
};