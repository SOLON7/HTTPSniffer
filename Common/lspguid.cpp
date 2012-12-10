// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 2004  Microsoft Corporation.  All Rights Reserved.
//
// Module Name: lspguid.cpp
//
// Description:
//
//      This module defines the GUID under which the LSP's hidden dummy
//      entry is to be installed under. This GUID must be hard coded in
//      this file so that the DLL instance can find itself in the Winsock 
//      catalog. Once it finds the dummy hidden entry, it can then find
//      the layered protocol entries belonging to the LSP (since these
//      entries will reference the catalog ID of the hidden dummy entry).
//      
//      It is possible to create multiple LSPs by simply copying this project
//      to another directory, changing the GUID in this file, and changing
//      the DLL name. The LSP installer (instlsp.exe) can then install each
//      LSP with the -d flag which is the full path to each LSP to install.
//
//      A new export has been added: GetLspGuid. This allows the LSP installer
//      to install an arbitrary LSP as long as it exports this function as the
//      installer needs to find the GUID to install the hidden dummy entry
//      with. The GUIDs for the layered protocol entries are generated on the
//      fly.
//
//#include "lspdef.h"
#include "stdafx.h"

//
// This is the hardcoded guid for our dummy (hidden) catalog entry
//
GUID gProviderGuid = { //c5fabbd0-9736-11d1-937f-00c04fad860d
    0xc5fabbd0,
    0x9736,
    0x11d1,
    {0x93, 0x7f, 0x00, 0xc0, 0x4f, 0xad, 0x86, 0x0d}
};