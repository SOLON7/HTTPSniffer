#ifndef _COLLECTOR_SERVER_
#define _COLLECTOR_SERVER_

#include <windows.h>
#include <string>

class CollectorServer
{
    CollectorServer(void);
    ~CollectorServer(void);

public:
    static CollectorServer* Instance();

    void Start();
    void Stop();
    bool IsRunning();
    bool SaveData(const std::string&);

private:
    CRITICAL_SECTION section_;
    bool running_;
    static CollectorServer* instance_;
    HANDLE serverThread_;
};

#endif //_COLLECTOR_SERVER_