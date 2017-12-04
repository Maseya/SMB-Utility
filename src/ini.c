/************************************************************************************

                                  smb Utility

  File: ini.c
  Description: レジストリ、INIファイルアクセスのためのルーチン
  History:

 ************************************************************************************/
#include <windows.h>
#include <windowsx.h>
#include "ini.h"
#include "cmnlib.h"
 /********************

   レジストリアクセス

 *********************/

 /*************************************
 dwType
  REG_BINARY 任意の形式のバイナリデータ
  REG_DWORD  32ビット値
  REG_SZ     文字列
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

/**********************

 **********************/

static BOOL CheckFileExistance(LPTSTR lpFile)
{
    return (0xFFFFFFFF != GetFileAttributes(lpFile)) ? TRUE : FALSE;
}

int GetAppPathName(LPTSTR lpBuffer, int iBufferSize, LPTSTR lpFileName)
{
    TCHAR FullPath[MAX_PATH];
    LPTSTR pt, p;
    int cb;

    // NOTE : コマンドプロンプトから実行された場合、入力されたコマンド文字列がそのまま取得される。
    //        例えば、WinIPSの置かれているディレクトリ内からコマンドプロンプトで"winips"と起動すると、
    //        "winips"が取得されるので、以下の方法ではだめ。
    // p = GetCommandLine();
    // 実行可能ファイルのフルパス名を取得する
    GetModuleFileName(GetModuleHandle(NULL), FullPath, MAX_PATH);
    p = FullPath;

    while (*p == '"')
        p = CharNext(p);
    pt = p; // "を除いた先頭へのポインタ

    // マルチバイト文字数を得る
    for (; *p && *p != '"'; p = CharNext(p)); // 終端へ"を探す
    for (; pt < p && *(CharPrev(pt, p)) != '\\'; p = CharPrev(pt, p)); // 終端から\を探す

    //
    cb = p - pt; // バイト数
    if (iBufferSize <= cb + (int)sizeof(TCHAR)) // + NULL文字
        return 0;

    //
    memcpy(lpBuffer, pt, cb);

    //
    *(LPTSTR)((LPBYTE)lpBuffer + cb) = '\0';

    // ファイル名の指定があれば、それをコピー
    if (lpFileName)
    {
        if (iBufferSize <= cb + (int)sizeof(TCHAR) + (int)strlen(lpFileName))
            return 0;
        lstrcat(lpBuffer, lpFileName);
    }

    return cb;

    /*
        TCHAR FullPath[MAX_PATH];
        LPTSTR lpCmdLine;
        int iCopySize;
        int iDirNameSize;//カレントディレクトリの名前のサイズ
        int iFileNameSize;//

        if(!lpBuffer) return 0;

        lpCmdLine = GetCommandLine();

        while(*lpCmdLine == '"') lpCmdLine++;
        for(iDirNameSize = 0;;iDirNameSize++) {
            if((*(lpCmdLine + iDirNameSize) == '\0')
                ||(*(lpCmdLine + iDirNameSize) == '"')
                ||(*(lpCmdLine + iDirNameSize)==' ')) {
                memcpy(FullPath, lpCmdLine, iDirNameSize * sizeof(TCHAR));
                FullPath[iDirNameSize] = 0;
                if(CheckFileExistance(FullPath))
                    break;
            }
        }

        for(;iDirNameSize >= 0;iDirNameSize--)if(*(lpCmdLine + iDirNameSize - 1) == '\\')break;

        if(lpFileName)
        {
            for(iFileNameSize = 0;;iFileNameSize++) {
                if(*(lpFileName + iFileNameSize) == '\0')
                    break;
            }
        }
        else
            iFileNameSize = 0;

        memcpy(FullPath, lpCmdLine, iDirNameSize * sizeof(TCHAR));
        FullPath[iDirNameSize] = 0;
        if(lpFileName) memcpy(FullPath + iDirNameSize, lpFileName, iFileNameSize);

        iCopySize = (iBufferSize < iDirNameSize + iFileNameSize) ? iBufferSize : iDirNameSize + iFileNameSize;

        memcpy(lpBuffer, FullPath, iCopySize * sizeof(TCHAR));
        lpBuffer[iCopySize] = 0;

        return iCopySize;
    */
}

UINT GetIntegerFromINI(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault)
{
    TCHAR szPath[MAX_PATH];
    GetAppPathName(szPath, MAX_PATH, INIFILE_FILENAME);
    return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, szPath);
}
