/************************************************************************************

                                  smb Utility

  File: logview.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "cmnlib.h"
#include "common.h"
#include "logview.h"
#include "ini.h"

#define BORDERWNDCLASSNAME  __T("BORDER")
#define LOGVIEWWNDCLASSNAME __T("LOGVIEW")

#define LOGVIEW_EDITBORDER 2
#define LOGVIEW_EDITID     7777

 //Windows 95 and Windows 98: For single-line edit controls,
 //the text limit is either 0x7FFE bytes or the value of the
 //wParam parameter, whichever is smaller. For multiline edit
 //controls, this value is either 0xFFFF bytes or the value of
 //the wParam parameter, whichever is smaller.
#define LOGVIEW_BUFSIZ     0x100

// GUI
HWND g_hBorderWnd;
HWND g_hLogViewWnd;
HFONT g_hEditFont;
HWND g_hEditWnd;
WORD g_nLogViewBorderPos;
BOOL g_fShowLogView;

// バッファ管理
TCHAR g_szLogBuffer[LOGVIEW_BUFSIZ];

static BOOL AdjustLogView(WORD wBorderPos)
{
    UINT w, h, sh = 0;
    RECT rc;
    HWND hWndParent = GetParent(g_hBorderWnd), hWndSB = fr_GetStatusBar();

    if (!g_hBorderWnd || !g_hLogViewWnd)
        return FALSE;

    GetClientRect(hWndParent, &rc);
    h = rc.bottom - rc.top;
    w = rc.right - rc.left;
    MoveWindow(g_hBorderWnd, 0, wBorderPos, w, LOGVIEW_BORDERWIDTH, TRUE);

    //
    if (hWndSB)
    {
        GetWindowRect(hWndSB, &rc);
        sh = rc.bottom - rc.top;
    }
    MoveWindow(g_hLogViewWnd, 0, wBorderPos + LOGVIEW_BORDERWIDTH, w, h - (wBorderPos + sh + LOGVIEW_BORDERWIDTH), TRUE);

    return TRUE;
}

LRESULT CALLBACK BorderWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    {
        SetCapture(hWnd);
    }
    break;
    case WM_LBUTTONUP:
    {
        POINT pt;

        //
        ReleaseCapture();

        //
        GetCursorPos(&pt);
        ScreenToClient(GetParent(hWnd), &pt);
        AdjustLogView((WORD)pt.y);
        g_nLogViewBorderPos = (WORD)pt.y;

        //
        PostMessage(GetParent(hWnd), WM_SIZE, 0, 0);
    }
    break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LogViewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CTLCOLORSTATIC:
    {
        return (long)GetSysColorBrush(COLOR_WINDOW);
    }
    break;
    case WM_SIZE:
    {
        RECT rc;

        GetClientRect(hWnd, &rc);
        MoveWindow(g_hEditWnd, LOGVIEW_EDITBORDER, LOGVIEW_EDITBORDER, rc.right - LOGVIEW_EDITBORDER, rc.bottom - LOGVIEW_EDITBORDER, TRUE);
    }
    break;
    case WM_CREATE:
    {
        g_hEditWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                                    __T("EDIT"),
                                    NULL,
                                    WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_CLIPSIBLINGS |
                                    ES_MULTILINE | ES_READONLY | ES_OEMCONVERT | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                    0, 0,
                                    1, 1,
                                    hWnd,
                                    (HMENU)LOGVIEW_EDITID,
                                    GetModuleHandle(NULL),
                                    NULL);

        SendMessage(g_hEditWnd, WM_SETFONT, (WPARAM)g_hEditFont, MAKELPARAM(TRUE, 0));

        SendMessage(g_hEditWnd, EM_SETLIMITTEXT, LOGVIEW_BUFSIZ, 0);
    }
    break;
    case WM_DESTROY:
    {
        DestroyWindow(g_hEditWnd);
    }
    break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

VOID lv_Trash()
{
    DWORD dwSetting;
    WORD  wFlags;

    wFlags = 0;
    wFlags |= (g_fShowLogView) ? LOGVIEW_SHOWLOGVIEW : 0;
    dwSetting = MAKELONG(wFlags, g_nLogViewBorderPos);
    WriteToRegistry(INI_LOGVIEW_SETTING, REG_DWORD, &dwSetting, sizeof(DWORD));

    lv_DestroyLogView();

    if (g_hEditFont)
    {
        DeleteObject(g_hEditFont);
        g_hEditFont = NULL;
    }
}

BOOL lv_Initialize()
{
    WNDCLASSEX wcx;
    LOGFONT lf;
    DWORD dwSetting;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    //
    g_hBorderWnd = NULL;
    g_hLogViewWnd = NULL;
    g_nLogViewBorderPos = LOGVIEW_DEFAULTHEIGHT;
    g_fShowLogView = TRUE;
    g_szLogBuffer[0] = 0;

    ZeroMemory(&wcx, sizeof(WNDCLASSEX));

    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.lpfnWndProc = (WNDPROC)BorderWndProc;
    wcx.hInstance = hInstance;
    wcx.hIcon =
        wcx.hIconSm = NULL;
    wcx.lpszMenuName = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_SIZENS);
    wcx.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wcx.lpszClassName = BORDERWNDCLASSNAME;

    if (!RegisterClassEx(&wcx)) return FALSE;

    ZeroMemory(&wcx, sizeof(WNDCLASSEX));

    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.lpfnWndProc = (WNDPROC)LogViewWndProc;
    wcx.hInstance = hInstance;
    wcx.hIcon =
        wcx.hIconSm = NULL;
    wcx.lpszMenuName = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wcx.lpszClassName = LOGVIEWWNDCLASSNAME;

    if (!RegisterClassEx(&wcx)) return FALSE;

    //
    if (ReadFromRegistry(INI_LOGVIEW_SETTING, REG_DWORD, &dwSetting, sizeof(DWORD)))
    {
        g_nLogViewBorderPos = HIWORD(dwSetting);
        g_fShowLogView = (dwSetting & LOGVIEW_SHOWLOGVIEW) ? TRUE : FALSE;
    }

    //
    ZeroMemory(&lf, sizeof(LOGFONT));
    lf.lfHeight = LOGVIEW_FONTHEIGHT;
    lf.lfPitchAndFamily = FIXED_PITCH;
#ifndef INTERNATIONAL
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lstrcat(lf.lfFaceName, LOGVIEW_FONTNAME);
#else
    lf.lfCharSet = ANSI_CHARSET;
    lstrcat(lf.lfFaceName, LOGVIEW_FONTNAMEI); // lstrcat(lf.lfFaceName, __T("Arial"));
#endif
    g_hEditFont = CreateFontIndirect(&lf);

    return TRUE;
}

VOID lv_DestroyLogView()
{
    if (g_hBorderWnd)
    {
        DestroyWindow(g_hBorderWnd);
        g_hBorderWnd = NULL;
    }
    if (g_hLogViewWnd)
    {
        DestroyWindow(g_hLogViewWnd);
        g_hLogViewWnd = NULL;
    }
    g_fShowLogView = FALSE;
}

static void SetLogViewTexts()
{
    int lines, index;

    SetWindowText(g_hEditWnd, g_szLogBuffer);

    lines = SendMessage(g_hEditWnd, EM_GETLINECOUNT, 0, 0);
    if (lines > 0)
    {
        SendMessage(g_hEditWnd, EM_LINESCROLL, 0, lines);
        index = SendMessage(g_hEditWnd, EM_LINEINDEX, lines - 1, 0);
        SendMessage(g_hEditWnd, EM_SETSEL, index, index);
    }
}

HWND lv_CreateLogView(HWND hWndParent, int nHeight)
{
    HWND hWnd;
    RECT rc;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    GetClientRect(hWndParent, &rc);

    if (nHeight <= 0 || nHeight + LOGVIEW_BORDERWIDTH > rc.bottom - rc.top)
        nHeight = g_nLogViewBorderPos;

    hWnd = CreateWindowEx(0,
                          BORDERWNDCLASSNAME,
                          STRING_EMPTY,
                          WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                          0, nHeight,
                          rc.right - rc.left, LOGVIEW_BORDERWIDTH,
                          hWndParent,
                          NULL,
                          hInstance,
                          NULL);

    g_hBorderWnd = hWnd;

    hWnd = CreateWindowEx(0,
                          LOGVIEWWNDCLASSNAME,
                          NULL,
                          WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                          0, nHeight + LOGVIEW_BORDERWIDTH,
                          rc.right - rc.left, 1,
                          hWndParent,
                          NULL,
                          hInstance,
                          NULL);

    g_hLogViewWnd = hWnd;

    //
    AdjustLogView((WORD)nHeight);

    //
    SetLogViewTexts();

    //
    g_fShowLogView = TRUE;

    return hWnd;
}

BOOL lv_OutputString(LPTSTR szText, DWORD dwFlag)
{
    UINT nNewLen, nValidLen, nCR, nNeedLen;

    if (!szText || szText[0] == 0) return FALSE;

    // 単位は、byte
    nNewLen = lstrlen(szText);
    nValidLen = lstrlen(g_szLogBuffer) + 1;

    nCR = (dwFlag & LOGVIEW_OUTPUTSTRING_CR) ? 2 : 0;

    // 新しい文字列が最大バッファを超えている場合
    if (nNewLen + nCR + 1 >= LOGVIEW_BUFSIZ)
    {
        g_szLogBuffer[0] = 0;
        nNewLen = LOGVIEW_BUFSIZ - nCR;//
        lstrcpyn(g_szLogBuffer, szText, nNewLen);
    }

    // 新しい文字列を格納するバッファが足りない場合
    else if (nValidLen + nNewLen + nCR > LOGVIEW_BUFSIZ)
    {
        nNeedLen = (nNewLen + nCR) - (LOGVIEW_BUFSIZ - nValidLen);
        MoveMemory(g_szLogBuffer, g_szLogBuffer + nNeedLen, nValidLen - nNeedLen);
        lstrcpyn(g_szLogBuffer + nValidLen - nNeedLen - 1, szText, nNewLen + 1);
    }

    // 新しい文字列を格納するバッファが足りている場合
    else
    {
        lstrcat(g_szLogBuffer, szText);
    }

    if (dwFlag & LOGVIEW_OUTPUTSTRING_CR)
    {
        lstrcat(g_szLogBuffer, __T("\x0D\x0A"));
    }

    if (g_hEditWnd)
        SetLogViewTexts();

    if (dwFlag & LOGVIEW_OUTPUTSTRING_BEEP)
        MessageBeep(MB_OK);

    return TRUE;
}

BOOL lv_OutputDebugString(LPTSTR szText)
{
    return lv_OutputString(szText, LOGVIEW_OUTPUTSTRING_CR);
}

VOID lv_AdjustLogView()
{
    AdjustLogView(g_nLogViewBorderPos);
    if (g_hLogViewWnd)
        PostMessage(GetParent(g_hLogViewWnd), WM_SIZE, 0, 0);
}

BOOL lv_ShowLogView()
{
    return g_fShowLogView;
}

HWND lv_GetBorderWnd()
{
    return g_hBorderWnd;
}

HWND lv_GetLogViewWnd()
{
    return g_hLogViewWnd;
}
