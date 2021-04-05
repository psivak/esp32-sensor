#include <time.h>

class TimeService {
    public:
        static bool shouldSyncTime();
        static bool syncTime(int timeout);
        static void myDelay(unsigned long ms, bool log = true);
        static __int64_t getCurrentTime();
};