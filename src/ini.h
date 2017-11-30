/************************************************************************************

                                  smb Utility

  File: ini.h
  Description:
  History:

 ************************************************************************************/
#ifndef INI_H
#define INI_H

 //
#define REGROOTPATH "Software\\smbUtil"

//
#define INI_MDIFRAME_WINDOW            "MDIFRAME_WNDPL"
#define INI_OBJECTLIST_WINDOW          "OBJLIST_WNDPL"
#define INI_MDIFRAME_OTHERSETTING      "MDIFRAME_OTHER"
#define INI_MDIFRAME_FILEPATH          "MDIFRAME_FILEPATH"
#define MDIFRAME_OTHERSETTING_TOOLBAR    0x00000001
#define MDIFRAME_OTHERSETTING_STATUSBAR  0x00000002
#define MDIFRAME_OTHERSETTING_MSGONSAVE  0x00010000
#define INI_OBJECTLIST_COLUMNWIDTH0   "OBJLIST_COLWIDTH0"
#define INI_OBJECTLIST_COLUMNWIDTH1   "OBJLIST_COLWIDTH1"
#define INI_OBJECTLIST_COLUMNWIDTH2   "OBJLIST_COLWIDTH2"
#define INI_OBJECTLIST_COLUMNWIDTH3   "OBJLIST_COLWIDTH3"
#define INI_OBJECTVIEW_WINDOW         "OBJVIEW_WNDPL"
#define INI_EMULATOR_WINDOW           "EMULATOR_WNDPL"
#define INI_EMULATOR_SETTING          "EMULATOR_SETTING"
#define INI_EMULATOR_KEYS             "EMULATOR_KEYS"
#define INI_EMULATOR_JOYBUTTONS       "EMULATOR_JOYBUTTONS"
#define INI_EDITOR_KEYS               "EDITOR_KEYS"
#define INI_EDITOR_WHEEL              "EDITOR_WHEEL"
#define INI_OBJECTVIEW_SETTING         "OBJVIEW_SETTING"

// HIWORD(N):高さ LOWORD(N):設定
#define LOGVIEW_SHOWLOGVIEW            0x00000001
#define INI_LOGVIEW_SETTING            "LOGVIEW_SETTING"

//#define INI_LOGVIEW_BORDER             "LOGVIEW_BORDERPL"
//#define INI_LOGVIEW_LOGVIEW            "LOGVIEW_LOGVIEWPL"

#define INI_APP_ORIGINALROMPATH       "APP_ORIGINALROMPATH"

#define INI_INSTALL_DIR                "INSTALL_DIR"

BOOL WriteToRegistry(LPSTR lpValueName, DWORD dwType, LPVOID lpData, DWORD dwSize);
BOOL ReadFromRegistry(LPSTR lpValueName, DWORD dwType, LPVOID lpData, DWORD dwSize);

// アンインストール時に呼び出す
BOOL DeleteRegistryEntries();

#define INIFILE_FILENAME              "smbutil.ini"
#define INIFILE_TESTPLAY              "TESTPLAY"
#define INIFILE_TESTPLAY_HARD         "HARD"
#define INIFILE_TESTPLAY_STATE        "STATE"
#define INIFILE_TESTPLAY_BADGUYS      "BADGUYS"
#define INIFILE_TESTPLAY_PAGE         "PAGE"
#define INIFILE_TESTPLAY_INVINCIBLE   "INVINCIBLE"
#define INIFILE_EDITOR                "EDITOR"
#define INIFILE_EDITOR_PROTECT        "PROTECT"

UINT  GetIntegerFromINI(LPCTSTR, LPCTSTR, INT);

#endif /* INI_H */
