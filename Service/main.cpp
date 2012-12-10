// Service.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Constants.h"

void WINAPI ServiceMain(DWORD, LPTSTR*);
bool InstallService();
bool UninstallService();

int main(int argc, char* argv[])
{
    if (2 == argc)
    {
        if (0 == strcmp(argv[1], "install"))
        {
            if (InstallService())
            {
                printf("Installed service.");
                return 0;
            }
            else
            {
                printf("Installation failed.");
                return -1;
            }
        }
        else if (0 == strcmp(argv[1], "uninstall"))
        {
            if (UninstallService())
            {
                printf("Removed service.");
                return 0;
            }
            else
            {
                printf("Uninstall failed.");
                return -1;
            }
        }
    }
    else
    {
        SERVICE_TABLE_ENTRY dispatchTable[] = 
        {
            { serviceName, (LPSERVICE_MAIN_FUNCTION) ServiceMain },
            { NULL, NULL }
        };

        if (!StartServiceCtrlDispatcher(dispatchTable))
        {
            printf("Failed start service.");
            return -1;
        }
    }
	return 0;
}

