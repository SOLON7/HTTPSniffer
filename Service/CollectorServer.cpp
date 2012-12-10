#include "StdAfx.h"
#include "CollectorServer.h"

CollectorServer* CollectorServer::instance_ = NULL;

const char* historyFileName = "C:\\HTTP_Histor.txt";
const char* port = "4004";

#define PACKSIZE    512 

DWORD WINAPI ServerMain(LPVOID);
DWORD WINAPI ClientProcess(LPVOID);

CollectorServer::CollectorServer(void)
{
    serverThread_ = CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
    if (INVALID_HANDLE_VALUE == serverThread_)
    {
        throw std::runtime_error("Failed to start server thread.");
    }
    InitializeCriticalSection(&section_);
}

CollectorServer::~CollectorServer(void)
{
    DeleteCriticalSection(&section_);
}

CollectorServer* CollectorServer::Instance()
{
    if (NULL == instance_)
    {
        instance_ = new CollectorServer();
    }

    return instance_;
}

void CollectorServer::Start()
{
    running_ = true;
}

void CollectorServer::Stop()
{
    running_ = false;
}

bool CollectorServer::IsRunning()
{
    return running_;
}

bool CollectorServer::SaveData( const std::string& data)
{
    const std::string requestTerminator = "\r\n\r\n";
    FILE* storage_;

    storage_ = fopen(historyFileName, "at");
    if (NULL == storage_)
    {
        return false;
    }

    EnterCriticalSection(&section_);
    int result = fwrite((data + requestTerminator).c_str(), (data + requestTerminator).size(), 1, storage_);
    LeaveCriticalSection(&section_);

    fclose(storage_);

    return (result > 0) ? true : false;
}
DWORD WINAPI ServerMain( LPVOID )
{
    WSADATA wsaData;
    if (0 != WSAStartup(MAKEWORD(2,2), &wsaData))
    {
        return -1;
    }

    struct addrinfo* result = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if( 0 != getaddrinfo(NULL, port, &hints, &result))
    {
        WSACleanup();
        return -1;
    }

    SOCKET listeningSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (INVALID_SOCKET == listeningSocket)
    {
        freeaddrinfo(result);
        WSACleanup();

        return -1;
    }

    if(SOCKET_ERROR == bind(listeningSocket, result->ai_addr, (int)result->ai_addrlen))
    {
        freeaddrinfo(result);
        closesocket(listeningSocket);
        WSACleanup();
        return -1;
    }

    freeaddrinfo(result);

    if (SOCKET_ERROR == listen(listeningSocket, SOMAXCONN))
    {
        closesocket(listeningSocket);
        WSACleanup();
        return -1;
    }

    while (CollectorServer::Instance()->IsRunning())
    {
        SOCKET clientSocket = accept(listeningSocket, NULL, NULL);
        if (INVALID_SOCKET == clientSocket)
        {
            closesocket(listeningSocket);
            WSACleanup();

            return -1;
        }
        else
        {
            HANDLE clientThread = CreateThread(NULL, 0, ClientProcess, (LPVOID)clientSocket, 0, NULL);

            if (INVALID_HANDLE_VALUE == clientThread)
            {
                return -1;
            }
            else
            {
                CloseHandle(clientThread);
            }
        }
    }

    closesocket(listeningSocket);

    return 0;
}

DWORD WINAPI ClientProcess( LPVOID parameter )
{
    SOCKET clientSocket = (SOCKET)parameter;

    if (INVALID_SOCKET == clientSocket)
    {
        return -1;
    }

    char buffer[PACKSIZE];
    const std::string messageTerminator = "\r\n\r\n";
    std::string response;
    int result = 0;
    size_t position = 0;

    do 
    {
        result = recv(clientSocket, buffer, PACKSIZE, 0);

        if (result > 0)
        {
            response += std::string(buffer, result);

            do 
            {
                position = response.find(messageTerminator);

                if (std::string::npos != position)
                {
                    if (!CollectorServer::Instance()->SaveData(std::string(response.begin(), response.begin() + position)))
                    {
                        return -1;
                    }
                    response = response.substr(position + messageTerminator.size());
                }
            } while (std::string::npos != position);
        }
        else
        {
            break;
        }
    } while (SOCKET_ERROR != result);
    return 0;
}