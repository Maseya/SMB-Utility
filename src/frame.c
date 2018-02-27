/**********************************************************************

                                  smb Utility

  File: frame.c
  Description:
  History:

 *********************************************************************/
#include "smbutil.h"
#include "emulator.h"
#include "objdata.h"
#include "objlist.h"
#include "objlib.h"
#include "objmng.h"
#include "objview.h"
#include "objeditcom.h"
#include "roommng.h"
#include "emulator.h"
#include "emuutil.h"
#include "logview.h"
#include "tools.h"
#include "filemng.h"
#include "ini.h"
#include "keys.h"
#include "versinfo.h"
#include "strings.h"

#define FRAMEWNDCLASSNAME __T("MDIFRAME")

#define NES_ONLY_FILTER __T("iNES format ROM image (*.nes)\0\0\0")
#define NES_ALL_FILTER __T("iNES format ROM image (*.nes)\0*.nes\0All (*.*)\0*.*\0\0\0")

HINSTANCE ghInst;
HWND ghWndMDIClient;
HWND ghWndFrame;

HWND ghWndEmu;
HWND ghWndMapEdit;
HWND ghWndMapView;

HWND g_hTbWnd = NULL;
HWND g_hSbWnd = NULL;

HWND g_hEditToolDlg = NULL;

TCHAR gFilePath[MAX_PATH] = {0};

INESHEADER Head;
BYTE bPRGROM[PRG_SIZE];
BYTE bCHRROM[SMB_CHR_SIZE];
int iTrainer = 0;

BOOL gblIsROMLoaded = FALSE;
BOOL gblDataChanged = FALSE;

BOOL g_fShowMsgOnSave = TRUE;

HWND fr_GetStatusBar()
{
    return g_hSbWnd;
}

int Msg(LPCTSTR lpText, UINT uType)
{
    return MessageBox(ghWndMDIClient, lpText, PROGRAMNAME, uType);
}

UINT CALLBACK OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
    case WM_INITDIALOG:
    {
        HWND hWnd;
        HWND hWndOwner;
        LPOPENFILENAME lpOfn = (LPOPENFILENAME)lParam;

        hWnd = GetParent(hDlg);
        hWndOwner = lpOfn->hwndOwner;

        if (IsZoomed(hWndOwner))
        {
            RECT rcDlg;
            RECT rcOwner;
            int x, y, w, h;

            GetWindowRect(hWnd, &rcDlg);
            GetWindowRect(hWndOwner, &rcOwner);

            w = rcDlg.right - rcDlg.left;
            h = rcDlg.bottom - rcDlg.top;
            x = rcOwner.left + ((rcOwner.right - rcOwner.left) - w) / 2;
            y = rcOwner.top + ((rcOwner.bottom - rcOwner.top) - h) / 2;
            if (x >= 0 && y >= 0 && w >= 0 && h >= 0)
                MoveWindow(hWnd, x, y, w, h, TRUE);
        }
        return 1;
    }
    break;
    case WM_NOTIFY:
    {
        LPOFNOTIFY lpon = (LPOFNOTIFY)lParam;

        switch (lpon->hdr.code)
        {
        case CDN_FILEOK:
        {
            lpon->lpOFN->lCustData = FALSE;
            if (IsDlgButtonChecked(hDlg, IDC_TITLE) == BST_CHECKED)
                lpon->lpOFN->lCustData = TRUE;
        }
        break;
        }
    }
    break;
    }

    return 0;
}

BOOL GetFileName(LPTSTR lpPath, HWND hWnd)
{
    OPENFILENAME fname;
    TCHAR filename[64] = {0};
    TCHAR filefilter[] = NES_ALL_FILTER;

    memset(&fname, 0, sizeof(OPENFILENAME));
    fname.lStructSize = sizeof(fname);
    fname.lpstrInitialDir = NULL;
    fname.hwndOwner = hWnd;
    fname.lpstrFilter = filefilter;
    fname.nFilterIndex = 0;
    fname.lpstrFile = lpPath;
    fname.nMaxFile = MAX_PATH;
    fname.lpstrFileTitle = filename;
    fname.nMaxFileTitle = sizeof(filename);
    fname.lpfnHook = NULL;
    fname.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_NOCHANGEDIR;
    fname.lpstrDefExt = __T("nes");

    if (!GetOpenFileName(&fname)) return FALSE;

    return TRUE;
}

LRESULT CALLBACK ApplicationOptionDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        BOOL fCheck;
        LPPROPSHEETPAGE lpPropPage = (LPPROPSHEETPAGE)lParam;

        if (lpPropPage->lParam)
            CenterPropatySheet(hDlg);

        fCheck = g_fShowMsgOnSave ? BST_CHECKED : BST_UNCHECKED;
        CheckDlgButton(hDlg, IDC_MSGONSAVE, fCheck);
    }
    break;
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        switch (pnmh->code)
        {
        case PSN_APPLY:
        {
            g_fShowMsgOnSave = FALSE;
            if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_MSGONSAVE))
                g_fShowMsgOnSave = TRUE;
            return TRUE;
        }
        break;
        }
    }
    break;
    }

    return FALSE;
}

/******************************************

  この関数は、はじめに表示されるページの
  WM_INITDIALGのハンドラ内で呼び出される。

  This function is used to display the first page
  It is called in the handler of WM_INITDIALG.

*******************************************/
void CenterPropatySheet(HWND hDlg)
{
    HWND hWndParent = GetParent(GetParent(hDlg));
    HWND hWndProp = GetParent(hDlg);

    // プロパテイシートを中央に持っていくためのもの。
    // For bringing the property sheet to the center.
    if (IsZoomed(hWndParent))
    {
        RECT rcDlg;
        RECT rcOwner;
        int x, y, w, h;

        GetWindowRect(hWndProp, &rcDlg);
        GetWindowRect(hWndParent, &rcOwner);

        w = rcDlg.right - rcDlg.left;
        h = rcDlg.bottom - rcDlg.top;
        x = rcOwner.left + ((rcOwner.right - rcOwner.left) - w) / 2;
        y = rcOwner.top + ((rcOwner.bottom - rcOwner.top) - h) / 2;
        if (x >= 0 && y >= 0 && w >= 0 && h >= 0)
            SetWindowPos(hWndProp, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOSIZE);
    }
}

void OptionPropertySheet(HWND hwndOwner, int nStartPage)
{
    // TODO
#define OPTPS_NUM_PAGES 4
    LPTSTR lpTitle[OPTPS_NUM_PAGES] = {STRING_OPTIONDIALOG_EMULATOR, STRING_OPTIONDIALOG_EDITOR, STRING_OPTIONDIALOG_APPLICATION, STRING_OPTIONDIALOG_OBJECTVIEW};
    LPTSTR lpDlgResName[OPTPS_NUM_PAGES] = {__T("EMULATOROPTIONDLG"), __T("EDITOROPTIONDLG"), __T("APPLICATIONOPTIONDLG"), __T("OBJECTVIEWOPTIONDLG")};
    DLGPROC pfnDlgProc[OPTPS_NUM_PAGES] = {EmulatorOptionDlgProc, EditorOptionDlgProc, ApplicationOptionDlgProc, ObjectViewOptionDlgProc};

    // Local
    PROPSHEETPAGE psp[OPTPS_NUM_PAGES];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < OPTPS_NUM_PAGES; i++)
    {
        memset(&psp[i], 0, sizeof(PROPSHEETPAGE));
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].hInstance = GetModuleHandle(NULL);
        psp[i].pszTemplate = lpDlgResName[i];
        psp[i].pszIcon = NULL;
        psp[i].pfnDlgProc = pfnDlgProc[i];
        psp[i].pszTitle = lpTitle[i];
        psp[i].lParam = 0;
        if (nStartPage == i) psp[i].lParam = 1;
        psp[i].pfnCallback = NULL;
        psp[i].dwFlags = PSP_USETITLE;
    }

    memset(&psh, 0, sizeof(PROPSHEETHEADER));
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = GetModuleHandle(NULL);
    psh.pszIcon = NULL;
    psh.pszCaption = (LPTSTR)STRING_OPTIONDIALOG_TITLE;
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = nStartPage;
    psh.ppsp = (LPCPROPSHEETPAGE)&psp;
    psh.pfnCallback = (PFNPROPSHEETCALLBACK)NULL;

    PropertySheet(&psh);

    OpenNewRoomProcess();
    return;
}

void RefreshWindowTitle(BOOL fNeedChanged)
{
    LPTSTR cWndTitle = GetTempStringBuffer();

    if (gblIsROMLoaded)
        wsprintf(cWndTitle, __T("%s - %s"), gFilePath, PROGRAMNAME);
    else
        wsprintf(cWndTitle, __T("%s"), PROGRAMNAME);

    if (fNeedChanged)
        lstrcat(cWndTitle, __T("*"));

    SetWindowText(ghWndFrame, cWndTitle);
}

BOOL fr_SetDataChanged(BOOL fChanged)
{
    BOOL fPrev = gblDataChanged;

    gblDataChanged = fChanged;

    if (fPrev != gblDataChanged)
    {
        RefreshWindowTitle(TRUE);
    }

    return fPrev;
}

BOOL fr_GetDataChanged()
{
    return gblDataChanged;
}

void InitGlobalValue()
{
    fr_SetDataChanged(FALSE);

    rm_Initialize();
    InitMapEditGlobalValue();
    InitMapViewGlobalValue();
}

/**************

  ツールバー

  Toolbar

**************/
#define TOOLBAR_BUTTONS 13
#define TOOLBAR_HEIGHT  16
static void SetToolBarButtonState(HWND hTBWnd)
{
#define STBBS_NUM_BUTTONS 9
    UINT uMenuID[STBBS_NUM_BUTTONS] = {IDM_FILE_SAVE,
                                     IDM_SETTING_AREA,
                                     IDM_SETTING_MAP,
                                     IDM_SETTING_BADGUYS,
                                     IDM_EMULATOR_NORMALPLAY,
                                     IDM_EMULATOR_PAGEPLAY,
                                     IDM_EMULATOR_PAGEPLAYHALF,
                                     IDM_EMULATOR_STOP,
                                     IDM_EMULATOR_TESTPLAYSETTING};
    register int i;

    if (!hTBWnd) return;

    if (gblIsROMLoaded)
    {
        for (i = 0; i < STBBS_NUM_BUTTONS; i++)
            SendMessage(hTBWnd, TB_SETSTATE, uMenuID[i], MAKELONG(TBSTATE_ENABLED, 0));

        // 敵とマップの選択。
        // Select enemies and maps.
        if (GetMapEditMode())
            SendMessage(hTBWnd, TB_CHECKBUTTON, (LPARAM)IDM_SETTING_BADGUYS, (WPARAM)1);
        else
            SendMessage(hTBWnd, TB_CHECKBUTTON, (LPARAM)IDM_SETTING_MAP, (WPARAM)1);
    }
    else
    {
        for (i = 0; i < STBBS_NUM_BUTTONS; i++)
            SendMessage(hTBWnd, TB_SETSTATE, uMenuID[i], MAKELONG(TBSTATE_INDETERMINATE, 0));
    }
}

static HWND CreateMainWindowToolBar(HWND hWnd)
{
    TBBUTTON tbBtn[TOOLBAR_BUTTONS];
    HWND hWndTb;
    int iBitmap[TOOLBAR_BUTTONS] = {0,              1,               0,            2,                0,           3,                   4,                  0,           5,                       6,                     7,                         8,                 9};
    int idCommand[TOOLBAR_BUTTONS] = {IDM_FILE_OPEN,  IDM_FILE_SAVE,   0,            IDM_SETTING_AREA, 0,           IDM_SETTING_BADGUYS, IDM_SETTING_MAP,    0,           IDM_EMULATOR_NORMALPLAY, IDM_EMULATOR_PAGEPLAY, IDM_EMULATOR_PAGEPLAYHALF, IDM_EMULATOR_STOP, IDM_EMULATOR_TESTPLAYSETTING};
    BYTE fsState[TOOLBAR_BUTTONS] = {TBSTATE_ENABLED, TBSTATE_ENABLED, 0,            TBSTATE_ENABLED,  0,           TBSTATE_ENABLED,     TBSTATE_ENABLED,    0,           TBSTATE_ENABLED,         TBSTATE_ENABLED,       TBSTATE_ENABLED,           TBSTATE_ENABLED,   TBSTATE_ENABLED};
    BYTE fsStyle[TOOLBAR_BUTTONS] = {TBSTYLE_CHECK,  TBSTYLE_BUTTON,  TBSTYLE_SEP,  TBSTYLE_CHECK,   TBSTYLE_SEP, TBSTYLE_CHECKGROUP,  TBSTYLE_CHECKGROUP, TBSTYLE_SEP, TBSTYLE_BUTTON,            TBSTYLE_BUTTON,        TBSTYLE_BUTTON,            TBSTYLE_BUTTON,    TBSTYLE_CHECK};
    int n;

    for (n = 0; n < TOOLBAR_BUTTONS; n++)
    {
        tbBtn[n].iBitmap = (int)iBitmap[n];
        tbBtn[n].idCommand = (int)idCommand[n];
        tbBtn[n].fsState = (BYTE)fsState[n];
        tbBtn[n].fsStyle = (BYTE)fsStyle[n];
        tbBtn[n].dwData = (DWORD)0;
        tbBtn[n].iString = (int)0;
    }

    hWndTb = CreateToolbarEx(hWnd,
                             WS_VISIBLE | WS_CHILD | TBSTYLE_TOOLTIPS,
                             IDW_TOOLBAR,
                             TOOLBAR_BUTTONS,
                             GetModuleHandle(NULL),
                             TOOLBAR_IMG,
                             tbBtn,
                             TOOLBAR_BUTTONS,
                             0, 0, 16, 16,
                             sizeof(TBBUTTON));

    SetToolBarButtonState(hWndTb);

    return hWndTb;
}

/*************

 ステータスバー

 Status bar

***************/
#define SB_BUFSIZ 126
#define SBPARTS 2

void SetStatusBarText(LPTSTR lpText)
{
    TCHAR szBuf[SB_BUFSIZ];
    SYSTEMTIME st;
    if (!g_hSbWnd) return;

    GetLocalTime(&st);
    wsprintf(szBuf, __T("%d:%.2d:%.2d  __T("), st.wHour, st.wMinute, st.wSecond);
    lstrcat(szBuf, lpText);
    SendMessage(g_hSbWnd, SB_SETTEXT, 0, (LPARAM)szBuf);
    InvalidateRect(g_hSbWnd, NULL, TRUE);
}

void UpdateStatusBarRoomInfoText(HWND hSbWnd)
{
    TCHAR szText[50];

    if (!hSbWnd)
        hSbWnd = g_hSbWnd;

    if (gblIsROMLoaded)
    {
        WORD wAddrBadGuys, wAddrMap;
        wAddrBadGuys = GetBadGuysAddress(GETADDRESS_CURRENT_EDITTING);
        wAddrMap = GetMapAddress(GETADDRESS_CURRENT_EDITTING);
        wsprintf(szText, __T(" %d-%d ( %.2XH [ $%.4X, $%.4X ] )"), g_iWorld + 1, g_iArea + 1, GetRoomID(), wAddrBadGuys, wAddrMap);
    }
    else
    {
        szText[0] = 0;
    }

    SendMessage(hSbWnd, SB_SETTEXT, 1, (LPARAM)szText);
}

static void SizeMainWindowStatusBar(HWND hStatusBarWnd, HWND hParentWnd)
{
#define SB_EXTRASPACE 1
    RECT rc;
    SIZE size;
    int iSize[SBPARTS] = {0};
    TCHAR szExampleText[] = __T(" X-X ( XXH [ $XXXX, $XXXX ] )");
    HDC hDC;

    GetClientRect(hParentWnd, &rc);
    iSize[SBPARTS - 1] = rc.right;
    hDC = GetDC(hParentWnd);
    if (GetTextExtentPoint32(hDC, szExampleText, (int)_tcslen(szExampleText), &size))
        iSize[SBPARTS - 2] = iSize[SBPARTS - 1] - (size.cx) - SB_EXTRASPACE;
    ReleaseDC(hParentWnd, hDC);

    SendMessage(hStatusBarWnd,
                SB_SETPARTS,
                sizeof(iSize) / sizeof(iSize[0]),
                (LPARAM)(LPINT)iSize);
}

static HWND CreateMainWindowStatusBar(HWND hWnd)
{
    HWND hWndSb;

    hWndSb = CreateStatusWindow(WS_CHILD | WS_VISIBLE,
                                NULL,
                                hWnd,
                                IDW_STATUSBAR);

    SizeMainWindowStatusBar(hWndSb, hWnd);

    UpdateStatusBarRoomInfoText(hWndSb);

    return hWndSb;
}

/**************************

  ファイルの保存・ロード

  Save and load files

***************************/
BOOL ConfirmOnExit()
{
    if (fr_GetDataChanged())
    {
        int iRet;

        iRet = Msg(STRING_CONFIRM_EXIT, MB_YESNOCANCEL | MB_ICONINFORMATION);
        if (iRet == IDYES)
        {
            if (!SaveAsFile(gFilePath))
            {
                Msg(STRING_FILEERROR_SAVE, MB_OK | MB_ICONWARNING);
                return FALSE;
            }
        }
        else if (iRet == IDCANCEL)
            return FALSE;
    }
    return TRUE;
}

#define SMBCHR_VALID_SIZE 0x1EC0
BOOL LoadCHRROMFromFile(LPTSTR pFileName, BOOL fLoadAll)
{
    FILE *fp;
    INESHEADER CHRHead;
    int iCHRTrn;
    WORD wCHRValidSize;
    LPTSTR szMsg = GetTempStringBuffer();

    if (!gblIsROMLoaded) return FALSE;

    if ((fp = _tfopen(pFileName, __T("rb"))) == NULL)
    {
        Msg(STRING_FILEERROR_NOTFOUND, MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    fread(&CHRHead, 1, sizeof(INESHEADER), fp);
    if (memcmp(CHRHead.cType, "NES\x1a", 4) || (CHRHead.bNum_CHARs != SMB_NUM_CHARS || CHRHead.bNum_PRGs != SMB_NUM_PRGS))
    {
        Msg(STRING_FILEERROR_FILEFORMAT, MB_OK | MB_ICONWARNING);
        fclose(fp);
        return FALSE;
    }

    undoPrepare(UNDONAME_CHRLOAD);

    iCHRTrn = (CHRHead.bROM_Type & 0x4) ? INES_TRAINERSIZE : 0;

    fseek(fp, iCHRTrn + INES_PRGROMBANKSIZE * SMB_NUM_PRGS, SEEK_CUR);

    wCHRValidSize = (!fLoadAll) ? bPRGROM[0x870A] | ((WORD)bPRGROM[0x8705] << 8) : SMB_CHR_SIZE;

    if (wCHRValidSize > SMB_CHR_SIZE)
        wCHRValidSize = SMB_CHR_SIZE;

    fread(bCHRROM, 1, wCHRValidSize, fp);

    fclose(fp);

    wsprintf(szMsg, STRING_LOGVIEW_LOADCHR, pFileName, wCHRValidSize);
    lv_OutputString(szMsg, LOGVIEW_OUTPUTSTRING_CR);

    return TRUE;
}

BOOL LoadROMFromFile()
{
    LPTSTR szLV = GetTempStringBuffer();
    memset(bPRGROM, 0, 0x10000);
    memset(bCHRROM, 0, 0x2000);

    if (gblIsROMLoaded) StopEmulator();

    undoReset();

    if (!LoadROM(gFilePath))
    {
        gFilePath[0] = 0;

        ObjectListClear();
        ClearObjectViewBackBuffer();
        UpdateObjectView(0);
        InitGlobalValue();
        SetToolBarButtonState(g_hTbWnd);
        UpdateStatusBarRoomInfoText(g_hSbWnd);
        RefreshWindowTitle(FALSE);
        return FALSE;
    }

    // キャラロムの前処理
    // Preprocess CHR data.
    PrepareVROMData(bCHRROM);

    InitGlobalValue();

    OpenNewRoomProcess();
    RefreshWindowTitle(FALSE);
    SetToolBarButtonState(g_hTbWnd);

    wsprintf(szLV, STRING_LOGVIEW_FILEOPEN, gFilePath);
    lv_OutputString(szLV, LOGVIEW_OUTPUTSTRING_CR);

    return TRUE;
}

BOOL SetSaveFileName(HWND hWnd)
{
    OPENFILENAME fname;
    TCHAR filename[65];
    TCHAR filefilter[] = NES_ONLY_FILTER;
    TCHAR curdir[MAX_PATH];
    TCHAR FilePath[MAX_PATH];

    ReadFromRegistry(INI_MDIFRAME_FILEPATH, REG_SZ, &curdir, MAX_PATH * sizeof(TCHAR));

    FilePath[0] = filename[0] = 0;
    memset(&fname, 0, sizeof(OPENFILENAME));
    fname.lStructSize = sizeof(OPENFILENAME);
    fname.lpstrInitialDir = curdir;
    fname.hwndOwner = hWnd;
    fname.lpstrFilter = filefilter;
    fname.nFilterIndex = 0;
    fname.lpstrFile = FilePath;
    fname.nMaxFile = MAX_PATH;
    fname.lpstrFileTitle = filename;
    fname.nMaxFileTitle = sizeof(filename);
    fname.lpfnHook = (LPOFNHOOKPROC)OFNHookProc;
    fname.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_NOREADONLYRETURN | OFN_NOCHANGEDIR;
    fname.lpstrDefExt = __T("nes");

    if (!GetSaveFileName(&fname)) return FALSE;

    memset(curdir, 0, MAX_PATH * sizeof(TCHAR));
    memcpy(curdir, FilePath, fname.nFileOffset * sizeof(TCHAR));
    WriteToRegistry(INI_MDIFRAME_FILEPATH, REG_SZ, curdir, MAX_PATH * sizeof(TCHAR));

    memcpy(gFilePath, FilePath, MAX_PATH);

    return TRUE;
}

BOOL SaveToFile()
{
    LPTSTR szBuf = GetTempStringBuffer();

    if (!SaveAsFile(gFilePath)) return FALSE;

    wsprintf(szBuf, STRING_LOGVIEW_FILESAVE, gFilePath);
    lv_OutputString(szBuf, LOGVIEW_OUTPUTSTRING_CR);
    SetStatusBarText(STRING_STATUSBAR_FILESAVE);

    return TRUE;
}

/********************

  バージョン情報

  Version information

*********************/

void DumpVersion()
{
    LPTSTR szBuf = GetTempStringBuffer();

    wsprintf(szBuf, __T("----------------- %s Ver.%d.%.2d -----------------"), PROGRAMNAME, MAJORVERSION, MINORVERSION);
    lv_OutputString(szBuf, LOGVIEW_OUTPUTSTRING_CR);

    wsprintf(szBuf, __T("(C) 1999-%d M.K.S"), THISYEAR);
    lv_OutputString(szBuf, LOGVIEW_OUTPUTSTRING_CR);

    lv_OutputString(TEXT(__DATE__), LOGVIEW_OUTPUTSTRING_CR);

    lv_OutputString(STRING_VERSION_CONTRIBUTION, LOGVIEW_OUTPUTSTRING_CR);
}

LRESULT CALLBACK VersionDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        LPTSTR szBuf = GetTempStringBuffer();

        wsprintf(szBuf, __T("%s Ver.%d.%.2d"), PROGRAMNAME, MAJORVERSION, MINORVERSION);
        SetDlgItemText(hDlg, IDC_TITLE, szBuf);

        wsprintf(szBuf, __T("(C) 1999-%d M.K.S"), THISYEAR);
        SetDlgItemText(hDlg, IDC_COPYRIGHT, szBuf);

        SetDlgItemText(hDlg, IDC_NOTE, TEXT(__DATE__));

        SetDlgItemText(hDlg, IDC_CONTRIBUTION, STRING_VERSION_CONTRIBUTION);

        DumpVersion();
    }
    break;
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDOK)
        {
            EndDialog(hDlg, TRUE);
            return TRUE;
        }
    }
    break;
    }

    return FALSE;
}

static void ResizeMDIParentWindow(HWND hWnd)
{
    WORD wWidth;
    WORD wHeight;
    int iToolY, iStatusY, iLogViewY;
    RECT rc;

    iToolY = iStatusY = iLogViewY = 0;

    GetClientRect(hWnd, &rc);
    wWidth = (WORD)(rc.right - rc.left);
    wHeight = (WORD)(rc.bottom - rc.top);

    if (g_hTbWnd)
    {
        GetWindowRect(g_hTbWnd, &rc);
        iToolY = rc.bottom - rc.top;
    }

    if (g_hSbWnd)
    {
        GetWindowRect(g_hSbWnd, &rc);
        iStatusY = rc.bottom - rc.top;
    }

    if (lv_ShowLogView())
    {
        GetWindowRect(lv_GetLogViewWnd(), &rc);
        iLogViewY = rc.bottom - rc.top;
    }

    if (g_hTbWnd)
    {
        MoveWindow(g_hTbWnd, 0, 0, wWidth, iToolY, TRUE);
    }
    if (g_hSbWnd)
    {
        MoveWindow(g_hSbWnd, 0, wHeight - iStatusY, wWidth, iStatusY, TRUE);
        SizeMainWindowStatusBar(g_hSbWnd, hWnd);
    }
    if (lv_ShowLogView())
    {
        MoveWindow(lv_GetBorderWnd(),
                   0,
                   wHeight - iStatusY - (iLogViewY + LOGVIEW_BORDERWIDTH),
                   wWidth,
                   LOGVIEW_BORDERWIDTH,
                   TRUE);
        MoveWindow(lv_GetLogViewWnd(),
                   0,
                   wHeight - iStatusY - iLogViewY,
                   wWidth,
                   iLogViewY,
                   TRUE);
        iLogViewY += LOGVIEW_BORDERWIDTH;
    }
    MoveWindow(ghWndMDIClient, 0, iToolY, wWidth, wHeight - iToolY - iStatusY - iLogViewY, TRUE);

    SendMessage(ghWndMDIClient, WM_MDIICONARRANGE, (WPARAM)0, (LPARAM)0);
}

static void LoadWindowPlacement(LPTSTR lpValueName, HWND hWnd)
{
    WINDOWPLACEMENT sWndPl = {0};

    if (ReadFromRegistry(lpValueName, REG_BINARY, &sWndPl, sizeof(WINDOWPLACEMENT)))
    {
        sWndPl.length = sizeof(WINDOWPLACEMENT);
        SetWindowPlacement(hWnd, &sWndPl);
    }
}

static BOOL SaveWindowPlacement(LPTSTR lpValueName, HWND hWnd)
{
    WINDOWPLACEMENT sWndPl = {0};

    sWndPl.length = sizeof(WINDOWPLACEMENT);
    sWndPl.flags = WPF_RESTORETOMAXIMIZED | WPF_SETMINPOSITION;
    GetWindowPlacement(hWnd, &sWndPl);
    WriteToRegistry(lpValueName, REG_BINARY, &sWndPl, sizeof(WINDOWPLACEMENT));

    return TRUE;
}

static void SetMenuStateNeedRom(HWND hWnd)
{
#define SMSNR_NUM_MENUS 24
    UINT uMenuID[SMSNR_NUM_MENUS] = {IDM_FILE_SAVE,
                                   IDM_FILE_SAVEAS,
                                   IDM_FILE_CHRLOAD,
                                   IDM_FILE_IPS,
                                   IDM_SETTING_AREA,
                                   IDM_SETTING_MAPHEAD,
                                   IDM_SETTING_MAP,
                                   IDM_SETTING_BADGUYS,
                                   IDM_SETTING_GAME,
                                   IDM_EDIT_STRINGS,
                                   IDM_EDIT_LOOP,
                                   IDM_EDIT_AREASORT,
                                   IDM_EDIT_LOOPWIZARD,
                                   IDM_TOOL_WORLDDATAUPDATE,
                                   IDM_TOOL_GENERALSETTING,
                                   IDM_TOOL_DEMORECORD,
                                   IDM_EMULATOR_NORMALPLAY,
                                   IDM_EMULATOR_LOADPLAY,
                                   IDM_EMULATOR_PAGEPLAY,
                                   IDM_EMULATOR_PAGEPLAYHALF,
                                   IDM_EMULATOR_TESTPLAYSETTING,
                                   IDM_EMULATOR_LOAD,
                                   IDM_EMULATOR_SAVE,
                                   IDM_EMULATOR_STOP};

    static BOOL blPrevLoaded = TRUE;
    register int i;
    HMENU hMenu;
    UINT uEnable;

    hMenu = GetMenu(hWnd);

    if (gblIsROMLoaded)
    {
        if (blPrevLoaded != TRUE)
        {
            for (i = 0; i < SMSNR_NUM_MENUS; i++)
                EnableMenuItem(hMenu, uMenuID[i], MF_BYCOMMAND | MF_ENABLED);
        }

        uEnable = (IsEmulatorRunning()) ? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_GRAYED);

        EnableMenuItem(hMenu, IDM_EMULATOR_SAVE, uEnable);
        EnableMenuItem(hMenu, IDM_EMULATOR_STOP, uEnable);

        uEnable = (IsEmulatorSavePresent()) ? (MF_BYCOMMAND | MF_ENABLED) : (MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(hMenu, IDM_EMULATOR_LOAD, uEnable);

        uEnable = (rm_IsSubRoom()) ? (MF_BYCOMMAND | MF_GRAYED) : (MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(hMenu, IDM_EMULATOR_PAGEPLAYHALF, uEnable);
    }
    else
    {
        if (blPrevLoaded != FALSE)
        {
            for (i = 0; i < SMSNR_NUM_MENUS; i++)
                EnableMenuItem(hMenu, uMenuID[i], MF_BYCOMMAND | MF_GRAYED);
        }
    }

    blPrevLoaded = gblIsROMLoaded;
}

LRESULT APIENTRY MDIFrameWndProc(HWND hWnd, UINT msg, WPARAM	wParam, LPARAM	lParam)
{
    switch (msg)
    {
    case WM_UPDATEFRAME:
    InvalidateRect(ghWndEmu, NULL, FALSE);
    break;
    case WM_ACTIVATEAPP:
    {
        static BOOL blMsg = FALSE;
        if (LOWORD(wParam) != WA_INACTIVE && !blMsg)
        {
            if (!CheckROMFileTime())
            {
                blMsg = TRUE;
                if (IDYES == Msg(STRING_CONFIRM_RELOAD, MB_YESNO | MB_ICONQUESTION))
                    LoadROMFromFile();
                else
                    SetROMFileTime();
                blMsg = FALSE;
            }
        }
    }
    break;
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (g_hTbWnd && pnmh->code == TTN_GETDISPINFO)//
        {
            LPNMTTDISPINFO TTtext;

            // Tooltip for toolbar.
            TTtext = (LPNMTTDISPINFO)lParam;
            switch (TTtext->hdr.idFrom)
            {
            case IDM_FILE_OPEN:TTtext->lpszText = STRING_TOOLTIP_OPEN; break;
            case IDM_FILE_SAVE:TTtext->lpszText = STRING_TOOLTIP_SAVE; break;
            case IDM_SETTING_AREA:TTtext->lpszText = STRING_TOOLTIP_ROOM; break;
            case IDM_SETTING_BADGUYS:TTtext->lpszText = STRING_TOOLTIP_BADGUYS; break;
            case IDM_SETTING_MAP:TTtext->lpszText = STRING_TOOLTIP_MAP; break;
            case IDM_EMULATOR_NORMALPLAY:TTtext->lpszText = STRING_TOOLTIP_TESTPLAY; break;
            case IDM_EMULATOR_PAGEPLAY:TTtext->lpszText = STRING_TOOLTIP_PAGETESTPLAY; break;
            case IDM_EMULATOR_PAGEPLAYHALF:TTtext->lpszText = STRING_TOOLTIP_HALFPOINTTESTPLAY; break;
            case IDM_EMULATOR_STOP:TTtext->lpszText = STRING_TOOLTIP_STOP; break;
            case IDM_EMULATOR_TESTPLAYSETTING:TTtext->lpszText = STRING_TOOLTIP_TESTPLAYSETTING; break;
            }
        }
    }
    break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDM_FILE_OPEN:
        {
            OPENFILENAME fname;
            TCHAR filename[64] = {0};
            TCHAR filefilter[] = NES_ALL_FILTER;
            TCHAR curdir[MAX_PATH] = {0};
            TCHAR FilePath[MAX_PATH] = {0};

            if (!ConfirmOnExit()) goto OPENCANCEL;

            ReadFromRegistry(INI_MDIFRAME_FILEPATH, REG_SZ, &curdir, MAX_PATH * sizeof(TCHAR));

            memset(&fname, 0, sizeof(OPENFILENAME));
            fname.lStructSize = sizeof(fname);
            fname.lpstrInitialDir = curdir;
            fname.hwndOwner = hWnd;
            fname.lpstrFilter = filefilter;
            fname.nFilterIndex = 0;
            fname.lpstrFile = FilePath;
            fname.nMaxFile = MAX_PATH;
            fname.lpstrFileTitle = filename;
            fname.nMaxFileTitle = sizeof(filename);
            fname.lpfnHook = (LPOFNHOOKPROC)OFNHookProc;
            fname.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_NOCHANGEDIR;
            fname.lpstrDefExt = __T("nes");

            if (!GetOpenFileName(&fname)) goto OPENCANCEL;

            memset(curdir, 0, MAX_PATH * sizeof(TCHAR));
            memcpy(curdir, FilePath, fname.nFileOffset * sizeof(TCHAR));
            WriteToRegistry(INI_MDIFRAME_FILEPATH, REG_SZ, curdir, MAX_PATH * sizeof(TCHAR));

            memcpy(gFilePath, FilePath, MAX_PATH * sizeof(TCHAR));
            LoadROMFromFile();
        OPENCANCEL:
            SendMessage(g_hTbWnd, TB_CHECKBUTTON, (LPARAM)IDM_FILE_OPEN, (WPARAM)MAKELONG(FALSE, 0));
        }
        break;
        case IDM_FILE_SAVE:
        if (gblIsROMLoaded)
        {
            if (g_fShowMsgOnSave)
            {
                if (IDNO == Msg(STRING_CONFIRM_SAVE, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
                    break;
            }
            while (gFilePath[0] == __T('\0') || !SaveToFile())
            {
                Msg(STRING_FILEERROR_SAVE, MB_OK | MB_ICONWARNING);
                if (!SetSaveFileName(hWnd)) break;
            }
            RefreshWindowTitle(FALSE);
        }
        break;
        case IDM_FILE_SAVEAS:
        {
            if (!gblIsROMLoaded) break;
            if (SetSaveFileName(hWnd))
            {
                if (SaveToFile())
                    RefreshWindowTitle(FALSE);
                else
                    Msg(STRING_FILEERROR_SAVEAS, MB_OK | MB_ICONWARNING);
            }
        }
        break;
        case IDM_FILE_CHRLOAD:
        {
            OPENFILENAME fname;
            TCHAR filename[64] = {0};
            TCHAR filefilter[] = NES_ALL_FILTER;
            TCHAR curdir[MAX_PATH] = {0};
            TCHAR filepath[MAX_PATH] = {0};

            if (!gblIsROMLoaded) break;

            memset(&fname, 0, sizeof(OPENFILENAME));
            fname.lStructSize = sizeof(fname);
            fname.lpstrInitialDir = curdir;
            fname.hwndOwner = hWnd;
            fname.lpstrTitle = STRING_FILEOPENDIALOG_CHRLOAD;
            fname.lpstrFilter = filefilter;
            fname.nFilterIndex = 0;
            fname.lpstrFile = filepath;
            fname.nMaxFile = sizeof(filepath);
            fname.lpstrFileTitle = filename;
            fname.nMaxFileTitle = sizeof(filename);
            fname.lpfnHook = (LPOFNHOOKPROC)OFNHookProc;
            fname.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_NOCHANGEDIR | OFN_ENABLETEMPLATE;
            fname.lpstrDefExt = __T("nes");
            fname.hInstance = GetModuleHandle(NULL);
            fname.lpTemplateName = __T("LOADCHROPTION");

            if (!GetOpenFileName(&fname)) break;

            if (LoadCHRROMFromFile(filepath, (BOOL)fname.lCustData))
            {
                PrepareVROMData(bCHRROM);
                UpdateObjectView(0);
            }
        }
        break;
        case IDM_FILE_EXIT:
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;
        case IDM_EDIT_UNDO:
        {
            if (gblIsROMLoaded) undoRestore();
        }
        break;
        case IDM_SETTING_AREA:
        {
            if (gblIsROMLoaded)
            {
                DialogBox(ghInst, __T("ROOMSELECTDLG"), hWnd, AreaSettingDlgProc);
                if (g_hTbWnd)
                {
                    LPARAM dwState;

                    // ボタンを元に戻す
                    // Restore button
                    SendMessage(g_hTbWnd, TB_CHECKBUTTON, (LPARAM)IDM_SETTING_AREA, 0);

                    // 途中ページテストプレイのボタンの有効、無効。
                    // Enable/Disable the button of the middle page test play.
                    dwState = (rm_IsSubRoom()) ? MAKELPARAM(FALSE, 0) : MAKELPARAM(TRUE, 0);
                    SendMessage(g_hTbWnd, TB_ENABLEBUTTON, IDM_EMULATOR_PAGEPLAYHALF, dwState);
                }
            }
        }
        break;
        case IDM_SETTING_MAPHEAD:
        {
            if (gblIsROMLoaded)
                DialogBox(ghInst, __T("MAPCOMHEADEDITDLG"), hWnd, MapComHeadEditDlgProc);
        }
        break;
        case IDM_SETTING_MAP:
        {
            ChangeMapEditMode(CHANGEEDITMODE_MAP, FALSE);
            if (gblIsROMLoaded)
            {
                UpdateObjectViewCursole();
                UpdateObjectList(0);
                UpdateObjectView(0);
            }
            SendMessage(g_hTbWnd, TB_CHECKBUTTON, (LPARAM)IDM_SETTING_MAP, (WPARAM)1);
        }
        break;
        case IDM_SETTING_BADGUYS:
        {
            ChangeMapEditMode(CHANGEEDITMODE_BADGUYS, FALSE);
            if (gblIsROMLoaded)
            {
                UpdateObjectViewCursole();
                UpdateObjectList(0);
                UpdateObjectView(0);
            }
            SendMessage(g_hTbWnd, TB_CHECKBUTTON, (LPARAM)IDM_SETTING_BADGUYS, (WPARAM)1);
        }
        break;
        case IDM_VIEW_TOOLBAR:
        {
            if (g_hTbWnd)
            {
                DestroyWindow(g_hTbWnd);
                g_hTbWnd = NULL;
            }
            else
            {
                g_hTbWnd = CreateMainWindowToolBar(hWnd);
            }
            ResizeMDIParentWindow(hWnd);
        }
        break;
        case IDM_VIEW_STATUSBAR:
        {
            if (g_hSbWnd)
            {
                DestroyWindow(g_hSbWnd);
                g_hSbWnd = NULL;
            }
            else
            {
                g_hSbWnd = CreateMainWindowStatusBar(hWnd);
            }
            ResizeMDIParentWindow(hWnd);
        }
        break;
        case IDM_VIEW_LOGVIEW:
        {
            if (lv_ShowLogView())
            {
                lv_DestroyLogView();
            }
            else
            {
                lv_CreateLogView(hWnd, -1);
            }
            ResizeMDIParentWindow(hWnd);
        }
        break;
        case IDM_EDIT_STRINGS:
        if (gblIsROMLoaded) DialogBox(ghInst, __T("STRINGEDITDLG"), hWnd, StringEditDlgProc);
        break;
        case IDM_EDIT_LOOP:
        if (gblIsROMLoaded) DialogBox(ghInst, __T("LOOPEDITDLG"), hWnd, LoopEditDlgProc);
        break;
        case IDM_EDIT_AREASORT:
        if (gblIsROMLoaded) DialogBox(ghInst, __T("AREASORTDLG"), hWnd, AreaSortDlgProc);
        break;
        case IDM_TOOL_WORLDDATAUPDATE:
        if (gblIsROMLoaded
            && (IDOK == Msg(STRING_CONFIRM_UPDATEWORLD, MB_OKCANCEL | MB_ICONINFORMATION)))
        {
            LPTSTR szBuf = GetTempStringBuffer();
            UpdateWorldData(TRUE);
            wsprintf(szBuf, __T("$9CB4 : %.2xH %.2xH %.2xH %.2xH %.2xH %.2xH %.2xH %.2xH"),
                     bPRGROM[SMB_WORLD_SETTING],
                     bPRGROM[SMB_WORLD_SETTING + 1],
                     bPRGROM[SMB_WORLD_SETTING + 2],
                     bPRGROM[SMB_WORLD_SETTING + 3],
                     bPRGROM[SMB_WORLD_SETTING + 4],
                     bPRGROM[SMB_WORLD_SETTING + 5],
                     bPRGROM[SMB_WORLD_SETTING + 6],
                     bPRGROM[SMB_WORLD_SETTING + 7]);
            lv_OutputString(szBuf, LOGVIEW_OUTPUTSTRING_CR);
        }
        break;
        case IDM_TOOL_GENERALSETTING:
        if (gblIsROMLoaded) DialogBox(ghInst, __T("GENERALSETTINGDLG"), hWnd, GeneralSettingDlgProc);
        break;
        case IDM_SETTING_GAME:
        if (gblIsROMLoaded) GameSettingPropertySheet(hWnd);
        break;
        case IDM_TOOL_DEMORECORD:
        if (gblIsROMLoaded)
        {
            if (IDCANCEL == Msg(STRING_CONFIRM_DEMORECORD, MB_OKCANCEL | MB_ICONINFORMATION)) break;

            OpenIcon(GetEmuWndHandle());
            SendMessage(ghWndMDIClient, WM_MDIACTIVATE, (WPARAM)GetEmuWndHandle(), 0);

            DemoRecord();
        }
        break;
        case IDM_EMULATOR_NORMALPLAY:
        {
            if (!gblIsROMLoaded) break;

            RunEmulatorTestPlay();

            OpenIcon(GetEmuWndHandle());
            SendMessage(ghWndMDIClient, WM_MDIACTIVATE, (WPARAM)GetEmuWndHandle(), 0);
        }
        break;
        case IDM_EMULATOR_PAGEPLAY:
        {
            int iPage;

            if (!gblIsROMLoaded) break;

            if (GetMapEditMode())
                GetBadGuysData(GETADDRESS_CURRENT_EDITTING, GetSelectedIndex(), NULL, &iPage);
            else
                GetMapData(GETADDRESS_CURRENT_EDITTING, GetSelectedIndex(), NULL, &iPage);

            RunEmulatorTestPlayEx(iPage, FALSE, NULL);

            OpenIcon(GetEmuWndHandle());
            SendMessage(ghWndMDIClient, WM_MDIACTIVATE, (WPARAM)GetEmuWndHandle(), 0);
        }
        break;
        case IDM_EMULATOR_PAGEPLAYHALF:
        {
            if (!gblIsROMLoaded || rm_IsSubRoom()) break;
            RunEmulatorTestPlayEx(0, TRUE, NULL);
            OpenIcon(GetEmuWndHandle());
            SendMessage(ghWndMDIClient, WM_MDIACTIVATE, (WPARAM)GetEmuWndHandle(), 0);
        }
        break;
        case IDM_EMULATOR_LOADPLAY:
        {
            if (!gblIsROMLoaded) break;

            RunEmulatorNormal();

            OpenIcon(GetEmuWndHandle());
            SendMessage(ghWndMDIClient, WM_MDIACTIVATE, (WPARAM)GetEmuWndHandle(), 0);
        }
        break;
        case IDM_EMULATOR_STOP:
        {
            if (!gblIsROMLoaded) break;
            StopEmulator();
        }
        break;
        case IDM_EMULATOR_SAVE:
        if (!gblIsROMLoaded) break;
        if (SaveEmulatorState())
            SetStatusBarText(STRING_STATUSBAR_EMUSAVE);
        break;
        case IDM_EMULATOR_LOAD:
        {
            EMULATORSETUP es;

            if (!gblIsROMLoaded) break;

            memset(&es, 0, sizeof(EMULATORSETUP));

            es.pbPRGROM = bPRGROM + 0x8000;
            es.pbCHRROM = bCHRROM;
            if (iTrainer) es.pbTRAINER = bPRGROM + 0x7000;
            if (LoadEmulatorState(&es))
            {
                SetStatusBarText(STRING_STATUSBAR_EMULOAD);
                OpenIcon(GetEmuWndHandle());
                SendMessage(ghWndMDIClient, WM_MDIACTIVATE, (WPARAM)GetEmuWndHandle(), 0);
            }
        }
        break;
        case IDM_EMULATOR_TESTPLAYSETTING:
        if (gblIsROMLoaded)
        {
            DialogBox(ghInst, __T("TESTPLAYSETTINGDLG"), hWnd, TestPlaySettingDlgProc);
            if (g_hTbWnd) SendMessage(g_hTbWnd, TB_CHECKBUTTON, (LPARAM)IDM_EMULATOR_TESTPLAYSETTING, (WPARAM)MAKELONG(FALSE, 0));
        }
        break;
        case IDM_TOOL_OPTION:
        OptionPropertySheet(hWnd, 0);
        break;
        case IDM_TOOL_CUSTOMIZE:
        DialogBox(ghInst, __T("CUSTOMIZEDLG"), hWnd, CustomizeDlgProc);
        break;
        case IDM_WINDOW_CLOSEALL:
        ShowWindow(ghWndMapView, SW_MINIMIZE);
        ShowWindow(ghWndMapEdit, SW_MINIMIZE);
        ShowWindow(GetEmuWndHandle(), SW_MINIMIZE);
        break;
        case IDM_WINDOW_CASCADE:
        SendMessage(ghWndMDIClient, WM_MDICASCADE, (WPARAM)0, (LPARAM)0);
        break;
        case IDM_WINDOW_NEXT:
        {
            HWND hCurWnd;

            hCurWnd = (HWND)SendMessage(ghWndMDIClient, WM_MDIGETACTIVE, 0, 0);
            if (!hCurWnd) break;
            SendMessage(ghWndMDIClient, WM_MDINEXT, (WPARAM)hCurWnd, (LPARAM)0);
        }
        break;
        case IDM_WINDOW_PREV:
        {
            HWND hCurWnd;

            hCurWnd = (HWND)SendMessage(ghWndMDIClient, WM_MDIGETACTIVE, 0, 0);
            if (!hCurWnd) break;
            SendMessage(ghWndMDIClient, WM_MDINEXT, (WPARAM)hCurWnd, (LPARAM)1);
        }
        break;
        case IDM_HELP_VERSION:
        DialogBox(ghInst, __T("VERSIONDLG"), hWnd, VersionDlgProc);
        break;
        default:
        {
            if (gblIsROMLoaded) MapEditCommand(LOWORD(wParam));
        }
        }
    }
    break;
    case WM_SIZE:
    {
        ResizeMDIParentWindow(hWnd);
        return 0;
    }
    case WM_INITMENU:
    {
        CheckMenuRadioItem(GetMenu(hWnd), IDM_SETTING_MAP, IDM_SETTING_BADGUYS, IDM_SETTING_MAP + GetMapEditMode(), MF_BYCOMMAND);

        if (g_hTbWnd)
            MyCheckMenuItem(hWnd, IDM_VIEW_TOOLBAR, TRUE);
        else
            MyCheckMenuItem(hWnd, IDM_VIEW_TOOLBAR, FALSE);

        if (g_hSbWnd)
            MyCheckMenuItem(hWnd, IDM_VIEW_STATUSBAR, TRUE);
        else
            MyCheckMenuItem(hWnd, IDM_VIEW_STATUSBAR, FALSE);

        if (lv_GetLogViewWnd())
            MyCheckMenuItem(hWnd, IDM_VIEW_LOGVIEW, TRUE);
        else
            MyCheckMenuItem(hWnd, IDM_VIEW_LOGVIEW, FALSE);

        EnableMenuItem(GetMenu(hWnd), IDM_EDIT_UNDO, MF_BYCOMMAND | MF_GRAYED);
        {
            TCHAR tcUndoMenu[UNDO_MENUTEXT_BUFFERSIZE];

            wsprintf(tcUndoMenu, UNDO_MENUTEXT, undoGetNameBuffer());
            MySetMenuItemText(hWnd, IDM_EDIT_UNDO, tcUndoMenu);
            if (undoIsEnabled())
                EnableMenuItem(GetMenu(hWnd), IDM_EDIT_UNDO, MF_BYCOMMAND | MF_ENABLED);
        }

        SetMenuStateNeedRom(hWnd);
    }
    break;
    case WM_DROPFILES:
    {
        if (ConfirmOnExit())
        {
            DragQueryFile((HANDLE)wParam, 0, gFilePath, MAX_PATH);
            LoadROMFromFile();
        }

        DragFinish((HANDLE)wParam);
    }
    break;
    case WM_CREATE:
    {
        CLIENTCREATESTRUCT ccs;
        DWORD dwSetting;

        // MDI
        if (!ReadFromRegistry(INI_MDIFRAME_OTHERSETTING, REG_DWORD, &dwSetting, sizeof(DWORD)))
            dwSetting = 0xFFFFFFFF;

        g_hTbWnd = NULL; g_hSbWnd = NULL;
        if (dwSetting&MDIFRAME_OTHERSETTING_TOOLBAR) g_hTbWnd = CreateMainWindowToolBar(hWnd);
        if (dwSetting&MDIFRAME_OTHERSETTING_STATUSBAR) g_hSbWnd = CreateMainWindowStatusBar(hWnd);
        g_fShowMsgOnSave = (dwSetting & MDIFRAME_OTHERSETTING_MSGONSAVE) ? TRUE : FALSE;

        if (lv_ShowLogView())
            lv_CreateLogView(hWnd, -1);

        // ObjView
        if (!ReadFromRegistry(INI_OBJECTVIEW_SETTING, REG_DWORD, &dwSetting, sizeof(DWORD)))
            dwSetting = OBJVIEWSETTING_DEFAULT;
        ObjectViewSetSetting(dwSetting);

        ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 5);
        ccs.idFirstChild = IDM_WINDOW_CHILD;

        ghWndMDIClient = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                        __T("mdiclient"),
                                        __T("mdiclient"),
                                        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | MDIS_ALLCHILDSTYLES,
                                        0, 0,
                                        0, 0,
                                        hWnd,
                                        (HMENU)IDW_MDICLIENT,
                                        GetModuleHandle(NULL),
                                        (LPTSTR)&ccs);

        ShowWindow(ghWndMDIClient, SW_SHOW);
        UpdateWindow(ghWndMDIClient);

        DumpVersion();
        return 0;
    }
    case WM_CLOSE:
    if (fr_GetDataChanged() && !ConfirmOnExit()) return 0;
    break;
    case WM_DESTROY:
    {
        DWORD dwSetting;

        SaveWindowPlacement(INI_MDIFRAME_WINDOW, hWnd);
        SaveWindowPlacement(INI_OBJECTLIST_WINDOW, ghWndMapEdit);
        SaveWindowPlacement(INI_OBJECTVIEW_WINDOW, ghWndMapView);
        SaveWindowPlacement(INI_EMULATOR_WINDOW, ghWndEmu);

        dwSetting = 0;
        if (g_hTbWnd)
        {
            dwSetting |= MDIFRAME_OTHERSETTING_TOOLBAR;
            DestroyWindow(g_hTbWnd);
            g_hTbWnd = NULL;
        }
        if (g_hSbWnd)
        {
            dwSetting |= MDIFRAME_OTHERSETTING_STATUSBAR;
            DestroyWindow(g_hSbWnd);
            g_hSbWnd = NULL;
        }
        if (g_fShowMsgOnSave)
            dwSetting |= MDIFRAME_OTHERSETTING_MSGONSAVE;
        WriteToRegistry(INI_MDIFRAME_OTHERSETTING, REG_DWORD, &dwSetting, sizeof(DWORD));

        dwSetting = ObjectViewGetSetting();
        WriteToRegistry(INI_OBJECTVIEW_SETTING, REG_DWORD, &dwSetting, sizeof(DWORD));

        lv_Trash();

        DestroyWindow(ghWndMDIClient);

        PostQuitMessage(0);
    }
    break;
    }

    return DefFrameProc(hWnd, ghWndMDIClient, msg, wParam, lParam);
}

BOOL RegisterWndClass(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX            wcx;

    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.lpfnWndProc = MDIFrameWndProc;
    wcx.style =
        wcx.cbClsExtra =
        wcx.cbWndExtra = 0;
    wcx.hInstance = hInstance;
    wcx.hIcon = LoadIcon(hInstance, __T("APPICON"));
    wcx.hIconSm = LoadIcon(hInstance, __T("APPICON"));
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wcx.lpszMenuName = __T("MAINWNDMENU");
    wcx.lpszClassName = FRAMEWNDCLASSNAME;

    if (!RegisterClassEx(&wcx))return FALSE;

    if (!RegisterMapEditWndClass(hInstance)) return FALSE;
    if (!RegisterEmuWndClass(hInstance)) return FALSE;
    if (!RegisterMapViewWndClass(hInstance)) return FALSE;

    if (!lv_Initialize()) return FALSE;

    /* Create the frame */
    ghWndFrame = CreateWindowEx(WS_EX_ACCEPTFILES,
                                FRAMEWNDCLASSNAME,
                                PROGRAMNAME,
                                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                NULL,
                                NULL,
                                hInstance,
                                NULL);

    if (!ghWndFrame || !ghWndMDIClient) return FALSE;

    // Create MDI Child window
    // 最後にオブジェクトビューを作成すれば、オブジェクトビューがアクティブで起動する。
    // When you create the object review at the end, the object review is activated.
    ghWndEmu = CreateEmulatorWnd(hInstance, ghWndMDIClient);
    ghWndMapEdit = CreateMapEditWnd(hInstance, ghWndMDIClient);

    // After Emulator is Created.
    ghWndMapView = CreateMapViewWnd(hInstance, ghWndMDIClient);

    if (!ghWndMapEdit || !ghWndEmu || !ghWndMapView) return FALSE;

    // ウインドウメニューのため
    // For the window menu
    Sleep(5);

    ShowWindow(ghWndMapView, SW_HIDE);
    ShowWindow(ghWndMapView, SW_HIDE);
    ShowWindow(ghWndMapEdit, SW_HIDE);
    ShowWindow(ghWndEmu, SW_HIDE);

    if (nCmdShow == SW_SHOWNORMAL)
    {
        LoadWindowPlacement(INI_MDIFRAME_WINDOW, ghWndFrame);
        LoadWindowPlacement(INI_EMULATOR_WINDOW, ghWndEmu);
        LoadWindowPlacement(INI_OBJECTLIST_WINDOW, ghWndMapEdit);
        LoadWindowPlacement(INI_OBJECTVIEW_WINDOW, ghWndMapView);
        nCmdShow = SW_SHOWNA;
    }

    ShowWindow(ghWndFrame, nCmdShow);
    UpdateWindow(ghWndFrame);

    ShowWindow(ghWndMapView, SW_SHOWNA);
    UpdateWindow(ghWndMapView);

    ShowWindow(ghWndMapEdit, SW_SHOWNA);
    UpdateWindow(ghWndMapEdit);

    ShowWindow(ghWndEmu, SW_SHOWNA);
    UpdateWindow(ghWndEmu);

    lv_AdjustLogView();

    if (_tcslen(gFilePath) != 0)
        LoadROMFromFile();

    return TRUE;
}

/*
Load resources for a given region.

0x409: English (US)
0x411: Japanese (J)
*/
void InitializeResources(LANGID locale)
{
    SetThreadLocale(locale);
    SetThreadUILanguage(locale);

    InitResourceStrings();
    InitObjectData();
    InitToolData();
    InitKeys();
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HACCEL hAccel;

    InitializeResources(0x409);

    // Ensure that the common control DLL is loaded
    InitCommonControls();

    // Get file
    gFilePath[0] = 0;
    if (lstrlen(lpCmdLine))
    {
        if (lpCmdLine[0] != __T('"'))
        {
            lstrcpy((LPTSTR)gFilePath, (LPCTSTR)lpCmdLine);
        }
        else
        {
            lstrcpyn(gFilePath, lpCmdLine + 1, lstrlen(lpCmdLine) - 1);
        }
    }

    ghInst = hInstance;

    if (!RegisterWndClass(hInstance, nCmdShow)) return 0;

    LoadTestPlaySettings();

    hAccel = ky_Initialize();

    LoadEditorOption();

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateMDISysAccel(ghWndMDIClient, &msg) &&
            !TranslateAccelerator(ghWndFrame, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
