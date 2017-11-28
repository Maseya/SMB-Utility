# Microsoft Developer Studio Project File - Name="smbutil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=smbutil - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "smbutil_i.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "smbutil_i.mak" CFG="smbutil - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "smbutil - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "smbutil - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "smbutil - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INTERNATIONAL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib advapi32.lib winmm.lib comctl32.lib imm32.lib libcmt.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib

!ELSEIF  "$(CFG)" == "smbutil - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /WX /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "INTERNATIONAL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib advapi32.lib winmm.lib comctl32.lib imm32.lib libcmtd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /pdbtype:sept

!ENDIF 

# Begin Target

# Name "smbutil - Win32 Release"
# Name "smbutil - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cmnlib.c
# End Source File
# Begin Source File

SOURCE=.\emubgset.c
# End Source File
# Begin Source File

SOURCE=.\emuengine.c
# End Source File
# Begin Source File

SOURCE=.\emuutil.c
# End Source File
# Begin Source File

SOURCE=.\filemng.c
# End Source File
# Begin Source File

SOURCE=.\frame.c
# End Source File
# Begin Source File

SOURCE=.\ini.c
# End Source File
# Begin Source File

SOURCE=.\keyaccel.c
# End Source File
# Begin Source File

SOURCE=.\keys.c
# End Source File
# Begin Source File

SOURCE=.\logview.c
# End Source File
# Begin Source File

SOURCE=.\M6502.asm

!IF  "$(CFG)" == "smbutil - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - creating M6502.obj...
IntDir=.\Release
WkspDir=.
InputPath=.\M6502.asm

"$(IntDir)\m6502.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(WkspDir)\bin\nasm.exe" -f win32 -o $(IntDir)\m6502.obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "smbutil - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - creating M6502.obj...
IntDir=.\Debug
WkspDir=.
InputPath=.\M6502.asm

"$(IntDir)\m6502.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(WkspDir)\bin\nasm.exe" -f win32 -o $(IntDir)\m6502.obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objeditcom.c
# End Source File
# Begin Source File

SOURCE=.\objeditdlg.c
# End Source File
# Begin Source File

SOURCE=.\objlib.c
# End Source File
# Begin Source File

SOURCE=.\objlist.c
# End Source File
# Begin Source File

SOURCE=.\objmng.c
# End Source File
# Begin Source File

SOURCE=.\objview.c
# End Source File
# Begin Source File

SOURCE=.\objviewbmp.c
# End Source File
# Begin Source File

SOURCE=.\objwndcmn.c
# End Source File
# Begin Source File

SOURCE=.\resource_i.rc
# End Source File
# Begin Source File

SOURCE=.\roommng.c
# End Source File
# Begin Source File

SOURCE=.\roomseldlg.c
# End Source File
# Begin Source File

SOURCE=.\tools.c
# End Source File
# Begin Source File

SOURCE=.\undo.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cmnlib.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\emubgset.h
# End Source File
# Begin Source File

SOURCE=.\emulator.h
# End Source File
# Begin Source File

SOURCE=.\emuutil.h
# End Source File
# Begin Source File

SOURCE=.\filemng.h
# End Source File
# Begin Source File

SOURCE=.\frame.h
# End Source File
# Begin Source File

SOURCE=.\ini.h
# End Source File
# Begin Source File

SOURCE=.\keyaccel.h
# End Source File
# Begin Source File

SOURCE=.\keys.h
# End Source File
# Begin Source File

SOURCE=.\logview.h
# End Source File
# Begin Source File

SOURCE=.\M6502.h
# End Source File
# Begin Source File

SOURCE=.\nespal.h
# End Source File
# Begin Source File

SOURCE=.\objdata.h
# End Source File
# Begin Source File

SOURCE=.\objeditcom.h
# End Source File
# Begin Source File

SOURCE=.\objlib.h
# End Source File
# Begin Source File

SOURCE=.\objlist.h
# End Source File
# Begin Source File

SOURCE=.\objmng.h
# End Source File
# Begin Source File

SOURCE=.\objview.h
# End Source File
# Begin Source File

SOURCE=.\objviewbmp.h
# End Source File
# Begin Source File

SOURCE=.\objwndcmn.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\roommng.h
# End Source File
# Begin Source File

SOURCE=.\roomseldlg.h
# End Source File
# Begin Source File

SOURCE=.\smbutil.h
# End Source File
# Begin Source File

SOURCE=.\string_i.h
# End Source File
# Begin Source File

SOURCE=.\strings.h
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# Begin Source File

SOURCE=.\undo.h
# End Source File
# Begin Source File

SOURCE=.\versinfo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\assist_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\assist_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\castle_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\emulator.ico
# End Source File
# Begin Source File

SOURCE=.\image\lf_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\maplisti.ico
# End Source File
# Begin Source File

SOURCE=.\image\mapview.ico
# End Source File
# Begin Source File

SOURCE=.\data\ram1.bin
# End Source File
# Begin Source File

SOURCE=.\data\ram2.bin
# End Source File
# Begin Source File

SOURCE=.\data\ram3.bin
# End Source File
# Begin Source File

SOURCE=.\image\rg_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\rom.ico
# End Source File
# Begin Source File

SOURCE=.\image\sea_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\sky_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\smbutil.ico
# End Source File
# Begin Source File

SOURCE=.\image\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\image\tvmask.bmp
# End Source File
# Begin Source File

SOURCE=.\image\ug_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\xgauge_img.bmp
# End Source File
# Begin Source File

SOURCE=.\image\ygauge_img.bmp
# End Source File
# End Group
# Begin Group "Document Files"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\misc\history.txt
# End Source File
# Begin Source File

SOURCE=.\document\international.txt
# End Source File
# Begin Source File

SOURCE=.\misc\readme.txt
# End Source File
# Begin Source File

SOURCE=.\document\todo.txt
# End Source File
# End Group
# End Target
# End Project
