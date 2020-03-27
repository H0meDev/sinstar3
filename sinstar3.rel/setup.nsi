; Script generated by the HM NIS Edit Script Wizard.
Unicode true
; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "启程输入法"
!define PRODUCT_VERSION "3.0"
!define PRODUCT_PUBLISHER "启程软件"
!define PRODUCT_WEB_SITE "http://soime.cn"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "sinstar3.ICO"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

SetPluginUnload alwaysoff
Var /GLOBAL bUpdate


; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "License.rtf"
; Components page
!define MUI_PAGE_HEADER_TEXT "组件选择"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW fun_comp
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!define MUI_PAGE_CUSTOMFUNCTION_SHOW fun_dir
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\使用说明.txt"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_SHOWREADME_TEXT "阅读使用说明"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"

function fun_dir
StrCmp $bUpdate "1" 0 Exit
;禁用浏览按钮
FindWindow $0 "#32770" "" $HWNDPARENT
GetDlgItem $0 $0 1001
EnableWindow $0 0
;禁止编辑目录
FindWindow $0 "#32770" "" $HWNDPARENT
GetDlgItem $0 $0 1019
EnableWindow $0 0
Exit:
FunctionEnd

function fun_comp
StrCmp $bUpdate "1" 0 Exit
FindWindow $0 "#32770" "" $HWNDPARENT
GetDlgItem $0 $0 1032
EnableWindow $0 0
Exit:
FunctionEnd

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Sinstar3.exe"
InstallDir "$PROGRAMFILES\sinstar3"
ShowInstDetails show
ShowUnInstDetails show

Section "核心程序" SEC_CORE
  SectionIn RO
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  File /a "register.exe"
  File /a "RegisterCore.dll"
  File /a "license.rtf"
  File /a "使用说明.txt"
  File /a "license.rtf"
  SetOutPath "$INSTDIR\program"
  File /r "program\*.*"
  SetOutPath "$INSTDIR\defskin"
  File /r "defskin\*.*"
  SetOutPath "$INSTDIR\sound"
  File /r "sound\*.*"
  SetOutPath "$INSTDIR\data"
  File /r "data\*.*"

  SetOutPath "$INSTDIR\server"
  File /r "server\blur.ini"
  File /r "server\config.ini"
  File /r "server\default.spl"
  File /r "server\en-ch.flm"
  File /r "server\line.dat"
  File /r "server\spell.dat"
  File /r "server\spell.pit"
  File /r "server\symbol.txt"
  File /r "server\usercmd.ud"
  File /r "server\userdef.ud"
  File /r "server\wordrate.dat"
SectionEnd

Section "扩展皮肤" SEC_SKIN
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\skins"
   File /r "skins\*.*"
SectionEnd
Section "辅助工具" SEC_TOOLS
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\tools"
   File /a "tools\CompBuilder.exe"
   File /a "tools\pleditor.exe"
SectionEnd
SectionGroup /e "!编码方案" SEC_COMP
Section "五笔86" COMP_WB86
SectionIn RO
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\server"
   File /a "server\wb86.cit"
   File /a "server\wb86.pit"
   File /a "server\wb86_GBK.cit"
SectionEnd
Section "五笔98" COMP_WB98
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\server"
   File /a "server\五笔98.cit"
SectionEnd
Section "五笔2000" COMP_WB2k
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\server"
   File /a "server\新世纪.cit"
SectionEnd
Section "表形码26" COMP_BXM26
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\server"
   File /a "server\表形26.cit"
SectionEnd
Section "表形码31" COMP_BXM31
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\server"
   File /a "server\表形31符.cit"
SectionEnd
Section "郑码6.6" COMP_ZM66
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\server"
   File /a "server\郑码GBK.cit"
   File /a "server\郑码GBK.scm"
SectionEnd
Section "权氏两笔" COMP_QS2B
   SetOverwrite ifnewer
   SetOutPath "$INSTDIR\server"
   File /a "server\权氏两笔.cit"
SectionEnd
SectionGroupEnd


Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\启程输入法\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\启程输入法\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  System::Call '$PLUGINSDIR\RegisterCore::Sinstar_InitW(t) ("$INSTDIR")'

  StrCmp $bUpdate "1" upgrade install
  upgrade:
  System::Call '$PLUGINSDIR\RegisterCore::Sinstar_Update()'
  Goto finish

  install:
  System::Call '$PLUGINSDIR\RegisterCore::Sinstar_Install()'
  Goto finish
  finish:

  SetPluginUnload manual
  System::Free 0
  
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\server\isserver3.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_CORE} "启程输入法核心文件。"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMP} "输入法码表"
  !insertmacro MUI_DESCRIPTION_TEXT ${COMP_WB86} "五笔字型86版"
  !insertmacro MUI_DESCRIPTION_TEXT ${COMP_WB98} "五笔字型98版"
  !insertmacro MUI_DESCRIPTION_TEXT ${COMP_WB2K} "五笔字型2000版"
  !insertmacro MUI_DESCRIPTION_TEXT ${COMP_BXM26} "表型码26键版本"
  !insertmacro MUI_DESCRIPTION_TEXT ${COMP_BXM31} "表型码31键版本"
  !insertmacro MUI_DESCRIPTION_TEXT ${COMP_ZM66} "郑码6.6版本"
  !insertmacro MUI_DESCRIPTION_TEXT ${COMP_QS2B} "权氏两笔"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_SKIN} "扩展皮肤，美化界面。"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_TOOLS} "包含码表编辑器，词库编辑器"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
;StrCpy $bUpdate "1"
;Goto Run
   InitPluginsDir
   SetOutPath $PLUGINSDIR
   File "RegisterCore.dll"
   File "Program\isserver3.exe"
   
   StrCpy $bUpdate "0"
   System::Call '$PLUGINSDIR\RegisterCore::Sinstar_IsRunning()i.R0'
   IntCmp $R0 1 0 no_run
   MessageBox MB_OKCANCEL|MB_ICONQUESTION  "安装程序检测到 ${PRODUCT_NAME} 正在运行。$\r$\n$\r$\n点击 “确定”查看调用程序，$\r$\n$\r$\n点击 “取消”退出安装程序。" IDOK 0 IDCANCEL Exit
   System::Call '$PLUGINSDIR\RegisterCore::Sinstar_ShowCaller(i1)i.R0'
   Goto Exit
   no_run:
   System::Call '$PLUGINSDIR\RegisterCore::Sinstar_GetCurrentVer2(*i,*i,*i,*i)i (.r0,.r1,.r2,.r3).r4'
   IntCmp $4 1 0 Run
   System::Call '$PLUGINSDIR\RegisterCore::Sinstar_PEVersion2W(t,*i,*i,*i,*i)i ("$PLUGINSDIR\isserver3.exe",.R0,.R1,.R2,.R3).R4'
   IntCmp $R0 $0 0 Degrade Upgrade
   IntCmp $R1 $1 0 Degrade Upgrade
   IntCmp $R2 $2 0 Degrade Upgrade
   IntCmp $R3 $3 0 Degrade Upgrade
   MessageBox MB_OK|MB_ICONSTOP  "您已经安装了${PRODUCT_NAME} $0.$1.$2.$3 。$\r$\n$\r$\n点击 “确定” 退出安装程序。" IDOK Exit
   Upgrade:
   MessageBox MB_OKCANCEL|MB_ICONSTOP  "确定升级${PRODUCT_NAME} $0.$1.$2.$3 到$R0.$R1.$R2.$R3吗?。$\r$\n$\r$\n点击 “确定” 升级，“取消”退出安装程序。" IDOK 0 IDCANCEL Exit
   StrCpy $bUpdate "1"
   ;get install dir
   StrCpy $1 ${NSIS_MAX_STRLEN}          ; assign memory to $0
   System::Call '$PLUGINSDIR\RegisterCore::Sinstar_GetInstallDir(t, *i) i(.r0, r1r1).r2'
   IntCmp $2 1 0 Run
   StrCpy $INSTDIR $0
   
   ;auto select sections, only program files and def skin will be updated.
   IntOp $1 ${SF_SELECTED} ~

   SectionGetFlags ${SEC_CORE} $R0
   IntOp $0 $R0 | ${SF_SELECTED}
   SectionSetFlags ${SEC_CORE} $0
   
   SectionGetFlags ${SEC_SKIN} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${SEC_SKIN} $0
   
   SectionGetFlags ${SEC_TOOLS} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${SEC_TOOLS} $0

   SectionGetFlags ${COMP_WB86} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${COMP_WB86} $0
   SectionGetFlags ${COMP_WB98} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${COMP_WB98} $0
   SectionGetFlags ${COMP_WB2K} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${COMP_WB2K} $0

   SectionGetFlags ${COMP_BXM26} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${COMP_BXM26} $0
   
   SectionGetFlags ${COMP_BXM31} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${COMP_BXM31} $0
   SectionGetFlags ${COMP_ZM66} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${COMP_ZM66} $0

   SectionGetFlags ${COMP_QS2B} $R0
   IntOp $0 $R0 & $1
   SectionSetFlags ${COMP_QS2B} $0

   Goto Run
   Degrade:
   MessageBox MB_OK|MB_ICONSTOP  "您已经安装了${PRODUCT_NAME} $0.$1.$2.$3 不能降级到$R0.$R1.$R2.$R3。$\r$\n$\r$\n点击 “确定” 退出安装程序。"
   Exit:
   Quit
   Run:
   System::Call '$PLUGINSDIR\RegisterCore::Sinstar_QuitServer()'
FunctionEnd

Var /Global bForceUninstall

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从你的计算机移除。"
FunctionEnd

Function un.onInit
   InitPluginsDir
   SetOutPath $PLUGINSDIR
   File "RegisterCore.dll"

  StrCpy $bForceUninstall "0"
  System::Call '$PLUGINSDIR\RegisterCore::Sinstar_InitW(t) ("$INSTDIR")'
  System::Call '$PLUGINSDIR\RegisterCore::Sinstar_IsRunning() i.R0'
  IntCmp $R0 0 uninstall 0
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(^Name) 正在使用，确定卸载吗？" IDYES 0 IDNO Exit
  StrCpy $bForceUninstall "1"
  Goto unreg
  uninstall:
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，其及所有的组件？" IDYES unreg
  Exit:
  Abort
  unreg:
  StrCmp $bForceUninstall "1" 0 unreg2
  System::Call '$PLUGINSDIR\RegisterCore::Sinstar_ForceUninstall()'
  SetRebootFlag true
  Goto end
  unreg2:
  System::Call '$PLUGINSDIR\RegisterCore::Sinstar_Uninstall()'
  end:
FunctionEnd

Section Uninstall
  RMDir /r "$INSTDIR"
  RMDir /r "$SMPROGRAMS\启程输入法"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true
SectionEnd
