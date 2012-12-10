#include "StdAfx.h"
#include "ServiceConnectionKeeper.h"

int ServiceConnectionKeeper::references_ = 0;
bool ServiceConnectionKeeper::connected_ = false;
bool ServiceConnectionKeeper::created_ = false;


bool ServiceConnectionKeeper::Connected()
{
    return connected_;
}

bool ServiceConnectionKeeper::Created()
{
    return (0 == references_) ? false : true;
}

void ServiceConnectionKeeper::SetConnected()
{
    connected_ = true;
}

void ServiceConnectionKeeper::AddReference()
{
    ++references_;
}

void ServiceConnectionKeeper::RemoveReference()
{
    --references_;
}

void ServiceConnectionKeeper::SetupSocket( SOCKET socket )
{
    socket_ = socket;
    created_ = true;
    ++references_;
}