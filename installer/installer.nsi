;Includes
!include "MUI.nsh"  ;Modern look
!include "winmessages.nsh"	;Windows messages

;Program information
!define PROG_NAME "NyuFX"
!define PROG_VERSION "1.6"
!define PROG_AUTHOR "Youka"
!define PROG_URL "http://forum.youka.de"
!define PROG_YEAR "2013"

;Directories
!define SOURCEDIR ".."
InstallDir "$PROGRAMFILES\${PROG_NAME}"
!define UNINSTALL_REG "Software\Microsoft\Windows\CurrentVersion\Uninstall"

;Compression
SetCompress Auto
SetCompressor /SOLID lzma
SetCompressorDictSize 32
SetDatablockOptimize On

;Installer
Name "${PROG_NAME} ${PROG_VERSION}"
OutFile "${PROG_NAME}${PROG_VERSION}_setup.exe"
!define MUI_ICON "${SOURCEDIR}\src\resources\NyuFX.ico"
VIProductVersion "${PROG_VERSION}.0.0"
VIAddVersionKey  "ProductName" "${PROG_NAME}"
VIAddVersionKey  "ProductVersion" "${PROG_VERSION}.0.0"
VIAddVersionKey  "CompanyName" "${PROG_AUTHOR}"
VIAddVersionKey  "LegalCopyright" "${PROG_AUTHOR}; ${PROG_YEAR}"
VIAddVersionKey  "FileDescription" "${PROG_NAME} Installer"
VIAddVersionKey  "FileVersion" "${PROG_VERSION}.0.0"
VIAddVersionKey  "OriginalFilename" "$EXEPATH"

;Page attributes
!define MUI_WELCOMEFINISHPAGE_BITMAP "NyuFX_banner.bmp"
!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN "$INSTDIR\NyuFX.exe"
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
ShowInstDetails show
ShowUnInstDetails show

;Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${SOURCEDIR}\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;Locale attributes
!insertmacro MUI_LANGUAGE "English"
XPStyle on
RequestExecutionLevel admin

;Install
Section ""
  ;Program
  SetOutPath "$INSTDIR\docs"
  File /r "${SOURCEDIR}\docs\*.*"
  SetOutPath "$INSTDIR\include"
  File /r "${SOURCEDIR}\include\*.*"
  SetOutPath "$INSTDIR\lang"
  File /r "${SOURCEDIR}\lang\*.*"
  CreateDirectory "$INSTDIR\templates"
  CreateDirectory "$INSTDIR\tools"
  SetOutPath "$INSTDIR"
  File "${SOURCEDIR}\LICENSE"
  File "${SOURCEDIR}\NyuFX.exe"
  WriteUninstaller "$INSTDIR\uninstall.exe"
  ;Links
  CreateShortCut "$SENDTO\NyuFX.lnk" "$INSTDIR\NyuFX.exe"
  CreateDirectory "$SMPROGRAMS\${PROG_NAME}"
  CreateShortCut "$SMPROGRAMS\${PROG_NAME}\NyuFX.lnk" "$INSTDIR\NyuFX.exe"
  CreateShortCut "$SMPROGRAMS\${PROG_NAME}\uninstall.lnk" "$INSTDIR\uninstall.exe"
  ;System uninstall entry
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "DisplayIcon" "$INSTDIR\NyuFX.exe"
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "DisplayName" "${PROG_NAME}"
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "DisplayVersion" "${PROG_VERSION}.0"
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "Publisher" "${PROG_AUTHOR}"  
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "URLInfoAbout" "${PROG_URL}"
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "HelpLink" "${PROG_URL}"
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "NoModify" 1
  WriteRegStr HKLM "${UNINSTALL_REG}\${PROG_NAME}" "NoRepair" 1
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  BringToFront
SectionEnd

;Components
Section "Shortcut" s1
  CreateShortCut "$DESKTOP\NyuFX.lnk" "$INSTDIR\NyuFX.exe"
SectionEnd

Section "Examples" s2
  SetOutPath "$INSTDIR\templates"
  File /r "${SOURCEDIR}\templates\*.*"
SectionEnd

Section "ASSDraw3" s3
  SetOutPath "$INSTDIR\tools"
  File "${SOURCEDIR}\tools\ASSDraw3.exe"
  File "${SOURCEDIR}\tools\ASSDraw3.chm"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${s1} "Create a desktop shortcut"
  !insertmacro MUI_DESCRIPTION_TEXT ${s2} "Add example scripts"
  !insertmacro MUI_DESCRIPTION_TEXT ${s3} "Add tool 'ASSDraw3'"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall
Section "Uninstall"
  ;Program
  RMDir /r "$INSTDIR"
  ;Links
  Delete "$DESKTOP\NyuFX.lnk"
  Delete "$SENDTO\NyuFX.lnk"
  RMDir /r "$SMPROGRAMS\${PROG_NAME}"
  ;System uninstall entry
  DeleteRegKey HKLM "${UNINSTALL_REG}\${PROG_NAME}"
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  BringToFront
SectionEnd 