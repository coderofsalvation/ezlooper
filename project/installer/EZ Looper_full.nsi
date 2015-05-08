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
OutFile "EZ Looper (full setup).exe"

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

; The stuff to install
Section "Example samples liveset" EXAMPLE
  ;SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
	File "examplesounds.zip"	

	ZipDLL::extractall "$INSTDIR\examplesounds.zip" "$INSTDIR"
	Delete "examplesounds.zip" 

  ; Set output path to the installation directory.
  SetOutPath $DESKTOP
  
  ; Put file there
  File "exampleliveset.zip"	

	ZipDLL::extractall "$DESKTOP\exampleliveset.zip" "$DESKTOP"
	Delete "exampleliveset.zip" 
  MessageBox MB_OK "Samples are stored in '$INSTDIR\FX'\n\n and the liveset is stored on your desktop as '$DESKTOP\ezlooper_example_liveset'"
  SetOutPath $INSTDIR  
  
SectionEnd

; Optional section (can be disabled by the user)
SectionGroup "Drivers (optional)"



	Section /o "MIDI Virtual devices" MIDV
  	SetOutPath $DESKTOP
		MessageBox MB_YESNO|MB_ICONQUESTION "Do you run WinXP or higher?" IDYES XP IDNO OLD

		OLD:
  	SetOutPath $TEMP
		File "g:\sqz_cfg\win98\mdlbk251.zip"
		ZipDLL::extractall "$TEMP\mdlbk251.zip" "$TEMP"
		ExecWait "$TEMP\INST95.BAT"
		Delete "$TEMP\mdlbk251.zip"
		goto END

		; XP 
		XP:
		MessageBox MB_OK "1) Please install to for ex. c:\temp 2) Goto configurationscreen -> add Hardware 3) choose 'new device' and install manually 'Driver for audio,video and games' 4) point to c:\temp directory and you are done!"
  	SetOutPath $TEMP
		File "g:\sqz_cfg\win2k_msx\tools\myokent.exe"
		ExecWait "$TEMP\myokent.exe"
		Delete "$TEMP\myokent.exe"
		END:
	SectionEnd

	Section "DM2 USB Driver for Windows XP" USB
  	SetOutPath $TEMP
		File "G:\sqz_coding\src\EZ Looper\installer\dm2-ultimate-usb-driver.zip"
		ZipDLL::extractall "$TEMP\dm2-ultimate-usb-driver.zip" "$DESKTOP"
		delete "$TEMP\dm2-ultimate-usb-driver.zip"
		MessageBox MB_OK "DM2 usb driver for XP installed in folder 'dm2' on your desktop. So please go to this folder and follow instructions"
	SectionEnd
	
	Section /o "ASIO4ALL Drivers" ASIO
  	SetOutPath $TEMP
		File "g:\sqz_cfg\win2k_msx\tools\ASIO4ALL_2_7_English.exe"
		ExecWait "$TEMP\ASIO4ALL_2_7_English.exe"
		Delete "$TEMP\ASIO4ALL_2_7_English.exe"
	SectionEnd

	Section "Mixman DM2MIDI utility" MIXM
  	SetOutPath $TEMP
		File "g:\sqz_cfg\win2k_msx\drivers\DM2MIDI_1.2_B7.exe"
		ExecWait "$TEMP\DM2MIDI_1.2_B7.exe"
		Delete "$TEMP\DM2MIDI_1.2_B7.exe"
	SectionEnd
SectionGroupEnd

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
LangString DESC_MIDV ${LANG_ENGLISH} "This installs HUBI's LoopBack -or MidiYoke to let programs take advantage of accessing the same mididevice...copies installer to desktop..if you want to control EZ Looper with the DM2 controller you'll need this."
LangString DESC_MIXM ${LANG_ENGLISH} "P.Doom's MIDI Driver for the usb DJ-controller from MIXMAN DM2..if you want to use the DM2 controller with EZ Looper you'll need this."
LangString DESC_EZ 	 ${LANG_ENGLISH} "A bangin EZ Looper liveset...copies to desktop"
LangString DESC_ASIO ${LANG_ENGLISH} "The famous fast ASIO 4ALL audiodrivers"
LangString DESC_USB ${LANG_ENGLISH} "The DM2 controller is not immediately recognized by XP when plugged into USB. This is the usb driver which needs to be installed."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;----BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${CORE} $(DESC_CORE)
!insertmacro MUI_DESCRIPTION_TEXT ${EXAMPLE} $(DESC_EXAMPLE)
!insertmacro MUI_DESCRIPTION_TEXT ${MIDV} $(DESC_MIDV)
!insertmacro MUI_DESCRIPTION_TEXT ${MIXM} $(DESC_MIXM)
!insertmacro MUI_DESCRIPTION_TEXT ${EZ} 	$(DESC_EZ)
!insertmacro MUI_DESCRIPTION_TEXT ${ASIO}	$(DESC_ASIO)
!insertmacro MUI_DESCRIPTION_TEXT ${USB}	$(DESC_USB)
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
