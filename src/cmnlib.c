/************************************************************************************

                                  smb Utility

  File: cmnlib.c
  Description:
  History:

 ************************************************************************************/
#include <windows.h>
#include <shlwapi.h>
#ifdef WIN64
#include <intrin.h>
#endif
#include "cmnlib.h"

TCHAR g_szTempStringBuffer[TMPSTRBUFSIZ];
TCHAR g_szTempStringBuffer2[TMPSTRBUFSIZ];

BOOL IsMMXAvailable()
{
    SYSTEM_INFO si;
    BOOL blReturn = FALSE;

    GetSystemInfo(&si);

    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        int cpuid[4];
        __cpuid(cpuid, 1);

        if (cpuid[3] & 0x00800000)
            blReturn = TRUE;
    }
    return blReturn;
}

BOOL IsCommonControlSupported(DWORD dwVersion)
{
    HINSTANCE hComCtl;
    BOOL fRet;
    DWORD dwInstalledVersion = 0;

    //load the DLL
    hComCtl = LoadLibrary(TEXT("comctl32.dll"));
    if (hComCtl)
    {
        HRESULT           hr = S_OK;
        DLLGETVERSIONPROC pDllGetVersion;
        /*
        You must get this function explicitly because earlier versions of the DLL
        don't implement this function. That makes the lack of implementation of the
        function a version marker in itself.
        */
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hComCtl, TEXT("DllGetVersion"));
        if (pDllGetVersion)
        {
            DLLVERSIONINFO    dvi;
            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
            hr = (*pDllGetVersion)(&dvi);
            if (SUCCEEDED(hr))
            {
                if (dvi.dwMajorVersion == 4)
                {
                    switch (dvi.dwMinorVersion)
                    {
                    case 70: dwInstalledVersion = COMCTRL_V470; break;
                    case 71: dwInstalledVersion = COMCTRL_V471; break;
                    case 72: dwInstalledVersion = COMCTRL_V472; break;
                    }
                }
                else
                {
                    if (dvi.dwMajorVersion > 4)
                        dwInstalledVersion = COMCTRL_LATER;
                }
            }
        }
        else
        {
            /*
            If GetProcAddress failed, then the DLL is a version previous to the one
            shipped with IE 3.x.
            */
            dwInstalledVersion = COMCTRL_V400;
        }

        fRet = (dwVersion <= dwInstalledVersion) ? TRUE : FALSE;

        FreeLibrary(hComCtl);
        return fRet;
    }
    return FALSE;
}

void MyCheckMenuItem(HWND hWnd, UINT uItemID, UINT uCheck)
{
    MENUITEMINFO mii;

    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STATE;
    if (uCheck)
        mii.fState |= MFS_CHECKED;
    else
        mii.fState &= ~MFS_CHECKED;
    SetMenuItemInfo(GetMenu(hWnd), uItemID, FALSE, &mii);
}

void MySetMenuItemText(HWND hWnd, UINT uItemID, LPTSTR lpText)
{
    MENUITEMINFO mii;

    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_TYPE;
    mii.fType = MFT_STRING;
    mii.dwTypeData = lpText;
    SetMenuItemInfo(GetMenu(hWnd), uItemID, FALSE, &mii);
}

LPTSTR GetTempStringBuffer()
{
    g_szTempStringBuffer[0] = 0; return g_szTempStringBuffer;
}

LPTSTR GetTempStringBuffer2()
{
    g_szTempStringBuffer2[0] = 0; return g_szTempStringBuffer2;
}
