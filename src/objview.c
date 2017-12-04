/************************************************************************************

                                  smb Utility

  File: objview.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "objview.h"
#include "emulator.h"
#include "emuutil.h"
#include "roommng.h"
#include "objlib.h"
#include "objmng.h"
#include "objlist.h"
#include "ini.h"
#include "objviewbmp.h"
#include "objwndcmn.h"
#include "keys.h"

#define ID_OBJVIEW_OBJSELECT_BASE 8000

#define MAPVIEWWNDCLASSNAME __T("MAPVIEWWND")
#define IDW_YPOSWINDOW      1000
#define IDW_XPOSWINDOW      1001
#define IDW_REWVPAGEWINDOW  6000
#define IDW_FWDPAGEWINDOW   6001
#define OBJVIEW_CHARACTER_WIDTH 16
#define CHARCTERS_PER_PAGE      16
#define OBJVIEW_COLOR_BADGUYS_INACTIVE RGB(0,127,127)
#define OBJVIEW_COLOR_BADGUYS_ACTIVE   RGB(0,255,255)
#define OBJVIEW_COLOR_MAP_INACTIVE     RGB(0,127,0)
#define OBJVIEW_COLOR_MAP_ACTIVE       RGB(0,255,0)

#define TOP_16PIXEL      16
#define LABEL_FONTSIZE   8
#define LABEL_WHITESPACE 12
#define YPOSLABEL_WIDTH  21
#define XPOSLABEL_HEIGHT 19
#define XPOSLABEL_WIDTH  OBJVIEW_CHARACTER_WIDTH*CHARCTERS_PER_PAGE
#define MAPVIEW_CLIENTHEIGHT (OBJVIEW_CHARACTER_WIDTH*17)
#define MAPVIEW_WINDOWHEIGHT (MAPVIEW_CLIENTHEIGHT+XPOSLABEL_HEIGHT+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYSIZEFRAME)*2)
#define MAPVIEW_VISIBLEHEIGHT (OBJVIEW_CHARACTER_WIDTH*14)
#define ONE_PAGE_SIZE (NES_SCREENSIZEX+GetSystemMetrics(SM_CXSIZEFRAME)*2-1+YPOSLABEL_WIDTH)

#define OBJVIEW_BADGUYSHARD_RECTWIDTH 8

extern BOOL MapEditCommand(WORD wCommand);

HWND ghMapViewWnd = NULL;
HWND ghToolTip = NULL;
HWND ghYPosWnd = NULL;
HDC     ghMemdcYPos = NULL;
HBITMAP ghBitmapYPos = NULL;
HBITMAP ghPrevBitmapYPos = NULL;
HWND ghXPosWnd = NULL;
HDC     ghMemdcXPos = NULL;
HBITMAP ghBitmapXPos = NULL;
HBITMAP ghPrevBitmapXPos = NULL;
HWND ghRPBtnWnd = NULL;
HWND ghFPBtnWnd = NULL;
HDC     ghMemdcMapViewWnd = NULL;
HBITMAP ghBitmapMapViewWnd = NULL;
HBITMAP ghPrevBitmapMapViewWnd = NULL;
int giCursorX = -1;
int giCursorY = -1;

//マップビューの左端に表示されているマップのページ
int giMapViewPageBase = 0;

//マップビューの左端を0ページとしたときのカーソルのあるページ
int giMapViewEditPage = 0;

//カーソルのあるマップのページ
int giMapViewPage = 0;

//ウインドウで表示可能なページ数(０起算)
int giWndPages;

//
int g_iCursoleEnable = 0;

BOOL g_blIsXPosLineDraw = FALSE;
BOOL g_blIsYPosLineDraw = FALSE;

BOOL g_fNotDrawAssistBmp = FALSE;

//BOOL g_fNoObjSelectPopup = FALSE;

/*****************************

******************************/
LRESULT CALLBACK ObjectViewOptionDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        BOOL fCheck;
        LPPROPSHEETPAGE lpPropPage = (LPPROPSHEETPAGE)lParam;

        if (lpPropPage->lParam)
            CenterPropatySheet(hDlg);

        fCheck = g_fNotDrawAssistBmp ? BST_CHECKED : BST_UNCHECKED;
        CheckDlgButton(hDlg, IDC_NOTDRAWASSISTBMP, fCheck);

        //			   fCheck = g_fNoObjSelectPopup ? BST_CHECKED : BST_UNCHECKED;
        //			   CheckDlgButton(hDlg, IDC_NOOBJSELECTPOPUP, fCheck);
    }
    break;
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        switch (pnmh->code)
        {
        case PSN_APPLY:
        {
            g_fNotDrawAssistBmp = FALSE;
            if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_NOTDRAWASSISTBMP))
                g_fNotDrawAssistBmp = TRUE;

            //				   g_fNoObjSelectPopup = FALSE;
            //				   if(BST_CHECKED==IsDlgButtonChecked(hDlg, IDC_NOOBJSELECTPOPUP))
            //					   g_fNoObjSelectPopup = TRUE;
            return TRUE;
        }
        break;
        }
    }
    break;
    }

    return FALSE;
}

DWORD ObjectViewGetSetting()
{
    DWORD dwSetting = 0;

    if (g_fNotDrawAssistBmp)
        dwSetting |= OBJVIEWSETTING_NOTDRAWASSISTBMP;

    if (g_blIsXPosLineDraw)
        dwSetting |= OBJVIEWSETTING_DRAWXPOSLINE;

    if (g_blIsYPosLineDraw)
        dwSetting |= OBJVIEWSETTING_DRAWYPOSLINE;

    //	if (g_fNoObjSelectPopup)
    //		dwSetting |= OBJVIEWSETTING_NOPOPUP;

    return dwSetting;
}

void ObjectViewSetSetting(DWORD dwSetting)
{
    g_fNotDrawAssistBmp = (dwSetting & OBJVIEWSETTING_NOTDRAWASSISTBMP) ? TRUE : FALSE;
    g_blIsXPosLineDraw = (dwSetting & OBJVIEWSETTING_DRAWXPOSLINE) ? TRUE : FALSE;
    g_blIsYPosLineDraw = (dwSetting & OBJVIEWSETTING_DRAWYPOSLINE) ? TRUE : FALSE;

    //	g_fNoObjSelectPopup = (dwSetting & OBJVIEWSETTING_NOPOPUP) ? TRUE : FALSE;
}

/*************************

**************************/
void InitMapViewGlobalValue()
{
    giMapViewPage = 0;
    giCursorX = -1;
    giCursorY = -1;
    g_iCursoleEnable = 0;
    giMapViewPage = 0;
    giMapViewEditPage = 0;
    giMapViewPageBase = 0;
}

/*************************

**************************/
static void RefreshMapViewWindowTitle()
{
    TCHAR cWndTitle[50];
    TCHAR cCurWndTitle[50];

    if (!ghMapViewWnd) return;

    if (gblIsROMLoaded)
    {
        GetWindowText(ghMapViewWnd, cCurWndTitle, 49);
        if (giWndPages)
            wsprintf(cWndTitle, STRING_OBJVIEW_TITLE2, STRING_WINDOW_OBJVIEW, giMapViewPageBase, giMapViewPageBase + giWndPages);
        else
            wsprintf(cWndTitle, STRING_OBJVIEW_TITLE, STRING_WINDOW_OBJVIEW, giMapViewPageBase);
        if (lstrcmp(cCurWndTitle, cWndTitle))
            SetWindowText(ghMapViewWnd, cWndTitle);
    }
    else
        SetWindowText(ghMapViewWnd, STRING_WINDOW_OBJVIEW);
}

/***********************

************************/
int ShowMapViewCursole(BOOL blShow)
{
    if (blShow)
    {
        g_iCursoleEnable++;
        if (g_iCursoleEnable > 0) g_iCursoleEnable = 0;
    }
    else
    {
        g_iCursoleEnable--;
    }
    return g_iCursoleEnable;
}

static BOOL IsMapViewCursoleVisible()
{
    BOOL blRet;

    blRet = (g_iCursoleEnable < 0) ? FALSE : TRUE;

    return blRet;
}

void EnsureMapViewCursoleVisible()
{
    g_iCursoleEnable = 0;
}

static void SetMapViewPage(int iPage)
{
    giMapViewPage = iPage;
}

static void SetMapViewPageBase(int iPage)
{
    giMapViewEditPage = 0;
    giMapViewPageBase = giMapViewPage = iPage;
}

static void SetMapViewCursor(int iCursorX, int iCursorY)
{
    if (iCursorX < 0)
    {
        iCursorX = 0x10 + iCursorX;
        if (giMapViewPage > 0) giMapViewPage--;
    }
    giCursorX = iCursorX;
    giCursorY = iCursorY;
}

BOOL ShadeRect(HDC hDC, int x, int y, COLORREF  crHighlightColor)
{
    COLORREF  crOldBkColor, crOldTextColor;
    HBRUSH    hBrush, hOldBrush;
    HBITMAP   hBitmap, hBrushBitmap, hOldMemBitmap;
    int       OldBkMode, nWidth, nHeight;
    HDC       hMemDC;
    RECT      rcRect = {0, 0, 0, 0}, rcDstRect;

    // The bitmap bits are for a monochrome "every-other-pixel"
    //     bitmap (for a pattern brush)
    WORD      Bits[8] = {0x0055, 0x00aa, 0x0055, 0x00aa,
                          0x0055, 0x00aa, 0x0055, 0x00aa};

    // The Width and Height of the target area
    SetRect(&rcDstRect, x, y, x + OBJVIEW_CHARACTER_WIDTH, y + OBJVIEW_CHARACTER_WIDTH);
    nWidth = OBJVIEW_CHARACTER_WIDTH + 1;
    nHeight = OBJVIEW_CHARACTER_WIDTH + 1;

    // Need a pattern bitmap
    hBrushBitmap = CreateBitmap(8, 8, 1, 1, &Bits);

    // Need to store the original image
    hBitmap = CreateCompatibleBitmap(hDC, nWidth, nHeight);

    // Need a memory DC to work in
    hMemDC = CreateCompatibleDC(hDC);

    // Create the pattern brush
    hBrush = CreatePatternBrush(hBrushBitmap);

    // Has anything failed so far? If so, abort!
    if ((hBrushBitmap == NULL) || (hBitmap == NULL) ||
        (hMemDC == NULL) || (hBrush == NULL))
    {
        if (hBrushBitmap != NULL) DeleteObject(hBrushBitmap);
        if (hBitmap != NULL) DeleteObject(hBitmap);
        if (hMemDC != NULL) DeleteDC(hMemDC);
        if (hBrush != NULL) DeleteObject(hBrush);
        return FALSE;
    }

    // Select the bitmap into the memory DC
    hOldMemBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // How wide/tall is the original?
    rcRect.right = nWidth;
    rcRect.bottom = nHeight;

    // Lay down the pattern in the memory DC
    FillRect(hMemDC, &rcRect, hBrush);

    // Fill in the non-color pixels with the original image
    BitBlt(hMemDC, 0, 0, nWidth, nHeight, hDC,
           x, y, SRCAND);

    // Set the color scheme
    crOldTextColor = SetTextColor(hDC, crHighlightColor);
    crOldBkColor = SetBkColor(hDC, RGB(0, 0, 0));
    OldBkMode = SetBkMode(hDC, OPAQUE);

    // Select the pattern brush
    hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

    // Fill in the color pixels, and set the others to black
    FillRect(hDC, &rcDstRect, hBrush);

    // Fill in the black ones with the original image
    BitBlt(hDC, x, y, nWidth, nHeight,
           hMemDC, 0, 0, SRCPAINT);

    // Restore target DC settings
    SetBkMode(hDC, OldBkMode);
    SetBkColor(hDC, crOldBkColor);
    SetTextColor(hDC, crOldTextColor);

    // Clean up
    SelectObject(hMemDC, hOldMemBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    DeleteObject(hBrushBitmap);
    SelectObject(hDC, hOldBrush);
    DeleteObject(hBrush);

    return TRUE;
}

static void DrawMapViewCursor(int iMode)
{
    HDC hdc;
    int x, y;
    COLORREF crColor;

    if (!gblIsROMLoaded) return;

    y = XPOSLABEL_HEIGHT + TOP_16PIXEL + giCursorY * 16;
    x = YPOSLABEL_WIDTH + giCursorX * 16 + giMapViewEditPage * 256;
    crColor = (!iMode) ? OBJVIEW_COLOR_MAP_ACTIVE : OBJVIEW_COLOR_BADGUYS_ACTIVE;

    if (hdc = GetDC(ghMapViewWnd))
    {
        ShadeRect(hdc, x, y, crColor);
        ReleaseDC(ghMapViewWnd, hdc);
    }
}

/*********************

**********************/
static void DrawPosLines()
{
#define POSLINE_COLOR_NORMAL COLOR_3DLIGHT
#define POSLINE_COLOR_PAGE   COLOR_HIGHLIGHT
#define POSLINE_WIDTH 1
    HDC hdc;
    RECT rc;
    HPEN hPen, hPenPage, hOldPen;
    int n, iPrevBkMode, iPrevRop;
    COLORREF crPrevBk;

    if (!g_blIsXPosLineDraw && !g_blIsYPosLineDraw) return;

    hPen = CreatePen(PS_SOLID, POSLINE_WIDTH, GetSysColor(POSLINE_COLOR_NORMAL));
    hPenPage = CreatePen(PS_DOT, POSLINE_WIDTH, GetSysColor(POSLINE_COLOR_PAGE));

    GetClientRect(ghMapViewWnd, &rc);
    hdc = ghMemdcMapViewWnd;

    hOldPen = SelectObject(hdc, hPen);
    crPrevBk = SetBkColor(hdc, GetSysColor(POSLINE_COLOR_NORMAL));
    iPrevBkMode = SetBkMode(hdc, OPAQUE);
    iPrevRop = SetROP2(hdc, R2_COPYPEN);

    if (g_blIsXPosLineDraw)
    {
        int x;
        for (x = 15, n = 0; x < rc.right; x += 16, n++)
        {
            if ((n & 0x0f) != 0x0f)
            {
                MoveToEx(hdc, x, 0, NULL);
                LineTo(hdc, x, rc.bottom);
            }
            else
            {
                SelectObject(hdc, hPenPage);
                MoveToEx(hdc, x, 0, NULL);
                LineTo(hdc, x, rc.bottom);
                SelectObject(hdc, hPen);
            }
        }
    }

    if (g_blIsYPosLineDraw)
    {
        int y;
        for (y = 15; y < rc.bottom; y += 16)
        {
            MoveToEx(hdc, 0, y, NULL);
            LineTo(hdc, rc.right, y);
        }
    }

    // restore dc
    SelectObject(hdc, hOldPen);
    SetBkColor(hdc, crPrevBk);
    SetBkMode(hdc, iPrevBkMode);
    SetROP2(hdc, iPrevRop);

    // delete objet
    DeleteObject(hPen);
    DeleteObject(hPenPage);
}

/*********************

**********************/
static void SetColorToGauge(HDC hdc)
{
#define GAUGE_COLORS 2
    RGBQUAD rgbCol[GAUGE_COLORS];
    int iSysColor[GAUGE_COLORS] = {COLOR_WINDOW, COLOR_WINDOWTEXT};
    DWORD dwColor;
    int n;

    for (n = 0; n < GAUGE_COLORS; n++)
    {
        dwColor = GetSysColor(iSysColor[n]);
        rgbCol[n].rgbRed = GetRValue(dwColor);
        rgbCol[n].rgbGreen = GetGValue(dwColor);
        rgbCol[n].rgbBlue = GetBValue(dwColor);
    }

    SetDIBColorTable(hdc, 0, GAUGE_COLORS, rgbCol);
}

static BOOL BitmapBltToDC(HBITMAP hBitmap, HDC hDestMemDC, HWND hWnd, int x, int y)
{
    HDC hMemdc;
    HBITMAP hOldBm;
    BOOL blRet = FALSE;
    BITMAP sBmp;
    DWORD nWidth, nHeight;
    HDC hDC;

    if (!hBitmap) return FALSE;
    if (!GetObject(hBitmap, sizeof(BITMAP), &sBmp)) return FALSE;
    nWidth = sBmp.bmWidth;
    nHeight = sBmp.bmHeight;

    hDC = GetDC(hWnd);
    if (hMemdc = CreateCompatibleDC(hDC))
    {
        if (hOldBm = SelectObject(hMemdc, hBitmap))
        {
            SetColorToGauge(hMemdc);

            BitBlt(hDestMemDC, x, y, nWidth, nHeight, hMemdc, 0, 0, SRCCOPY);
            SelectObject(hMemdc, hOldBm);
            blRet = TRUE;
        }
        DeleteDC(hMemdc);
    }

    ReleaseDC(hWnd, hDC);

    return blRet;
}

void DrawXPosGauge()
{
    RECT rc;
    HBITMAP hBmp;

    rc.top = rc.left = 0;
    rc.bottom = XPOSLABEL_HEIGHT;
    rc.right = XPOSLABEL_WIDTH;

    //
    hBmp = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_XGAUGE_IMG), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADMAP3DCOLORS);
    BitmapBltToDC(hBmp, ghMemdcXPos, ghXPosWnd, 0, 0);
    DeleteObject(hBmp);
}

void DrawYPosGauge()
{
    RECT rc;
    HBITMAP hBmp;

    rc.top = rc.left = 0;
    rc.bottom = MAPVIEW_CLIENTHEIGHT;
    rc.right = YPOSLABEL_WIDTH;

    hBmp = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_YGAUGE_IMG), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_LOADMAP3DCOLORS);
    BitmapBltToDC(hBmp, ghMemdcYPos, ghYPosWnd, 0, 0);
    DeleteObject(hBmp);
}

BOOL PrepareGauge(HWND hWnd, HDC *lphDC, HBITMAP *lphBmp, UINT X, UINT Y, HBITMAP *lphPrevBmp)
{
    HDC hdc;

    hdc = GetDC(hWnd);
    *lphDC = CreateCompatibleDC(hdc);
    *lphBmp = CreateCompatibleBitmap(hdc, X, Y);
    *lphPrevBmp = SelectObject(*lphDC, *lphBmp);
    ReleaseDC(hWnd, hdc);

    return TRUE;
}

void DeleteGauge(HWND hWnd, HDC *lphDC, HBITMAP *lphBmp, HBITMAP *lphPrevBmp)
{
    HDC hdc;
    hdc = GetDC(hWnd);
    SelectObject(hdc, *lphPrevBmp);
    DeleteObject(*lphBmp);
    DeleteDC(*lphDC);
    ReleaseDC(hWnd, hdc);
}

/******************

  ツールチップ

*******************/
HWND CreateObjectViewTooltip()
{
    HWND hwndTT;    // handle of tooltip

    if (!ghMapViewWnd) return NULL;

    hwndTT = CreateWindowEx(0, TOOLTIPS_CLASS,
        (LPTSTR)NULL,
                            0,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            ghMapViewWnd,//この指定は必要！！
                            (HMENU)NULL,
                            GetModuleHandle(NULL),
                            NULL);
    if (!hwndTT) return NULL;

    return hwndTT;
}

BOOL AddToolToObjectViewToolTip(int x, int y, LPTSTR lpText, UINT id)
{
    TOOLINFO ti = {0};
    TTHITTESTINFO ht = {0};

    if (!lpText) return FALSE;

    // オブジェクトが複数重なった位置にある場合の検出、および追加のテキスト
    ht.hwnd = ghMapViewWnd;
    ht.pt.x = x;
    ht.pt.y = y;
    ht.ti.cbSize = sizeof(TOOLINFO);

    if (SendMessage(ghToolTip, TTM_HITTEST, 0, (LPARAM)(LPHITTESTINFO)&ht))
    {
        // ht.ti.uIdにヒットしたツールチップのIDが格納されている
        if (!SendMessage(ghToolTip, TTM_GETTOOLINFO, 0, (LPARAM)(LPTOOLINFO)&ht.ti))
            return FALSE;

        // lpTextは、GetTempStringBuffer()で確保されたバッファー
        ht.ti.lpszText = GetTempStringBuffer2();
        SendMessage(ghToolTip, TTM_GETTEXT, 0, (LPARAM)(LPTOOLINFO)&ht.ti);
        if (lstrlen(lpText) + lstrlen(ht.ti.lpszText) < TMPSTRBUFSIZ)
        {
            lstrcat(ht.ti.lpszText, __T("/"));
            lstrcat(ht.ti.lpszText, lpText);
        }
        lstrcpy(lpText, ht.ti.lpszText);
    }

    // 追加するツールチップ用構造体の初期化
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = 0;
    ti.hwnd = ghMapViewWnd;
    ti.hinst = GetModuleHandle(NULL);
    ti.uId = (UINT)id;
    ti.lpszText = (LPTSTR)lpText;
    ti.rect.left = x;
    ti.rect.top = y;
    ti.rect.right = x + 16;
    ti.rect.bottom = y + 16;

    if (!SendMessage(ghToolTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti))
        return FALSE;

    return TRUE;
}

BOOL DeleteAllToolObjectViewToolTip()
{
    UINT iTools = SendMessage(ghToolTip, TTM_GETTOOLCOUNT, 0, 0);
    UINT t;
    TOOLINFO ti = {0};

    ti.cbSize = sizeof(TOOLINFO);
    ti.hwnd = ghMapViewWnd;
    for (t = 0; t <= iTools; t++)
    {
        ti.uId = t;
        SendMessage(ghToolTip, TTM_DELTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
    }

    return (!SendMessage(ghToolTip, TTM_GETTOOLCOUNT, 0, 0));
}

/*************************

  マップの補助図形を描画

**************************/
void DrawAssistObjectMap()
{
    int x, y, tx, ty, id, uBaseX;
    OBJECTSEEKINFO ObjSeek;
    LPTSTR lpText = GetTempStringBuffer(), lpText2;
    RECT rcInv;

    if (!gblIsROMLoaded
        || giMapViewPageBase < 0
        || giWndPages < 0) return;

    if (!DeleteAllToolObjectViewToolTip())
        return;

    id = 0; tx = ty = -2;

    if (!GetMapEditMode())
    {
        if (MapSeekFirst(&ObjSeek, GETADDRESS_CURRENT_EDITTING))
        {
            for (;;)
            {
                if ((DWORD)giMapViewPageBase <= ObjSeek.dwPage && ObjSeek.dwPage <= (DWORD)giMapViewPageBase + (DWORD)giWndPages + 1)
                {//最後のページの横位置が負のオブジェクトのために必要
                    uBaseX = (ObjSeek.dwPage - giMapViewPageBase >= 0) ? (ObjSeek.dwPage - giMapViewPageBase) * 256 : 0;
                    x = GetMapXPos(ObjSeek.pbData) * 16 + uBaseX;
                    y = TOP_16PIXEL + GetMapYPos(ObjSeek.pbData) * 16;

                    ShadeRect(ghMemdcMapViewWnd, x, y, OBJVIEW_COLOR_MAP_INACTIVE);

                    if (!g_fNotDrawAssistBmp)
                        DrawMapAssistBmpData(&ObjSeek, ghMemdcMapViewWnd, x, y);

                    FormatMapString(ObjSeek.pbData, lpText);
                    lpText2 = GetTempStringBuffer2();
                    if (tx != x || ty != y)
                        wsprintf(lpText2, STRING_OBJVIEW_TOOLTIP, ObjSeek.dwPage, (x - uBaseX) / OBJVIEW_CHARACTER_WIDTH, y / OBJVIEW_CHARACTER_WIDTH - 1, lpText);
                    else
                        lpText2 = lpText;
                    AddToolToObjectViewToolTip(XPOSLABEL_HEIGHT + x, YPOSLABEL_WIDTH + y, lpText2, id++);

                    tx = x;
                    ty = y;
                }
                if (!MapSeekNext(&ObjSeek)) break;
            }
        }
    }
    else
    {
        if (BadGuysSeekFirst(&ObjSeek, GETADDRESS_CURRENT_EDITTING))
        {
            for (;;)
            {
                if ((DWORD)giMapViewPageBase <= ObjSeek.dwPage && ObjSeek.dwPage <= (DWORD)giMapViewPageBase + (DWORD)giWndPages + 1)
                {//
                    uBaseX = (ObjSeek.dwPage - giMapViewPageBase >= 0) ? (ObjSeek.dwPage - giMapViewPageBase) * 256 : 0;
                    x = GetBadGuysXPos(ObjSeek.pbData) * 16 + uBaseX;
                    y = TOP_16PIXEL + (GetBadGuysYPos(ObjSeek.pbData)) * 16;

                    if (BadGuysIsHardObject(ObjSeek.pbData))
                    {
                        SetRect(&rcInv, x, y, x + OBJVIEW_BADGUYSHARD_RECTWIDTH, y + OBJVIEW_CHARACTER_WIDTH);
                        InvertRect(ghMemdcMapViewWnd, &rcInv);
                    }

                    ShadeRect(ghMemdcMapViewWnd, x, y, OBJVIEW_COLOR_BADGUYS_INACTIVE);

                    if (!g_fNotDrawAssistBmp)
                        DrawBadguysAssistBmpData(&ObjSeek, ghMemdcMapViewWnd, x, y);

                    FormatBadGuysString(ObjSeek.pbData, lpText);
                    lpText2 = GetTempStringBuffer2();
                    if (tx != x || ty != y)
                        wsprintf(lpText2, STRING_OBJVIEW_TOOLTIP, ObjSeek.dwPage, (x - uBaseX) / OBJVIEW_CHARACTER_WIDTH, y / OBJVIEW_CHARACTER_WIDTH - 1, lpText);
                    else
                        lpText2 = lpText;
                    AddToolToObjectViewToolTip(XPOSLABEL_HEIGHT + x, YPOSLABEL_WIDTH + y, lpText2, id++);

                    //
                    tx = x;
                    ty = y;
                }
                if (!BadGuysSeekNext(&ObjSeek)) break;
            }
        }
    }
}

BOOL DrawHalfPageMark()
{
    int halfpage, uBaseX;
    ASSISTBMPDATA ABD = {0};

    if (rm_IsSubRoom()) return FALSE;

    halfpage = GetHalfPointPage(rm_GetWorld(), rm_GetArea());

    if (giMapViewPageBase > halfpage
        || giMapViewPageBase + giWndPages < halfpage)
        return FALSE;

    uBaseX = (halfpage - giMapViewPageBase) * 256;

    //
    ABD.iBmpIndex = 3;
    ABD.iPosDeltaX =
        ABD.iPosDeltaY = 0;

    BitBltAssistBmp(ghMemdcMapViewWnd, &ABD, uBaseX, 0);

    return TRUE;
}

/**************************************

  バックバッファからの転送と補助の描画

***************************************/

static void RedrawMapViewWnd()
{
    HDC hdc;

    if (!ghMemdcMapViewWnd || !ghMapViewWnd) return;

    hdc = GetDC(ghMapViewWnd);
    BitBlt(hdc, YPOSLABEL_WIDTH, XPOSLABEL_HEIGHT, GetSystemMetrics(SM_CXSCREEN), MAPVIEW_CLIENTHEIGHT, ghMemdcMapViewWnd, 0, 0, SRCCOPY);
    ReleaseDC(ghMapViewWnd, hdc);

    if (IsMapViewCursoleVisible())
        DrawMapViewCursor(GetMapEditMode());
}

void ClearObjectViewBackBuffer()
{
    RECT rc;

    rc.top = rc.left = 0;
    rc.right = GetSystemMetrics(SM_CXSCREEN);
    rc.bottom = MAPVIEW_CLIENTHEIGHT;

    if (ghMemdcMapViewWnd) FillRect(ghMemdcMapViewWnd, &rc, GetSysColorBrush(COLOR_WINDOW));
}

static void ClearMapViewBackBufferSpecialObjectArea(int iPage)
{
    RECT rc;

    rc.top = 0;
    rc.left = 0;
    rc.right = iPage*NES_SCREENSIZEX;
    rc.bottom = MAPVIEW_CLIENTHEIGHT;

    if (ghMemdcMapViewWnd) FillRect(ghMemdcMapViewWnd, &rc, GetSysColorBrush(COLOR_WINDOW));
}

void UpdateObjectView(DWORD dwUpdateFlag)
{
    RECT rcWnd;
    int n;

    if (!gblIsROMLoaded || !ghMapViewWnd || !ghMemdcMapViewWnd) return;

    //マップビューウインドウの幅を取得
    GetClientRect(ghMapViewWnd, &rcWnd);
    giWndPages = (rcWnd.right - YPOSLABEL_WIDTH) / NES_SCREENSIZEX;

    if (giMapViewPage < giMapViewPageBase)
    {
        giMapViewEditPage = 0;
        giMapViewPageBase = giMapViewPage;
    }
    else if (giMapViewPageBase <= giMapViewPage && giMapViewPage <= giMapViewPageBase + giWndPages)
    {
        giMapViewEditPage = giMapViewPage - giMapViewPageBase;
    }
    else
    {
        giMapViewEditPage = giWndPages;
        giMapViewPageBase = giMapViewPage - giWndPages;
    }

    ClearMapViewBackBufferSpecialObjectArea(giWndPages + 1);

    for (n = 0; n <= giWndPages; n++)
    {
        if (RunEmulatorViewPage(GETADDRESS_CURRENT_EDITTING, giMapViewPageBase + n))
        {
            TransferFromEmuBackBuffer(ghMemdcMapViewWnd, n*NES_SCREENSIZEX, 0, NES_SCREENSIZEX - 1, NES_SCREENSIZEY - 16, FALSE);
        }
    }

    DrawHalfPageMark();

    DrawAssistObjectMap();

    DrawPosLines();

    RedrawMapViewWnd();

    ClearEmuBackBuffer();

    RefreshMapViewWindowTitle();
}

/********************************

*********************************/
void SetMapViewCursoleBadGuys(BYTE *pbBuf, int iPage)
{
    int iCurX;
    int iCurY;

    if (!pbBuf || sizeof(pbBuf) < 2) return;

    iCurX = GetBadGuysXPos(pbBuf);
    iCurY = GetBadGuysYPos(pbBuf);

    SetMapViewPage(iPage);
    SetMapViewCursor(iCurX, iCurY);
}

void SetMapViewCursoleMap(BYTE *pbBuf, int iPage)
{
    if (!pbBuf || sizeof(pbBuf) < 2) return;

    SetMapViewPage(iPage);
    SetMapViewCursor(GetMapXPos(pbBuf), GetMapYPos(pbBuf));
}

void SetObjectViewCursole(int iPage)
{
    int iNewIndex;
    BYTE bBuf[3];

    //
    if (GetMapEditMode())
    {
        iNewIndex = GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING, NULL, iPage, TRUE);
        if (-1 != GetBadGuysData(GETADDRESS_CURRENT_EDITTING, iNewIndex, bBuf, NULL))
        {
            SetMapViewPageBase(iPage);
            SetMapViewCursor(GetBadGuysXPos(bBuf), GetBadGuysYPos(bBuf));
            EnsureMapViewCursoleVisible();
        }
        else
        {
            ShowMapViewCursole(FALSE);
            SetMapViewPage(0);
        }
    }
    else
    {
        iNewIndex = GetMapDataIndex(GETADDRESS_CURRENT_EDITTING, NULL, iPage, TRUE);
        if (-1 != GetMapData(GETADDRESS_CURRENT_EDITTING, iNewIndex, bBuf, NULL))
        {
            SetMapViewPageBase(iPage);
            SetMapViewCursor(GetMapXPos(bBuf), GetMapYPos(bBuf));
            EnsureMapViewCursoleVisible();
        }
        else
        {
            ShowMapViewCursole(FALSE);
            SetMapViewPage(0);
        }
    }
    if (iNewIndex == -1) iNewIndex = 0;

    //giSelectedItem=iNewIndex;
    SetSelectedItem(iNewIndex, TRUE);
}

//マップビューのカーソルの設定
BOOL UpdateObjectViewCursole()
{
    int iPage;
    BYTE bBuf[3];

    if (GetMapEditMode())
    {
        if (-1 != GetBadGuysData(GETADDRESS_CURRENT_EDITTING, GetSelectedIndex(), bBuf, &iPage))
        {
            SetMapViewCursoleBadGuys(bBuf, iPage);
            EnsureMapViewCursoleVisible();
            return TRUE;
        }
        else
        {
            ShowMapViewCursole(FALSE);
            SetMapViewPage(0);
            return FALSE;
        }
    }
    else
    {
        if (-1 != GetMapData(GETADDRESS_CURRENT_EDITTING, GetSelectedIndex(), bBuf, &iPage))
        {
            SetMapViewCursoleMap(bBuf, iPage);
            EnsureMapViewCursoleVisible();
            return TRUE;
        }
        else
        {
            ShowMapViewCursole(FALSE);
            SetMapViewPage(0);
            return FALSE;
        }
    }
}

/***********************

*************************/
void ChangeMapViewScreenSize(int iPages)
{
    RECT rcWnd;
    if (IsZoomed(ghMapViewWnd) || IsIconic(ghMapViewWnd)) return;

    //マップビューウインドウの幅を取得
    GetWindowRect(ghMapViewWnd, &rcWnd);
    SetWindowPos(ghMapViewWnd, NULL, 0, 0, ONE_PAGE_SIZE + NES_SCREENSIZEX*(iPages - 1) - 1, MAPVIEW_WINDOWHEIGHT/*rcWnd.bottom-rcWnd.top*/, SWP_NOMOVE | SWP_NOZORDER);
}

void ShowPopupMenu(HWND hwnd, POINT point, LPTSTR lpMenuName)
{
    HMENU hMenu;
    HMENU hMenuTrackPopup;

    /* Get the menu for the popup from the resource file. */
    hMenu = LoadMenu(GetModuleHandle(NULL), lpMenuName);
    if (!hMenu) return;

    /* Get the first menu in it which we will use for the call to
     * TrackPopup(). This could also have been created on the fly using
     * CreatePopupMenu and then we could have used InsertMenu() or
     * AppendMenu.
     */
    hMenuTrackPopup = GetSubMenu(hMenu, 0);

    /* Convert the mouse point to screen coordinates since that is what
     * TrackPopup expects.
     */
    ClientToScreen(hwnd, (LPPOINT)&point);

    /* Draw and track the "floating" popup */
    TrackPopupMenu(hMenuTrackPopup, 0, point.x, point.y, 0, hwnd, NULL);

    /* Destroy the menu since were are done with it. */
    DestroyMenu(hMenu);
}
/**************************

  マウス入力に対する前処理

***************************/
typedef struct _tagOBJVIEWMOUSEINPUT
{
    int X;
    int Y;
    int Page;
}OBJVIEWMOUSEINPUT, FAR *LPOBJVIEWMOUSEINPUT;

static BOOL ObjviewMousePos2ObjPos(WORD xPos, WORD yPos, LPOBJVIEWMOUSEINPUT lpMouseInput)
{
    if (gblIsROMLoaded
        && yPos > XPOSLABEL_HEIGHT
        && xPos > YPOSLABEL_WIDTH)
    {
        lpMouseInput->X = xPos;
        lpMouseInput->Y = yPos;

        //label size
        lpMouseInput->Y -= XPOSLABEL_HEIGHT;
        lpMouseInput->X -= YPOSLABEL_WIDTH;

        //pixel to 16pixel*16pixel box
        lpMouseInput->Y /= OBJVIEW_CHARACTER_WIDTH;
        lpMouseInput->Y--;
        lpMouseInput->X /= OBJVIEW_CHARACTER_WIDTH;

        //page
        lpMouseInput->Page = giMapViewPageBase + (lpMouseInput->X / CHARCTERS_PER_PAGE);
        lpMouseInput->X &= 0xF;

        if (lpMouseInput->X > 15 || lpMouseInput->Y > 15) return FALSE;

        return TRUE;
    }

    return FALSE;
}

static BOOL ObjviewObjPos2MousePos(LPOBJVIEWMOUSEINPUT lpMouseInput, LPPOINT lpPt)
{
    if (IsBadReadPtr(lpMouseInput, sizeof(OBJVIEWMOUSEINPUT))
        || IsBadReadPtr(lpPt, sizeof(POINT)))
        return FALSE;

    // charcters
    lpPt->x = CHARCTERS_PER_PAGE*(lpMouseInput->Page - giMapViewPageBase);
    if (lpPt->x < 0) return FALSE;
    lpPt->x += lpMouseInput->X;
    lpPt->y = lpMouseInput->Y + 1;

    // pixels
    lpPt->x *= OBJVIEW_CHARACTER_WIDTH;
    lpPt->y *= OBJVIEW_CHARACTER_WIDTH;
    lpPt->x += YPOSLABEL_WIDTH;
    lpPt->y += XPOSLABEL_HEIGHT;

    return TRUE;
}

static BOOL ObjviewInvertObject(HWND hWnd, LPOBJVIEWMOUSEINPUT lpMouseInput)
{
    RECT rcx, rcy;
    int TX, TY, W;
    POINT pt;
    HDC hdc;

    if (!ObjviewObjPos2MousePos(lpMouseInput, &pt))
        return FALSE;

    W = OBJVIEW_CHARACTER_WIDTH * 3;
    TX = (pt.x) ? pt.x - OBJVIEW_CHARACTER_WIDTH : 0;
    TY = (pt.y) ? pt.y - OBJVIEW_CHARACTER_WIDTH : 0;

    SetRect(&rcx, TX, pt.y, TX + W, pt.y + OBJVIEW_CHARACTER_WIDTH);
    SetRect(&rcy, pt.x, TY, pt.x + OBJVIEW_CHARACTER_WIDTH, TY + W);
    hdc = GetDC(hWnd);
    InvertRect(hdc, &rcx);
    InvertRect(hdc, &rcy);
    ReleaseDC(hWnd, hdc);

    return TRUE;
}

long FAR PASCAL MapViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL fObjectIsGrabed = FALSE, fObjViewActive = FALSE;
    static OBJVIEWMOUSEINPUT GrabedObject, TestPlayPos;

    switch (message)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDW_FWDPAGEWINDOW:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
                SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAPVIEW_NEXTPAGE, 0), 0);
                SetFocus(hWnd);
                break;
            }
        }
        break;
        case IDW_REWVPAGEWINDOW:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
                SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_MAPVIEW_PREVPAGE, 0), 0);
                SetFocus(hWnd);
                break;
            }
        }
        break;
        case IDM_MAPVIEW_NEXTPAGE:
        {
            if (!gblIsROMLoaded) break;
            MapEditCommand(IDM_EDITCOMMAND_FWDPAGE);
        }
        break;
        case IDM_MAPVIEW_PREVPAGE:
        {
            if (!gblIsROMLoaded) break;
            MapEditCommand(IDM_EDITCOMMAND_REWPAGE);
        }
        break;
        case IDM_MAPVIEW_SHOWEDITDIALOG:
            if (!gblIsROMLoaded) break;
            if (GetMapEditMode())
                DialogBox(GetModuleHandle(NULL), __T("BADGUYSCOMEDITDLG"), hWnd, BadGuysComEditDlgProc);
            else
                DialogBox(GetModuleHandle(NULL), __T("MAPCOMEDITDLG"), hWnd, MapComEditDlgProc);
            break;
        case IDM_MAPVIEW_SENDOBJECT:
        {
            if (!gblIsROMLoaded) break;
            DialogBox(GetModuleHandle(NULL), __T("SENDOBJECTDLG"), hWnd, SendObjectDlgProc);
        }
        break;
        case IDM_MAPVIEW_TESTPLAY:
        {
            POINT pt;
            if (!gblIsROMLoaded) break;
            pt.x = TestPlayPos.X * 16;
            pt.y = (TestPlayPos.Y + 1) * 16;
            RunEmulatorTestPlayEx(TestPlayPos.Page, FALSE, &pt);

            OpenIcon(GetEmuWndHandle());
            SendMessage(GETCLIENTWNDHANDLE(hWnd), WM_MDIACTIVATE, (WPARAM)GetEmuWndHandle(), 0);
        }
        break;
        case IDM_MAPVIEW_SWITCHEDITMODE:
        {
            if (!gblIsROMLoaded) break;
            ObjwndSwitchEditMode(hWnd);
        }
        break;
        case IDW_YPOSWINDOW:
        {
            if (HIWORD(wParam) == STN_CLICKED)
            {
                g_blIsYPosLineDraw = (g_blIsYPosLineDraw) ? FALSE : TRUE;
                UpdateObjectView(0);
            }
        }
        break;
        case IDW_XPOSWINDOW:
        {
            if (HIWORD(wParam) == STN_CLICKED)
            {
                g_blIsXPosLineDraw = (g_blIsXPosLineDraw) ? FALSE : TRUE;
                UpdateObjectView(0);
            }
        }
        break;
        }
    }
    break;
    case WM_MBUTTONDOWN:
    {
        //ObjwndSwitchEditMode(hWnd);
    }
    break;
    case WM_INITMENU:
    {
        HMENU hMenuInit = (HMENU)wParam;

        if (!gblIsROMLoaded)
        {
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_NEXTPAGE, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_PREVPAGE, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_SENDOBJECT, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_SHOWEDITDIALOG, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_TESTPLAY, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_SWITCHEDITMODE, MF_BYCOMMAND | MF_GRAYED);
        }

        if (TestPlayPos.Y > TESTPLAY_SETTINGS_MAXYPOS)
        {
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_TESTPLAY, MF_BYCOMMAND | MF_GRAYED);
        }

        if (!rm_IsThereObject())
        {
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_NEXTPAGE, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_PREVPAGE, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_SENDOBJECT, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenuInit, IDM_MAPVIEW_SHOWEDITDIALOG, MF_BYCOMMAND | MF_GRAYED);
        }
    }
    break;
    case WM_DRAWITEM:
    {
        UINT idCtl = (UINT)wParam;//control identifier
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;//item-drawing information
        RECT rc;

        GetClientRect(lpdis->hwndItem, &rc);
        switch (idCtl)
        {
        case IDW_YPOSWINDOW:
        {
            BitBlt(lpdis->hDC, 0, 0, YPOSLABEL_WIDTH, MAPVIEW_CLIENTHEIGHT, ghMemdcYPos, 0, 0, SRCCOPY);
        }
        break;
        case IDW_XPOSWINDOW:
        {
            int n, iPages = rc.right / NES_SCREENSIZEX + 1;
            for (n = 0; n < iPages; n++)
                BitBlt(lpdis->hDC, n*NES_SCREENSIZEX, 0, XPOSLABEL_WIDTH, XPOSLABEL_HEIGHT, ghMemdcXPos, 0, 0, SRCCOPY);
        }
        break;
        }
    }
    break;
    case WM_SIZE:
    {
        WORD wWidth = LOWORD(lParam);  // width of client area
        WORD wHeight = HIWORD(lParam); // height of client area

        MoveWindow(ghYPosWnd, 0, XPOSLABEL_HEIGHT, YPOSLABEL_WIDTH, wHeight - XPOSLABEL_HEIGHT, TRUE);
        MoveWindow(ghXPosWnd, YPOSLABEL_WIDTH, 0, wWidth - YPOSLABEL_WIDTH, XPOSLABEL_HEIGHT, TRUE);

        UpdateObjectView(0);
    }
    break;
    case WM_WINDOWPOSCHANGING:
    {
        LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;
        int iPages = 0;
        UINT uBasePageSize = ONE_PAGE_SIZE - GetSystemMetrics(SM_CXSIZEFRAME);
        if ((UINT)lpwp->cx > uBasePageSize)
            iPages = (lpwp->cx - (NES_SCREENSIZEX / 2)) / NES_SCREENSIZEX;
        lpwp->cx = ONE_PAGE_SIZE + NES_SCREENSIZEX*iPages - 1;
        lpwp->cy = MAPVIEW_WINDOWHEIGHT;
        return 0;
    }
    break;
    case WM_PAINT:
    {
        //
        HDC hdc = (HDC)wParam;
        InstallEmulatorPalette(hdc);
        RedrawMapViewWnd();
    }
    break;
    case WM_KEYUP:
    {
        ObjectListShowCursor();
    }
    break;
    case WM_LBUTTONDBLCLK:
        if (!gblIsROMLoaded) break;
        if (GetMapEditMode())
            DialogBox(GetModuleHandle(NULL), __T("BADGUYSCOMEDITDLG"), hWnd, BadGuysComEditDlgProc);
        else
            DialogBox(GetModuleHandle(NULL), __T("MAPCOMEDITDLG"), hWnd, MapComEditDlgProc);
        break;
    case WM_MDIACTIVATE:
    {
        HWND hwndChildDeact = (HWND)wParam;        // child being deactivated

//			HWND hwndChildAct = (HWND) lParam;          // child being activated

        fObjViewActive = (hwndChildDeact == hWnd) ? FALSE : TRUE;
    }
    break;
    case WM_LBUTTONUP:
        if (fObjectIsGrabed && !fObjViewActive)
        {
            ObjviewInvertObject(hWnd, &GrabedObject);
            fObjectIsGrabed = FALSE;
            ReleaseCapture();

            //ClipCursor(NULL);
        }
        fObjViewActive = FALSE;
        break;
    case WM_LBUTTONDOWN:
    {
        DWORD fwKeys = wParam;        // key flags
        int iNewIndex;
        GETINDEXINFO sGetIndex;
        OBJVIEWMOUSEINPUT MouseInput;
        BOOL uPopMenuSelected = 0; //ポップアップメニューが　 0：非表示、-1：表示→選択なし、それ以外：表示→選択あり

        if (!gblIsROMLoaded) break;

        if (!ObjviewMousePos2ObjPos(LOWORD(lParam), HIWORD(lParam), &MouseInput))
            break;

        //
        sGetIndex.dwFlag = GETINDEX_FLAG_XY;
        sGetIndex.x = MouseInput.X;
        sGetIndex.y = MouseInput.Y;
        if (GetMapEditMode())
            iNewIndex = GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING, &sGetIndex, MouseInput.Page, FALSE);
        else
            iNewIndex = GetMapDataIndex(GETADDRESS_CURRENT_EDITTING, &sGetIndex, MouseInput.Page, FALSE);

        if (iNewIndex == GETDATAINDEX_ERROR_NOTFOUND)
            break;

        if (iNewIndex == GETDATAINDEX_ERROR_PL)
        {
            BYTE bData[SMB_OBJECT_DATA_MAX_SIZE];
            int N = 0;
            LPTSTR szMenuText = GetTempStringBuffer();
            HMENU hSelMenu;
            POINT pt;
            UINT uMenuFlag;

            if (MK_CONTROL & fwKeys)
            {
                LONG2POINT(lParam, pt);

                hSelMenu = CreatePopupMenu();
                if (!hSelMenu) break;

                if (!GetMapEditMode())
                {
                    while (N < sGetIndex.nNumSamePos)
                    {
                        GetMapData(GETADDRESS_CURRENT_EDITTING, sGetIndex.iIndex + N, bData, NULL);
                        FormatMapString(bData, szMenuText);
                        uMenuFlag = (sGetIndex.iIndex + N == GetSelectedIndex()) ? MF_BYPOSITION | MF_CHECKED : MF_BYPOSITION;
                        InsertMenu(hSelMenu, N, uMenuFlag, ID_OBJVIEW_OBJSELECT_BASE + sGetIndex.iIndex + N, szMenuText);
                        N++;
                    }
                }
                else
                {
                    while (N < sGetIndex.nNumSamePos)
                    {
                        GetBadGuysData(GETADDRESS_CURRENT_EDITTING, sGetIndex.iIndex + N, bData, NULL);
                        FormatBadGuysString(bData, szMenuText);
                        uMenuFlag = (sGetIndex.iIndex + N == GetSelectedIndex()) ? MF_BYPOSITION | MF_CHECKED : MF_BYPOSITION;
                        InsertMenu(hSelMenu, N, uMenuFlag, ID_OBJVIEW_OBJSELECT_BASE + sGetIndex.iIndex + N, szMenuText);
                        N++;
                    }
                }
                ClientToScreen(hWnd, (LPPOINT)&pt);
                uPopMenuSelected = TrackPopupMenu(hSelMenu, TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
                DestroyMenu(hSelMenu);

                if (uPopMenuSelected)
                {
                    SetSelectedItem(uPopMenuSelected - ID_OBJVIEW_OBJSELECT_BASE, TRUE);
                }
                else
                    uPopMenuSelected = -1;
            }
            else
            {
                SetSelectedItem(sGetIndex.iIndex, TRUE);
            }
        }
        else
        {
            SetSelectedItem(iNewIndex, TRUE);//giSelectedItem=iNewIndex;
        }

        if (uPopMenuSelected != -1)
        {
            SetMapViewPage(MouseInput.Page);
            SetMapViewCursor(MouseInput.X, MouseInput.Y);
        }

        //
        UpdateObjectList(1);
        ObjectListShowCursor();

        //
        UpdateObjectView(0);

        if (!uPopMenuSelected && !fObjViewActive)
        {
            fObjectIsGrabed = TRUE;
            CopyMemory(&GrabedObject, &MouseInput, sizeof(OBJVIEWMOUSEINPUT));
            ObjviewInvertObject(hWnd, &GrabedObject);
            SetCapture(hWnd);
        }
    }
    break;
    case WM_RBUTTONDOWN:
    {
        RECT rc;
        POINT pt;
        OBJVIEWMOUSEINPUT MouseInput;

        if (ObjviewMousePos2ObjPos(LOWORD(lParam), HIWORD(lParam), &MouseInput))
        {
            CopyMemory(&TestPlayPos, &MouseInput, sizeof(OBJVIEWMOUSEINPUT));

            /* Draw the "floating" popup in the app's client area */
            GetClientRect(hWnd, (LPRECT)&rc);

            // Temporary porting macro
            LONG2POINT(lParam, pt);
            if (PtInRect((LPRECT)&rc, pt))
                ShowPopupMenu(hWnd, pt, __T("MAPVIEW_POPUP"));
        }
    }
    break;
    case WM_MOUSEMOVE:
    {
        MSG   msg;
        DWORD fwKeys = wParam;        // key flags
        OBJVIEWMOUSEINPUT MouseInput;

        if (ghToolTip)
        {
            //we need to fill out a message structure and pass it to the tooltip
            //with the TTM_RELAYEVENT message
            msg.hwnd = hWnd;
            msg.message = message;
            msg.wParam = wParam;
            msg.lParam = lParam;
            GetCursorPos(&msg.pt);
            msg.time = GetMessageTime();
            SendMessage(ghToolTip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
        }

        if ((fwKeys&MK_LBUTTON)
            && fObjectIsGrabed
            && ObjviewMousePos2ObjPos(LOWORD(lParam), HIWORD(lParam), &MouseInput))
        {
            BOOL blUpdate = FALSE;
            if (GrabedObject.Page == MouseInput.Page)
            {
                if (GrabedObject.Y == MouseInput.Y)
                {
                    if (GrabedObject.X - 1 == MouseInput.X)
                    {
                        if (MapEditCommand(IDM_EDITCOMMAND_LEFT))
                            blUpdate = TRUE;
                    }
                    else if (GrabedObject.X + 1 == MouseInput.X)
                    {
                        if (MapEditCommand(IDM_EDITCOMMAND_RIGHT))
                            blUpdate = TRUE;
                    }
                }
                else if (GrabedObject.X == MouseInput.X)
                {
                    if (GrabedObject.Y + 1 == MouseInput.Y)
                    {
                        if (MapEditCommand(IDM_EDITCOMMAND_DOWN))
                            blUpdate = TRUE;
                    }
                    else if (GrabedObject.Y - 1 == MouseInput.Y)
                    {
                        if (MapEditCommand(IDM_EDITCOMMAND_UP))
                            blUpdate = TRUE;
                    }
                }
            }
            else
            {
                if (GrabedObject.Page - 1 == MouseInput.Page
                    && MouseInput.X == 15
                    && !GrabedObject.X)
                {
                    if (MapEditCommand(IDM_EDITCOMMAND_LEFT))
                        blUpdate = TRUE;
                }
                else if (GrabedObject.Page + 1 == MouseInput.Page
                         && MouseInput.X == 0
                         && GrabedObject.X == 15)
                {
                    if (MapEditCommand(IDM_EDITCOMMAND_RIGHT))
                        blUpdate = TRUE;
                }
            }
            if (blUpdate)
            {
                CopyMemory(&GrabedObject, &MouseInput, sizeof(OBJVIEWMOUSEINPUT));
                ObjviewInvertObject(hWnd, &GrabedObject);
            }
        }
    }
    break;
    case 0x020A:// #define WM_MOUSEWHEEL                   0x020A
    {
        if (!gblIsROMLoaded) break;

        ky_WM_MOUSEWHEEL(wParam, lParam);
    }
    break;
    case WM_CREATE:
    {
        HDC hdc;
        HBITMAP hBmp;

        //
        ghMapViewWnd = hWnd;

        //DisableIME
        ImmAssociateContext(hWnd, (HIMC)NULL);

        //
        hdc = GetDC(hWnd);
        ghMemdcMapViewWnd = CreateCompatibleDC(hdc);
        ghBitmapMapViewWnd = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), MAPVIEW_CLIENTHEIGHT);
        ghPrevBitmapMapViewWnd = SelectObject(ghMemdcMapViewWnd, ghBitmapMapViewWnd);
        ReleaseDC(hWnd, hdc);

        ClearObjectViewBackBuffer();
        ghMapViewWnd = hWnd;

        ghYPosWnd = CreateWindowEx(/*WS_EX_STATICEDGE*/0,
                                   __T("STATIC"),
                                   NULL,
                                   WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY,
                                   0, XPOSLABEL_HEIGHT, YPOSLABEL_WIDTH, 0,
                                   hWnd, (HMENU)IDW_YPOSWINDOW,
                                   GetModuleHandle(NULL),
                                   NULL);
        PrepareGauge(ghYPosWnd, &ghMemdcYPos, &ghBitmapYPos, YPOSLABEL_WIDTH, MAPVIEW_CLIENTHEIGHT, &ghPrevBitmapYPos);
        DrawYPosGauge();

        ghXPosWnd = CreateWindowEx(/*WS_EX_STATICEDGE*/0,
                                   __T("STATIC"),
                                   NULL,
                                   WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY,
                                   YPOSLABEL_WIDTH, 0, 0, XPOSLABEL_HEIGHT,
                                   hWnd, (HMENU)IDW_XPOSWINDOW,
                                   GetModuleHandle(NULL),
                                   NULL);
        PrepareGauge(ghXPosWnd, &ghMemdcXPos, &ghBitmapXPos, XPOSLABEL_WIDTH, XPOSLABEL_HEIGHT, &ghPrevBitmapXPos);
        DrawXPosGauge();

        ghRPBtnWnd = CreateWindow(
            __T("BUTTON"),
            STRING_NULL,
            WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_CENTER,
            0, 0, YPOSLABEL_WIDTH / 2, XPOSLABEL_HEIGHT,
            hWnd, (HMENU)IDW_REWVPAGEWINDOW,
            GetModuleHandle(NULL),
            NULL);
        hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), __T("LF_IMG"), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
        SendMessage(ghRPBtnWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp);

        ghFPBtnWnd = CreateWindow(__T("BUTTON"),
                                  STRING_NULL,
                                  WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_CENTER,
                                  YPOSLABEL_WIDTH / 2, 0, YPOSLABEL_WIDTH / 2, XPOSLABEL_HEIGHT,
                                  hWnd, (HMENU)IDW_FWDPAGEWINDOW,
                                  GetModuleHandle(NULL),
                                  NULL);
        hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), __T("RG_IMG"), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
        SendMessage(ghFPBtnWnd, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBmp);

        ghToolTip = CreateObjectViewTooltip();

        InitAssistBmp(hWnd);

        //
        ChangeMapViewScreenSize(2);
        UpdateObjectView(0);
    }
    break;
    case WM_DESTROY:
    {
        HDC hdc;
        HBITMAP hBmp;

        hdc = GetDC(hWnd);

        SelectObject(hdc, ghPrevBitmapMapViewWnd);
        DeleteObject(ghBitmapMapViewWnd);
        DeleteDC(ghMemdcMapViewWnd);

        ReleaseDC(hWnd, hdc);

        DeleteGauge(ghYPosWnd, &ghMemdcYPos, &ghBitmapYPos, &ghPrevBitmapYPos);
        DestroyWindow(ghYPosWnd);
        DeleteGauge(ghXPosWnd, &ghMemdcXPos, &ghBitmapXPos, &ghPrevBitmapXPos);
        DestroyWindow(ghXPosWnd);

        hBmp = (HBITMAP)SendMessage(ghRPBtnWnd, BM_GETIMAGE, (WPARAM)IMAGE_BITMAP, 0);
        DeleteObject(hBmp);
        DestroyWindow(ghRPBtnWnd);

        hBmp = (HBITMAP)SendMessage(ghFPBtnWnd, BM_GETIMAGE, (WPARAM)IMAGE_BITMAP, 0);
        DeleteObject(hBmp);
        DestroyWindow(ghFPBtnWnd);

        DestroyWindow(ghToolTip);

        TrashAssistBmp();
    }
    break;
    case WM_SYSCOMMAND:
    {
        if (wParam == SC_CLOSE) return 0;
    }
    break;
    }
    return DefMDIChildProc(hWnd, message, wParam, lParam);
}

BOOL RegisterMapViewWndClass(HINSTANCE hInstance)
{
    WNDCLASS            wc;
#define CBWNDEXTRA      0//12
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
    wc.lpfnWndProc = MapViewWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = CBWNDEXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, __T("MAPVIEWICON"));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = MAPVIEWWNDCLASSNAME;

    if (!RegisterClass(&wc))return FALSE;

    return TRUE;
}

HWND CreateMapViewWnd(HINSTANCE hInstance, HWND hWndMDIClient)
{
    HWND hWnd;

    //WS_VISIBLEを指定して作成しないと、Windowﾒﾆｭｰにｳｲﾝﾄﾞｳが追加されない。
    hWnd = CreateMDIWindow(MAPVIEWWNDCLASSNAME,
                           STRING_WINDOW_OBJVIEW,
                           WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_THICKFRAME | WS_CLIPCHILDREN | WS_VISIBLE,
                           60,//CW_USEDEFAULT,
                           40,//CW_USEDEFAULT,
                           GetSystemMetrics(SM_CXSCREEN) / 2,
                           GetSystemMetrics(SM_CYSCREEN) / 2,
                           hWndMDIClient,
                           hInstance,
                           0);

    return hWnd;
}
