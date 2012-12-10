#include "stdafx.h"
#include "Constants.h"

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

void WINAPI ServiceController(DWORD command)
{
    switch (command)
    {
    case SERVICE_CONTROL_STOP:            
    case SERVICE_CONTROL_SHUTDOWN:
        serviceStatus.dwWin32ExitCode = 0;
        serviceStatus.dwCurrentState =  SERVICE_STOPPED;
        serviceStatus.dwCheckPoint = 0;
        serviceStatus.dwWaitHint = 0;

        CollectorServer::Instance()->Stop();
        break;

    case SERVICE_CONTROL_PAUSE:
        serviceStatus.dwCurrentState = SERVICE_PAUSED;
        break;

    case SERVICE_CONTROL_CONTINUE:
        serviceStatus.dwCurrentState = SERVICE_RUNNING;
        break;

    default:
        break;
    }
    if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
    {
        return;
    }
}
void WINAPI ServiceMain(DWORD, LPTSTR*)
{
    serviceStatus.dwServiceType =   SERVICE_WIN32;
    serviceStatus.dwCurrentState =  SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |SERVICE_ACCEPT_SHUTDOWN;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;

    serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, ServiceController);

    if (!serviceStatusHandle)
    {
        return;
    }

    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;

    if (!SetServiceStatus(serviceStatusHandle, &serviceStatus))
    {
        return;
    }

    try
    {
        CollectorServer::Instance()->Start();
    }
    catch (...)
    {
        // Report error
    }
}

bool InstallService()
{
    TCHAR path[MAX_PATH];

    if (!GetModuleFileName(NULL, path, MAX_PATH))
    {
        return false;
    }

    SC_HANDLE manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == manager)
    {
        return false;
    }

    SC_HANDLE service = CreateService(manager, serviceName, fullServiceName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL );
    if (NULL == service)
    {
        CloseServiceHandle(manager);
        return false;
    }

    if (!StartService(service, 0, NULL))
    {
        CloseServiceHandle(manager);
        return false;
    }

    CloseServiceHandle(service);
    CloseServiceHandle(manager);

    return true;
}

bool UninstallService()
{
    SERVICE_STATUS_PROCESS info;
    DWORD bytes;

    SC_HANDLE manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == manager)
    {
        return false;
    }

    SC_HANDLE service = OpenService(manager, serviceName, SERVICE_ALL_ACCESS);

    if (NULL == service)
    {
        CloseServiceHandle(manager);
        return false;
    }

    if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, (LPBYTE)&info, sizeof(SERVICE_STATUS_PROCESS), &bytes))
    {
        CloseServiceHandle(manager);
        return false;
    }

    if (SERVICE_STOPPED != info.dwCurrentState)
    {
        SERVICE_STATUS status;
        if (!ControlService(service, SERVICE_CONTROL_STOP, &status))
        {
            CloseServiceHandle(manager);
            return false;
        }
    }

    if (!DeleteService(service))
    {
        CloseServiceHandle(manager);
        return false;
    }

    CloseServiceHandle(service);
    CloseServiceHandle(manager);

    return true;
}