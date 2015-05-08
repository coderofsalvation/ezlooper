; EZ Looper.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install EZ Looper.nsi into a directory that the user selects,

;--------------------------------
!include "zipdll.nsh"
!include "MUI2.nsh"

; The name of the installer
Name "EZ Looper"

; The file to write
OutFile "EZ Looper (core setup).exe"

; The default installation directory
InstallDir "$PROGRAMFILES\EZ Looper"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_EZ Looper" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings
!define MUI_ABORTWARNING
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "EZ Looper.bmp" ; optional
;!define MUI_ABORTWARNING



;--------------------------------

; Pages
 	!insertmacro MUI_PAGE_LICENSE "EZ Looper.txt" 
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "EZ Looper core" CORE
  ;SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
	File "ezlooper.zip"	

	ZipDLL::extractall "$INSTDIR\ezlooper.zip" "$INSTDIR"
	Delete "ezlooper.zip" 
 
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\NSIS_EZ Looper" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZ Looper" "DisplayName" "NSIS EZ Looper"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZ Looper" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZ Looper" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZ Looper" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  SetOutPath $INSTDIR  
  CreateDirectory "$SMPROGRAMS\EZ Looper"
  CreateShortCut "$SMPROGRAMS\EZ Looper\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$DESKTOP\EZ Looper.lnk" "$INSTDIR\ezlooper.exe" "" "" 0
  CreateShortCut "$SMPROGRAMS\EZ Looper\EZ Looper.lnk" "$INSTDIR\ezlooper.exe" "" "" 0
  
SectionEnd

; --------------------------------
; Descriptions

LangString DESC_CORE ${LANG_ENGLISH} "This will install the Latest EZ Loopercore. Also the latest plugins which are very usefull for MIDI live-performances."
LangString DESC_EXAMPLE ${LANG_ENGLISH} "This will install the example sounds, uf you don't have any sounds you should do this."
LangString DESC_MIDV ${LANG_ENGLISH} "This installs HUBI's LoopBack -or MidiYoke to let programs take advantage of accessing the same mididevice...copies installer to desktop."
LangString DESC_MIXM ${LANG_ENGLISH} "P.Doom's MIDI Driver for the usb DJ-controller from MIXMAN DM2..copies installer to desktop."
LangString DESC_EZ 	 ${LANG_ENGLISH} "A bangin EZ Looper liveset...copies to desktop"
LangString DESC_ASIO ${LANG_ENGLISH} "The famous ASIO 4ALL audiodrivers"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;----BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${CORE} $(DESC_CORE)
;----END
!insertmacro MUI_FUNCTION_DESCRIPTION_END



; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EZ Looper"
  DeleteRegKey HKLM "SOFTWARE\NSIS_EZ Looper"

  ; Remove files and uninstaller
  Delete "$INSTDIR\EZ Looper.nsi"
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\EZ Looper\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\EZ Looper"
  RMDir "$INSTDIR"

SectionEnd
