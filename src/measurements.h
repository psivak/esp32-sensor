#include <Arduino.h>

class Measurements
{
    public:
        Measurements();

        int getCount();

        bool add(__int64_t timeStamp, float temperature);
        void clear();
        void log();
        String createMqttMessage();

    private:
        static const int size = 50;
        int count;
        float temperatures[size];
        __int64_t measurementTimes[size];
        int getCurrentIndex();
};