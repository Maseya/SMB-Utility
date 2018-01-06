/************************************************************************************

                                  smb Utility

  File: ini.h
  Description:
  History:

 ************************************************************************************/
#ifndef INI_H
#define INI_H

#include <tchar.h>

#define REGROOTPATH                      __T("Software\\smbUtil")

#define INI_MDIFRAME_WINDOW              __T("MDIFRAME_WNDPL")
#define INI_OBJECTLIST_WINDOW            __T("OBJLIST_WNDPL")
#define INI_MDIFRAME_OTHERSETTING        __T("MDIFRAME_OTHER")
#define INI_MDIFRAME_FILEPATH            __T("MDIFRAME_FILEPATH")
#define MDIFRAME_OTHERSETTING_TOOLBAR    0x00000001
#define MDIFRAME_OTHERSETTING_STATUSBAR  0x00000002
#define MDIFRAME_OTHERSETTING_MSGONSAVE  0x00010000
#define INI_OBJECTLIST_COLUMNWIDTH0      __T("OBJLIST_COLWIDTH0")
#define INI_OBJECTLIST_COLUMNWIDTH1      __T("OBJLIST_COLWIDTH1")
#define INI_OBJECTLIST_COLUMNWIDTH2      __T("OBJLIST_COLWIDTH2")
#define INI_OBJECTLIST_COLUMNWIDTH3      __T("OBJLIST_COLWIDTH3")
#define INI_OBJECTVIEW_WINDOW            __T("OBJVIEW_WNDPL")
#define INI_EMULATOR_WINDOW              __T("EMULATOR_WNDPL")
#define INI_EMULATOR_SETTING             __T("EMULATOR_SETTING")
#define INI_EMULATOR_KEYS                __T("EMULATOR_KEYS")
#define INI_EMULATOR_JOYBUTTONS          __T("EMULATOR_JOYBUTTONS")
#define INI_EDITOR_KEYS                  __T("EDITOR_KEYS")
#define INI_EDITOR_WHEEL                 __T("EDITOR_WHEEL")
#define INI_OBJECTVIEW_SETTING           __T("OBJVIEW_SETTING")

 // HIWORD(N):高さ LOWORD(N):設定
 // HIWORD (N): height LOWORD (N): setting
#define LOGVIEW_SHOWLOGVIEW            0x00000001
#define INI_LOGVIEW_SETTING              __T("LOGVIEW_SETTING")

//#define INI_LOGVIEW_BORDER             __T("LOGVIEW_BORDERPL")
//#define INI_LOGVIEW_LOGVIEW            __T("LOGVIEW_LOGVIEWPL")

#define INI_APP_ORIGINALROMPATH          __T("APP_ORIGINALROMPATH")

#define INI_INSTALL_DIR                  __T("INSTALL_DIR")

BOOL WriteToRegistry(LPTSTR lpValueName, DWORD dwType, LPVOID lpData, DWORD dwSize);
BOOL ReadFromRegistry(LPTSTR lpValueName, DWORD dwType, LPVOID lpData, DWORD dwSize);

// アンインストール時に呼び出す
// Call during uninstallation
BOOL DeleteRegistryEntries();

#define INIFILE_FILENAME                 __T("smbutil.ini")
#define INIFILE_TESTPLAY                 __T("TESTPLAY")
#define INIFILE_TESTPLAY_HARD            __T("HARD")
#define INIFILE_TESTPLAY_STATE           __T("STATE")
#define INIFILE_TESTPLAY_BADGUYS         __T("BADGUYS")
#define INIFILE_TESTPLAY_PAGE            __T("PAGE")
#define INIFILE_TESTPLAY_INVINCIBLE      __T("INVINCIBLE")
#define INIFILE_EDITOR                   __T("EDITOR")
#define INIFILE_EDITOR_PROTECT           __T("PROTECT")

UINT  GetIntegerFromINI(LPCTSTR, LPCTSTR, INT);

#endif /* INI_H */
