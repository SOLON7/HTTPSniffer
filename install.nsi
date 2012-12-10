; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Test Provider Installer"

; The file to write
OutFile "setup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\TestLSP

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM Software\TestLSP "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Components (required)"

  SectionIn RO
  
  ; Set out dir for provider component
  SetOutPath $SYSDIR
  File "LSP.dll"  
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "Service.exe"
  File "Installer.exe"
  
  ; Run installer components
  Exec '"$INSTDIR\Service.exe" install'
  Exec '"$INSTDIR\Installer.exe" install'
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\TestLSP" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestLSP" "DisplayName" "NSIS TestLSP"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestLSP" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestLSP" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestLSP" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
  CreateShortCut "$DESKTOP\UninstallTestLSP.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0  
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\TestLSP"
  DeleteRegKey HKLM SOFTWARE\TestLSP

  ; Stopping components
  Exec '"$INSTDIR\Service.exe" uninstall'
  Exec '"$INSTDIR\Installer.exe" uninstall'

  ; Remove files and uninstaller
  Delete $SYSDIR\LSP.dll
  Delete $INSTDIR\Service.exe
  Delete $INSTDIR\Installer.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove directories used
  RMDir "$SMPROGRAMS\TestLSP"
  RMDir "$INSTDIR"

SectionEnd
