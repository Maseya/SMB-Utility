/**********************************************************************

                                  smb Utility

  File: ini.c
  Description: レジストリ、INIファイルアクセスのためのルーチン
  Description: Registry, routines for INI file access
  History:

 *********************************************************************/
#include <windows.h>
#include <windowsx.h>
#include "ini.h"
#include "cmnlib.h"
 /********************

   レジストリアクセス

   Registry Access

 *********************/

 /*************************************
 dwType
  REG_BINARY 任意の形式のバイナリデータ
  REG_DWORD  32ビット値
  REG_SZ     文字列

  REG_BINARY Binary data in any format
  REG_DWORD  32 bit value
  REG_SZ     character string
 **************************************/
BOOL WriteToRegistry(LPTSTR lpValueName, DWORD dwType, LPVOID lpData, DWORD dwSize)
{
    HKEY hRegKey;
    BOOL blRet = TRUE;

    RegCreateKeyEx(HKEY_CURRENT_USER, REGROOTPATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL);
    if (!hRegKey) return FALSE;
    if (ERROR_SUCCESS != RegSetValueEx(hRegKey, lpValueName, 0, dwType, lpData, dwSize))
        blRet = FALSE;
    RegCloseKey(hRegKey);
    return blRet;
}

/*************************************
dwType
 REG_BINARY 任意の形式のバイナリデータ
 REG_DWORD  32ビット値
 REG_SZ     文字列

 REG_BINARY Binary data in any format
 REG_DWORD  32 bit value
 REG_SZ     character string
**************************************/
BOOL ReadFromRegistry(LPTSTR lpValueName, DWORD dwType, LPVOID lpData, DWORD dwSize)
{
    HKEY hRegKey;
    DWORD dwTypeRet;
    BOOL blRet = TRUE;
    LONG lResult;

    RegCreateKeyEx(HKEY_CURRENT_USER, REGROOTPATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL);
    if (!hRegKey) return FALSE;
    lResult = RegQueryValueEx(hRegKey, lpValueName, NULL, &dwTypeRet, lpData, &dwSize);
    if (lResult != ERROR_SUCCESS || dwType != dwTypeRet) blRet = FALSE;
    RegCloseKey(hRegKey);
    return blRet;
}

BOOL DeleteRegistryEntries()
{
    if (ERROR_SUCCESS != RegDeleteKey(HKEY_CURRENT_USER, REGROOTPATH))
        return FALSE;
    return TRUE;
}

static BOOL CheckFileExistance(LPTSTR lpFile)
{
    return (0xFFFFFFFF != GetFileAttributes(lpFile)) ? TRUE : FALSE;
}

size_t GetAppPathName(LPTSTR lpBuffer, int iBufferSize, LPTSTR lpFileName)
{
    TCHAR FullPath[MAX_PATH];
    LPTSTR pt, p;
    size_t cb;

    // NOTE : コマンドプロンプトから実行された場合、入力されたコマンド文字列がそのまま取得される。
    //        例えば、WinIPSの置かれているディレクトリ内からコマンドプロンプトで"winips"と起動すると、
    //        "winips"が取得されるので、以下の方法ではだめ。
    // p = GetCommandLine();
    // 実行可能ファイルのフルパス名を取得する
    // NOTE: When executed from the command prompt, the input command character string is acquired as it is.
    // For example, if you start "winips" at the command prompt from within the directory where WinIPS is located,
    // "winips" is acquired, so it can not be done in the following way.
    // p = GetCommandLine();
    // Get the full path name of the executable file
    GetModuleFileName(GetModuleHandle(NULL), FullPath, MAX_PATH);
    p = FullPath;

    while (*p == __T('"'))
        p = CharNext(p);

    // "を除いた先頭へのポインタ
    // Pointer to the beginning without \"
    pt = p;

    // マルチバイト文字数を得る
    // get multibyte character count
    for (; *p && *p != __T('"'); p = CharNext(p))
    {
        // 終端へ"を探す
        // Search for "to the end
        continue;
    }

    for (; pt < p && *(CharPrev(pt, p)) != __T('\\'); p = CharPrev(pt, p))
    {
        // 終端から\を探す
        // Look for \ from the end
        continue;
    }

    // バイト数
    // Number of bytes
    cb = (p - pt) * sizeof(TCHAR);

    // + NULL文字
    // + NULL character
    if (iBufferSize <= cb + (int)sizeof(TCHAR))
        return 0;

    memcpy(lpBuffer, pt, cb);

    lpBuffer[cb / sizeof(TCHAR)] = __T('\0');

    // ファイル名の指定があれば、それをコピー
    // Copy the file name if specified
    if (lpFileName)
    {
        if (iBufferSize <= cb + (int)sizeof(TCHAR) + (int)_tcslen(lpFileName))
            return 0;
        lstrcat(lpBuffer, lpFileName);
    }

    return cb / sizeof(TCHAR);
}

UINT GetIntegerFromINI(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault)
{
    TCHAR szPath[MAX_PATH];
    GetAppPathName(szPath, MAX_PATH, INIFILE_FILENAME);
    return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, szPath);
}
