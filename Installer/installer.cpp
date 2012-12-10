//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 2004  Microsoft Corporation.  All Rights Reserved.
//
// Module Name: instlsp.cpp
//
// Description:
//
//    This sample illustrates how to develop a layered service provider.
//    This LSP is simply a pass through sample which counts the bytes transfered
//    on each socket. 
//
//    This file contains an installation program to insert the layered sample
//    into the Winsock catalog of providers.
//    
//
#include "stdafx.h"
#include <algorithm>
#include "install.h"

//
// Global variable: Function pointer to WSCUpdateProvider if on Windows XP or greater.
//                  Uninstalling an LSP when other LSPs are layered over it is really
//                  difficult; however on Windows XP and greater the WSCUpdateProvider
//                  function makes this much simpler. On older platforms its a real
//                  pain.
LPWSCUPDATEPROVIDER fnWscUpdateProvider   = NULL,
                    fnWscUpdateProvider32 = NULL;

// Prototype for usage information
void usage(char *progname);

////////////////////////////////////////////////////////////////////////////////
//
// Function Implementation
//
////////////////////////////////////////////////////////////////////////////////

//
// Function: main
//
// Description:
//    Parse the command line arguments and call either the install, remove, 
//    print, etc. routines.
//
int _cdecl main(int argc, char *argv[])
{
    WSADATA             wsd;
    LPWSAPROTOCOL_INFOW pProtocolInfo   = NULL;
    int                 protocolCount = 0;
    WINSOCK_CATALOG     eCatalog        = LspCatalog32Only;
    DWORD              *pdwCatalogIdArray = NULL,
                        dwCatalogIdArrayCount = 1,       // Install over TCP provider
                        dwRemoveCatalogId = 0;
    BOOL                bIFSProvider = FALSE;
    char               *lpszLspName = "Test Provider",
                       *lpszLspPathAndFile = "%SYSTEMROOT%\\system32\\LSP.dll";
    int                 rc;
    const wchar_t tcpName[] = L"[TCP/IP]";
    const size_t tcpLen = (sizeof (tcpName) / sizeof (wchar_t)) - 1;

    ////////////////////////////////////////////////////////////////////////////
    //
    // Initialization and Command Line Parsing
    //
    ////////////////////////////////////////////////////////////////////////////
    
    if (2 == argc)
    {
        rc = WSAStartup( MAKEWORD(2,2), &wsd );
        if ( 0 != rc )
        {
            fprintf( stderr, "Unable to load Winsock: %d\n", rc );
            return -1;
        }

        // Initialize data structures
        LspCreateHeap( &rc );

        __try
        {
            InitializeCriticalSection( &gDebugCritSec );
        }
        __except( EXCEPTION_EXECUTE_HANDLER )
        {
            goto cleanup;
        }


        // Allocate space for the array of catalog IDs
        pdwCatalogIdArray = (DWORD *) LspAlloc(
            sizeof( DWORD ) * dwCatalogIdArrayCount,
            &rc
            );
        if ( NULL == pdwCatalogIdArray )
        {
            goto cleanup;
        }

        // look for TCP/IP providers CatalogIs.
        pProtocolInfo = EnumerateProviders(eCatalog, &protocolCount);
        if (pProtocolInfo == NULL)
        {
            rc = ERROR_FILE_NOT_FOUND;
            goto cleanup;
        }
        for(int i = 0; i < protocolCount ;i++)
        {
            size_t len = wcslen(pProtocolInfo[i].szProtocol);
            if ((pProtocolInfo[i].szProtocol + len) != 
                std::search(pProtocolInfo[i].szProtocol, 
                pProtocolInfo[i].szProtocol + len,
                tcpName,
                tcpName + tcpLen))
            {
                pdwCatalogIdArray[0] = pProtocolInfo[i].dwCatalogEntryId;
            }
            if (memcmp (&pProtocolInfo[i].ProviderId, &gProviderGuid, sizeof (GUID))==0)
            {
                dwRemoveCatalogId = pProtocolInfo[i].dwCatalogEntryId;
            }
        }

        if (0 == strcmp(argv[1], "install"))
        {  
            // Install the LSP with the supplied parameters
            rc = InstallLsp(
                eCatalog,
                lpszLspName,
                lpszLspPathAndFile,
                dwCatalogIdArrayCount,
                pdwCatalogIdArray,
                bIFSProvider
                );
        }
        else if (0 == strcmp(argv[1], "uninstall"))
        {
            // We must be removing an LSP

            RemoveProvider( LspCatalog32Only, dwRemoveCatalogId );
        }

cleanup:

        //
        // Free any dynamic allocations and/or handles
        //

        if ( NULL != pdwCatalogIdArray )
            LspFree( pdwCatalogIdArray );

        if ( NULL != pProtocolInfo)
            FreeProviders( pProtocolInfo );

        LspDestroyHeap( );

        DeleteCriticalSection( &gDebugCritSec );

        WSACleanup();

        return 0;
    }

    return -1;
}
