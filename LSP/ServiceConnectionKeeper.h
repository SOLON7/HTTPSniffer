#ifndef _SERVICE_CONNECTION_KEEPER_
#define _SERVICE_CONNECTION_KEEPER_

class ServiceConnectionKeeper
{
public:
    ServiceConnectionKeeper(){}
    ~ServiceConnectionKeeper(){}
    bool Created();
    bool Connected();
    void SetupSocket(SOCKET);
    void SetConnected();
    void AddReference();
    void RemoveReference();
    SOCKET GetSocket() { return socket_; }

private:
    SOCKET socket_;
    static int references_;
    static bool created_;
    static bool connected_;
};

#endif //_SERVICE_CONNECTION_KEEPER_
