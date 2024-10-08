﻿/**********************************************************************

                                  smb Utility

  File: emuengine.c
  Description: エミュレータの中心的なルーチン
  Description: Emulator's core routine
  History:

 *********************************************************************/
#include "emulator.h"
#include "ini.h"
#include "nespal.h"
#include "objwndcmn.h"
#include "smbutil.h"

//-------------
//
//  Constants
//
//--------------
#define NES_SCANLINE_VISIBLE_TOP 8
#define NES_SCANLINE_VISIBLE_BOTTOM 231
#define NES_SCANLINE_VBLANK 243
#define NES_SCANLINE_BOTTOM 262
#define NES_VISIBLESIZEY 224  //(NES_SCREENSIZEY-16)
#define EMULATOR_WINDOWFRAMESIZEX (GetSystemMetrics(SM_CXSIZEFRAME) * 2)
#define EMULATOR_WINDOWFRAMESIZEY \
    (GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) * 2)
#define EMULATOR_TIMER_INTERVAL 17
#define INITCOLORINDEX 0x20
#define EMUWNDCLASSNAME __T("EMU_WND")

//-------------
//
//  Global
//
//--------------
BYTE bPPUCtrlReg1;
BYTE bPPUCtrlReg2;
BYTE bPPUStaReg;
CONTEXTM6502* psM6502;
BYTE* pb6502CPUMemory;
BOOL blBGIsVertical;
BYTE bBGScrlH;
BYTE bBGScrlV;
WORD wScanline;
BYTE g_bInVBlank;
BOOL g_blVRAMAddrLow;
WORD wVRAMAddr;
BYTE pbVRAM[0x4000];
BOOL blVRAMFirstRead;
BYTE pbVROMData[0x8000 + 8];
BYTE pbSPRRAM[0x100];
BYTE bSPRRAMAddr;
WORD wSprite0Tile;
BYTE gbBGColor;
BYTE bJoy1Read;
BYTE bJoy2Read;
WORD g_wPrevPC;
BOOL gblPause = FALSE;
BOOL gblExecute = FALSE;
BOOL g_fUpdate = FALSE;
BOOL gblDemoRecord = FALSE;
BOOL g_fUseMMX = FALSE;
BOOL g_fSkipJoyRead = FALSE;
BOOL g_fUseJoyPOV = FALSE;

// A, B, SELECT, START, UP, DOWN, LEFT, RIGHTの順
// NOTE : A, B, SELECT, STARTが最初にくることを前提にして、
//        ジョイスティックのボタン設定を実装している
//        (UP, DOWN, LEFT, RIGHTは未使用)
// in order of A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
// NOTE: On the premise that A, B, SELECT, START comes first,
// Implement the joystick button settings
// (UP, DOWN, LEFT, RIGHT are not used)
BYTE g_EmuKey[EMULATOR_NUM_BUTTONS] = {0};
DWORD g_EmuJoyButton[EMULATOR_NUM_JOYBUTTONS] = {0};
UINT g_uEmuJoyID = JOYSTICKID1;
DWORD g_dwEmuJoyXRight;
DWORD g_dwEmuJoyXLeft;
DWORD g_dwEmuJoyYUp;
DWORD g_dwEmuJoyYDown;
HWND ghEmuWnd = NULL;
UINT g_nTimerID = 0;
DWORD dwThreadID = 0;

// エミュレータのスレッドを複数作成しないためのクリテイカルセクションオブジェクト
// a critical section object not to create multiple emulator threads
CRITICAL_SECTION CriticalSection;

HDC ghMemdcOffScreen = NULL;
HBITMAP ghBmOffScreen = NULL;
HBITMAP ghOldBmOffScreen = NULL;
BYTE* gpbBmBufOffScreen;
LPBITMAPINFO g_lpBmInfo;
HPALETTE g_hPal = NULL;

/*********************

  JOYSTICK Functions

**********************/

// 0: 無効とみなし、セットされない。
// それ以外: 有効な値とみなす
// 要素数8つの配列
// A, B, SELECT, START, UP, DOWN, LEFT, RIGHTの順
//
// 0: It is considered invalid and it is not set.
// other than: regard as a valid value
// array of 8 elements
// in order of A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
BOOL SetEmulatorKeys(WORD aEmuKeys[]) {
    int i;

    for (i = 0; i < EMULATOR_NUM_BUTTONS; i++) {
        if (aEmuKeys[i]) g_EmuKey[i] = (BYTE)aEmuKeys[i];
    }

    return TRUE;
}

BOOL SetEmulatorJoyButtons(DWORD aEmuJoyButtons[]) {
    int i;

    for (i = 0; i < EMULATOR_NUM_JOYBUTTONS; i++) {
        if (aEmuJoyButtons[i]) g_EmuJoyButton[i] = aEmuJoyButtons[i];
    }

    return TRUE;
}

BOOL GetDefaultEmulatorKeys(WORD aEmuKeys[]) {
    // A, B, SELECT, START, UP, DOWN, LEFT, RIGHTの順
    // in order of A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
    aEmuKeys[0] = __T('N');
    aEmuKeys[1] = __T('B');
    aEmuKeys[2] = VK_BACK;
    aEmuKeys[3] = VK_RETURN;
    aEmuKeys[4] = VK_UP;
    aEmuKeys[5] = VK_DOWN;
    aEmuKeys[6] = VK_LEFT;
    aEmuKeys[7] = VK_RIGHT;
    return TRUE;
}

BOOL GetDefaultEmulatorJoyButtons(DWORD aEmuJoyButtons[]) {
    // A, B, SELECT, STARTの順
    // in order of A, B, SELECT, START
    aEmuJoyButtons[0] = JOY_BUTTON2;
    aEmuJoyButtons[1] = JOY_BUTTON1;
    aEmuJoyButtons[2] = JOY_BUTTON3;
    aEmuJoyButtons[3] = JOY_BUTTON4;
    return TRUE;
}

// 呼び出す前にg_uEmuJoyIDをJOYSTICKID1かJOYSTICKID2で初期化しなければならない。
// We must initialize g_uEmuJoyID with JOYSTICKID 1 or JOYSTICKID 2 before calling.
BOOL InitJoystick() {
    //	JOYINFO joyInfo;
    JOYCAPS JoyCaps;
    DWORD dwRange;

    //	MMRESULT mmResult;

    if (!joyGetNumDevs()) goto JOY_ERROR;

    if (joyGetDevCaps(g_uEmuJoyID, &JoyCaps, sizeof(JOYCAPS)) != JOYERR_NOERROR)
        goto JOY_ERROR;

    dwRange = JoyCaps.wXmax - JoyCaps.wXmin;

    // 0 ->
    g_dwEmuJoyXRight = dwRange / 2 + JoyCaps.wXmin + dwRange / 4;
    g_dwEmuJoyXLeft = dwRange / 2 + JoyCaps.wXmin - dwRange / 4;

    // |
    // V
    dwRange = JoyCaps.wYmax - JoyCaps.wYmin;
    g_dwEmuJoyYDown = dwRange / 2 + JoyCaps.wYmin + dwRange / 4;
    g_dwEmuJoyYUp = dwRange / 2 + JoyCaps.wYmin - dwRange / 4;

    //
    if (!(JoyCaps.wCaps & JOYCAPS_HASPOV))  // Joystick has point-of-view information.
        g_fUseJoyPOV = FALSE;

    //	mmResult=joyGetPos(g_uEmuJoyID,&joyInfo);
    //	if(mmResult!=JOYERR_NOERROR)
    //		goto JOY_ERROR;

    //	wXCenter = joyInfo.wXpos;
    //	wYCenter = joyInfo.wYpos;

    return TRUE;

JOY_ERROR:

    //	wXCenter = 0;
    //	wYCenter = 0;
    g_fSkipJoyRead = TRUE;
    return FALSE;
}

BOOL GetEmulatorVKeys(WORD aEmuKeys[]) {
    if (!ReadFromRegistry(INI_EMULATOR_KEYS, REG_BINARY, aEmuKeys,
                          EMULATOR_NUM_BUTTONS * sizeof(WORD))) {
        GetDefaultEmulatorKeys(aEmuKeys);
        return FALSE;
    }

    return TRUE;
}

BOOL GetEmulatorJoyButtons(DWORD aEmuJoyButtons[]) {
    if (!ReadFromRegistry(INI_EMULATOR_JOYBUTTONS, REG_BINARY, aEmuJoyButtons,
                          EMULATOR_NUM_JOYBUTTONS * sizeof(DWORD))) {
        GetDefaultEmulatorJoyButtons(aEmuJoyButtons);
        return FALSE;
    }

    return TRUE;
}

// NOTE:
// レジストリへの書き込みは、keys.cのダイアログコールバック関数内でOKボタンを押した時に行われる。
// NOTE: Writing to the registry is done when the OK button is pressed within the dialog
// callback function of keys.c.

BOOL LoadEmuKeySetting() {
    WORD wKeys[EMULATOR_NUM_BUTTONS] = {0};
    DWORD dwJoyButtons[EMULATOR_NUM_JOYBUTTONS] = {0};
    BOOL blRetk, blRetj;

    blRetk = GetEmulatorVKeys(&wKeys[0]);

    SetEmulatorKeys(&wKeys[0]);

    blRetj = GetEmulatorJoyButtons(&dwJoyButtons[0]);

    SetEmulatorJoyButtons(&dwJoyButtons[0]);

    return (blRetk && blRetj) ? TRUE : FALSE;
}

#define ENABLE_JOYSTICK_RELATED_CONTROLS(H, E)                     \
    {                                                              \
        EnableWindow(GetDlgItem((H), IDC_JOYSTICKID), (E));        \
        EnableWindow(GetDlgItem((H), IDC_USEPOV), (E));            \
        EnableWindow(GetDlgItem((H), IDC_STATIC_JOYSTICKID), (E)); \
    }

LRESULT CALLBACK EmulatorOptionDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                       LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            LPPROPSHEETPAGE lpPropPage = (LPPROPSHEETPAGE)lParam;
            LPTSTR szBuf = GetTempStringBuffer();
            DWORD N;

            if (lpPropPage->lParam) CenterPropatySheet(hDlg);

            // use MMX
            if (g_fUseMMX) CheckDlgButton(hDlg, IDC_SPEEDOPTIMIZE, BST_CHECKED);

            if (!IsMMXAvailable()) {
                g_fUseMMX = FALSE;
                CheckDlgButton(hDlg, IDC_SPEEDOPTIMIZE, BST_UNCHECKED);
                EnableWindow(GetDlgItem(hDlg, IDC_SPEEDOPTIMIZE), FALSE);
            }

            // disable joystick
            if (g_fSkipJoyRead) {
                CheckDlgButton(hDlg, IDC_SKIPJOYREAD, BST_CHECKED);
                ENABLE_JOYSTICK_RELATED_CONTROLS(hDlg, FALSE);
            }

            // joystick ID
            for (N = 0; N < 2; N++) {
                wsprintf(szBuf, __T("%s%d"), STRING_EMULATOROPTION_JOYSTICK, N + 1);
                SendDlgItemMessage(hDlg, IDC_JOYSTICKID, CB_ADDSTRING, 0,
                                   (LPARAM)szBuf);
            }
            SendDlgItemMessage(hDlg, IDC_JOYSTICKID, CB_SETCURSEL,
                               g_uEmuJoyID == JOYSTICKID2 ? 1 : 0, 0);

            // use POV
            if (g_fUseJoyPOV) CheckDlgButton(hDlg, IDC_USEPOV, BST_CHECKED);
        } break;
        case WM_COMMAND: {
            WORD wNotifyCode = HIWORD(wParam);
            WORD wID = LOWORD(wParam);
            switch (wID) {
                case IDC_SKIPJOYREAD:
                    if (wNotifyCode == BN_CLICKED) {
                        BOOL fEnable = (IsDlgButtonChecked(hDlg, IDC_SKIPJOYREAD) ==
                                        BST_UNCHECKED);
                        ENABLE_JOYSTICK_RELATED_CONTROLS(hDlg, fEnable);
                    }
                    break;
            }
        } break;
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch (pnmh->code) {
                case PSN_APPLY: {
                    g_fUseMMX = FALSE;
                    if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_SPEEDOPTIMIZE))
                        g_fUseMMX = TRUE;

                    g_fSkipJoyRead = FALSE;
                    if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_SKIPJOYREAD))
                        g_fSkipJoyRead = TRUE;

                    g_uEmuJoyID = JOYSTICKID1;
                    if (SendDlgItemMessage(hDlg, IDC_JOYSTICKID, CB_GETCURSEL, 0, 0) ==
                        1)
                        g_uEmuJoyID = JOYSTICKID2;

                    g_fUseJoyPOV = FALSE;
                    if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_USEPOV))
                        g_fUseJoyPOV = TRUE;

                    // ジョイスティックにおいて、それ自身や機能のうち、使用できないものは、
                    // この関数内でリセットされる。
                    // In the joystick, what you can not use, of itself or function,
                    // Reset within this function.
                    InitJoystick();

                    return TRUE;
                } break;
            }
        } break;
    }

    return FALSE;
}

#define EMULATOR_SETTING_BITS_USEMMX 0x00000008
#define EMULATOR_SETTING_BITS_DONTUSEJOYSTICK 0x00000010
#define EMULATOR_SETTING_BITS_JOYSTICKID 0x00000020
#define EMULATOR_SETTING_BITS_USEPOV 0x00000040

void SaveEmulatorSetting() {
    // ---- ---- ---- ---- ---- ---- -hij m---
    // m  - use MMX
    // j  - do not use joystick
    // i  - joystick id (0 : JOYSTICKID1, 1 : JOYSTICKID2)
    // h  - use point of view
    DWORD dwSetting = 0;
    if (g_fUseMMX) dwSetting |= EMULATOR_SETTING_BITS_USEMMX;                // 8;
    if (g_fSkipJoyRead) dwSetting |= EMULATOR_SETTING_BITS_DONTUSEJOYSTICK;  // 0x10;
    if (g_uEmuJoyID == JOYSTICKID2) dwSetting |= EMULATOR_SETTING_BITS_JOYSTICKID;
    if (g_fUseJoyPOV) dwSetting |= EMULATOR_SETTING_BITS_USEPOV;
    WriteToRegistry(INI_EMULATOR_SETTING, REG_DWORD, &dwSetting, sizeof(DWORD));
}

BOOL LoadEmulatorSetting() {
    DWORD dwSetting;
    if (ReadFromRegistry(INI_EMULATOR_SETTING, REG_DWORD, &dwSetting, sizeof(DWORD))) {
        g_fUseMMX = (dwSetting & EMULATOR_SETTING_BITS_USEMMX) ? TRUE : FALSE;
        g_fSkipJoyRead =
                (dwSetting & EMULATOR_SETTING_BITS_DONTUSEJOYSTICK) ? TRUE : FALSE;
        g_uEmuJoyID = (dwSetting & EMULATOR_SETTING_BITS_JOYSTICKID) ? JOYSTICKID2
                                                                     : JOYSTICKID1;
        g_fUseJoyPOV = (dwSetting & EMULATOR_SETTING_BITS_USEPOV) ? TRUE : FALSE;
        return TRUE;
    }
    return FALSE;
}

/******************

  グラフィックス

  Graphics

*******************/

HPALETTE CreateNESPalette(HDC hdc) {
    HPALETTE hPal = NULL;
    LOGPALETTE* plogpal;
    UINT n;

    if (hdc == NULL || !(GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE)) return FALSE;

    // Reset system palette
    SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
    SetSystemPaletteUse(hdc, SYSPAL_STATIC);

    // GetDeviceCaps(hdc,NUMRESERVED);

    plogpal = (LOGPALETTE*)Malloc(sizeof(LOGPALETTE) +
                                  EMULATOR_NES_COLORS * sizeof(PALETTEENTRY));
    if (!plogpal) return NULL;

    plogpal->palVersion = 0x300;
    plogpal->palNumEntries = EMULATOR_NES_COLORS;

    for (n = 0; n < EMULATOR_NES_COLORS; n++) {
        plogpal->palPalEntry[n].peRed = rgbNESPal[n].rgbRed;
        plogpal->palPalEntry[n].peGreen = rgbNESPal[n].rgbGreen;
        plogpal->palPalEntry[n].peBlue = rgbNESPal[n].rgbBlue;
        plogpal->palPalEntry[n].peFlags = PC_NOCOLLAPSE;
    }

    hPal = CreatePalette(plogpal);

    Mfree(plogpal);

    return hPal;
}

HPALETTE GetEmulatorPalette() { return g_hPal; }

static void TrashGraphics() {
    if (ghOldBmOffScreen) SelectObject(ghMemdcOffScreen, ghOldBmOffScreen);
    ghOldBmOffScreen = NULL;
    if (ghBmOffScreen) DeleteObject(ghBmOffScreen);
    ghBmOffScreen = NULL;
    if (ghMemdcOffScreen) DeleteDC(ghMemdcOffScreen);
    ghMemdcOffScreen = NULL;
    if (g_hPal) DeleteObject(g_hPal);
    g_hPal = NULL;

    if (g_lpBmInfo) Mfree(g_lpBmInfo);
}

BOOL InitGraphics(HWND hWnd) {
    HDC hdc;
    int iDevCaps;

    hdc = GetDC(hWnd);

    iDevCaps = GetDeviceCaps(hdc, RASTERCAPS);
    if (!(iDevCaps & RC_DIBTODEV)) {
        ReleaseDC(hWnd, hdc);
        return FALSE;
    }

    g_hPal = CreateNESPalette(hdc);

    ReleaseDC(hWnd, hdc);

    g_lpBmInfo = (LPBITMAPINFO)Malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
    if (!g_lpBmInfo) return FALSE;

    memset(g_lpBmInfo, 0, sizeof(BITMAPINFO));
    g_lpBmInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    g_lpBmInfo->bmiHeader.biWidth = NES_SCREENSIZEX;
    g_lpBmInfo->bmiHeader.biHeight = -NES_SCREENSIZEY;
    g_lpBmInfo->bmiHeader.biBitCount = 8;
    g_lpBmInfo->bmiHeader.biPlanes = 1;

    memcpy(g_lpBmInfo->bmiColors, rgbNESPal, EMULATOR_NES_COLORS * sizeof(RGBQUAD));

    ghMemdcOffScreen = CreateCompatibleDC(NULL);
    if (!ghMemdcOffScreen) return FALSE;

    ghBmOffScreen =
            CreateDIBSection(ghMemdcOffScreen, (LPBITMAPINFO)g_lpBmInfo, DIB_RGB_COLORS,
                             (void**)&gpbBmBufOffScreen, NULL, 0);

    if (!ghBmOffScreen) return FALSE;

    ghOldBmOffScreen = (HBITMAP)SelectObject(ghMemdcOffScreen, ghBmOffScreen);
    if (!ghOldBmOffScreen) return FALSE;

    ClearEmuBackBuffer();

    return TRUE;
}

void ClearEmuBackBuffer() {
    if (ghMemdcOffScreen) {
        RECT rc;
        DWORD dwColor = GetSysColor(COLOR_WINDOW);

        SetRect(&rc, 0, 0, NES_SCREENSIZEX, NES_SCREENSIZEY);
        FillRect(ghMemdcOffScreen, &rc, (HBRUSH)(COLOR_WINDOW + 1));
    }
}

void SetEmuBackBufferPallete(LPBYTE lpPalData) {
    int n;

    if (!ghMemdcOffScreen || !g_lpBmInfo) return;

    if (lpPalData) {
        for (n = 0; n < 0x20; n++) {
            if (lpPalData[n] > 63) continue;
            memcpy(&g_lpBmInfo->bmiColors[n], &rgbNESPal[lpPalData[n]],
                   sizeof(RGBQUAD));
        }
    } else {
        for (n = 0; n < 0x20; n++) {
            SetDIBColorTable(ghMemdcOffScreen, n, 1, &g_lpBmInfo->bmiColors[n]);
        }
    }
}

static void RefreshSpriteLine(DWORD dwLine) {
    int x, y2;
    DWORD y;
    int iPosX;
    int i;
    int iFlipX, iFlipY, iCol;
    int iPriority;
    int iIndex;

    BYTE* pbChrBase;
    LPBYTE lpbBase = gpbBmBufOffScreen + dwLine * NES_SCREENSIZEX;
    int iPtr;
    LPBYTE pbVBuf;

#define iSize 8

    for (i = 0xFC; i >= 0; i -= 4) {
        y = pbSPRRAM[i] + 1;

        if ((y > dwLine) || (y + iSize <= dwLine)) continue;

        iIndex = pbSPRRAM[i + 1];
        iCol = (pbSPRRAM[i + 2] & 0x03);
        iFlipX = (pbSPRRAM[i + 2] & 0x40);
        iFlipY = (pbSPRRAM[i + 2] & 0x80);
        iPriority = (pbSPRRAM[i + 2] & 0x20);
        x = pbSPRRAM[i + 3];

        y2 = dwLine - y;
        if (iFlipY) y2 = (iSize - 1) - y2;

        pbChrBase = pbVROMData + iIndex * 64 + y2 * 8;
        if ((bPPUCtrlReg1 & 0x08) /*&& iSize==8*/) pbChrBase += 0x4000;

        iCol = (iCol << 2) + 0x10;

        if (iPriority) {
            pbVBuf = lpbBase + x;
            if (iFlipX) {
                for (iPosX = 7; iPosX >= 0; pbVBuf++) {
                    iPtr = pbChrBase[iPosX--];
                    if (iPtr && (!(*pbVBuf & 0x03) || (*pbVBuf & 0x10)))
                        *pbVBuf = iCol + iPtr;
                }
            } else {
                for (iPosX = 0; iPosX < 8; pbVBuf++) {
                    iPtr = pbChrBase[iPosX++];
                    if (iPtr &&
                        (!(*pbVBuf & 0x03) ||
                         (*pbVBuf & 0x10)))  // if(BACKCOLORINDEX==*pbVBuf && iPtr)
                        *pbVBuf = iCol + iPtr;
                }
            }
        } else {
            pbVBuf = lpbBase + x;
            if (iFlipX) {
                pbChrBase += 7;
                for (iPosX = 7; iPosX-- >= 0; pbChrBase--, pbVBuf++) {
                    if (!*pbChrBase) continue;
                    *pbVBuf = iCol + (*pbChrBase);
                }
            } else {
                for (iPosX = 0; iPosX++ < 8; pbChrBase++, pbVBuf++) {
                    if (!*pbChrBase) continue;
                    *pbVBuf = iCol + (*pbChrBase);
                }
            }
        }
    }

    return;
}

static void RefreshBackGroundLine(register WORD wLine) {
    int x;
    int iTileIndex;
    int iPixelX;
    int iStartY;

    int iNameTable;
    LPBYTE lp_pbVROMData_iBase_iStartYModEight;
    int iFirst = 1;
    LPBYTE pbChrBase;
    LPBYTE pbVideoBuf = gpbBmBufOffScreen + wLine * NES_SCREENSIZEX;
    int iColorByte;
    int iCol;
    LARGE_INTEGER qwCol64;

    iPixelX = ((bBGScrlH & 7) ^ 7) - 7;

    iStartY = wLine;  // iStartY = bBGScrlV + wLine;

    //	iStartYModEight=((iStartY&7)<<3);
    iNameTable = (((bPPUCtrlReg1 & 0x03) * 0x400) | 0x2000);

    // タイルの計算
    // Calculate tiles
    x = bBGScrlH >> 3;
    iTileIndex = iNameTable + ((iStartY / 8) * 32);

    lp_pbVROMData_iBase_iStartYModEight = pbVROMData + ((iStartY & 7) << 3);

    if (bPPUCtrlReg1 & 0x10) lp_pbVROMData_iBase_iStartYModEight += 0x4000;

    while (iPixelX < 256) {
        int iIndex1 = iTileIndex + x;

        if ((!(x & 1)) || iFirst) {
            // アトリビュートテーブルのデータを取得
            // Retrieve the data of the attribute table
            if ((!(x & 3)) || iFirst) {
                iColorByte = pbVRAM[((iIndex1 & 0x3C00) | (iIndex1 & 0x1F) >> 2) |
                                    ((iIndex1 & 0x380) >> 4) | 0x3C0];
            }

            // アトリビュートデータのどの2ビットを使用するかを計算
            // Calculate which 2 bits of the attribute data to use
            iCol = (((iColorByte >> ((iIndex1 & 0x02) | ((iIndex1 & 0x40) >> 4))) &
                     0x03)
                    << 2);
            iFirst = 0;

            if (g_fUseMMX) {
                memset(&qwCol64, iCol, 8);
            }
        }

        pbChrBase =
                lp_pbVROMData_iBase_iStartYModEight + (pbVRAM[iIndex1] << 6) /**64*/;

        if (!g_fUseMMX) {
            int n = (iPixelX < 0) ? 8 + iPixelX : 8;
            if (iPixelX < 0) pbChrBase -= iPixelX;
            iPixelX += 8;

            *pbVideoBuf |= iCol;

            for (int i = n; --i >= 0;) pbVideoBuf[i] = pbChrBase[i] | iCol;

            pbChrBase += n;
            pbVideoBuf += n;

            if (++x & 0x20) {
                x = 0;
                iTileIndex ^= 0x400;
            }
        } else {
            int n = (iPixelX < 0) ? 8 + iPixelX : 8;
            if (iPixelX < 0) pbChrBase -= iPixelX;

            for (int i = 0; i < 8; i++) {
                pbVideoBuf[i] = pbChrBase[i] | iCol;
            }

            pbVideoBuf += n;
            iPixelX += 8;
            pbChrBase += 8;

            if (++x & 0x20) {
                x = 0;
                iTileIndex ^= 0x400;
            }
        }
    }
}

static void RefreshLine(WORD wLine) {
    if (bPPUCtrlReg2 & 0x08)
        RefreshBackGroundLine(wLine);
    else {
        memset(gpbBmBufOffScreen + wLine * NES_SCREENSIZEX, 0, NES_SCREENSIZEX);
    }

    if (bPPUCtrlReg2 & 0x10) RefreshSpriteLine(wLine);
}

/********************************

  キャラロムがエディットされたら
  この関数を呼び出す必要がある

  When CHR ROM is edited
  I need to call this function

*********************************/
BOOL PrepareVROMData(BYTE* pbSource) {
    BYTE* pbVROMDataPointer;
    BYTE* pbVROMPointer;
    int iByte;
    int iBit;
    int iIndex;

    if (IsBadReadPtr(pbSource, SMB_CHR_SIZE)) return FALSE;

    pbVROMDataPointer = pbVROMData;
    pbVROMPointer = pbSource;

    for (iIndex = 0; iIndex < 0x200; iIndex++) {
        for (iByte = 0; iByte < 8; iByte++) {
            for (iBit = 7; iBit >= 0; iBit--) {
                *pbVROMDataPointer = ((pbVROMPointer[8] >> iBit) & 0x01);
                *pbVROMDataPointer <<= 1;
                *pbVROMDataPointer |= ((pbVROMPointer[0] >> iBit) & 0x01);

                pbVROMDataPointer++;
            }

            pbVROMPointer++;
        }
        pbVROMPointer += 8;
    }

    return TRUE;
}

/****************************

  Functions for NES Register

*****************************/
UINT8 RdNESReg(UINT32 Addr, struct MemoryReadByte* psMemRead) {
    register BYTE bRet;
    switch (Addr) {
        case 0x2002: {
            bRet = bPPUStaReg;
            bPPUStaReg &= 0x7F;  // Unset vblank flag
            if (g_bInVBlank) bPPUCtrlReg1 &= 0xFC;
            blBGIsVertical = g_blVRAMAddrLow = FALSE;
            return bRet;
        }
        case 0x4016: {
            // JOYINFO joyInfo;
            JOYINFOEX JoyInfoEx;

            // ジョイスティックの状態をトラップ
            // Trap Joystick state
            if (!g_fSkipJoyRead) {
                JoyInfoEx.dwSize = sizeof(JOYINFOEX);
                JoyInfoEx.dwFlags =
                        g_fUseJoyPOV ? JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS |
                                               JOY_RETURNPOV
                                     : JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
                joyGetPosEx(g_uEmuJoyID, &JoyInfoEx);
            }
            bRet = 0x40;
            switch (bJoy1Read++) {
                case 0:  // A
                    if (GetAsyncKeyState(g_EmuKey[0]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[0])))  // JOY_BUTTON2)))
                        bRet |= 0x01;
                    break;
                case 1:  // B
                    if (GetAsyncKeyState(g_EmuKey[1]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[1])))  // JOY_BUTTON1)))
                        bRet |= 0x01;
                    break;
                case 2:  // SELECT
                    if (GetAsyncKeyState(g_EmuKey[2]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[2])))  // JOY_BUTTON3)))
                        bRet |= 0x01;
                    break;
                case 3:  // START
                    if (GetAsyncKeyState(g_EmuKey[3]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[3])))  // JOY_BUTTON4)))
                        bRet |= 0x01;
                    break;
                case 4:  // UP
                    if (GetAsyncKeyState(g_EmuKey[4]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwYpos < g_dwEmuJoyYUp) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVFORWARD))))
                        bRet |= 0x01;
                    break;
                case 5:  // DOWN
                    if (GetAsyncKeyState(g_EmuKey[5]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwYpos > g_dwEmuJoyYDown) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVBACKWARD))))
                        bRet |= 0x01;
                    break;
                case 6:  // LEFT
                    if (GetAsyncKeyState(g_EmuKey[6]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwXpos < g_dwEmuJoyXLeft) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVLEFT))))
                        bRet |= 0x01;
                    break;
                case 7:  // RIGHT
                    if (GetAsyncKeyState(g_EmuKey[7]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwXpos > g_dwEmuJoyXRight) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVRIGHT))))
                        bRet |= 0x01;
                    break;
            } /* switch */

            if (gblDemoRecord) DemoRecorderHandler((BYTE)(bJoy1Read - 1), bRet);

            return bRet;
        }
        case 0x4017: {
            JOYINFOEX JoyInfoEx;

            // ジョイスティックの状態をトラップ
            // Trap Joystick state
            if (!g_fSkipJoyRead) {
                JoyInfoEx.dwSize = sizeof(JOYINFOEX);
                JoyInfoEx.dwFlags =
                        g_fUseJoyPOV ? JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS |
                                               JOY_RETURNPOV
                                     : JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
                joyGetPosEx(g_uEmuJoyID, &JoyInfoEx);
            }
            bRet = 0x40;
            switch (bJoy2Read++) {
                case 0:  // A
                    if (GetAsyncKeyState(g_EmuKey[0]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[0])))  // JOY_BUTTON2)))
                        bRet |= 0x01;
                    break;
                case 1:  // B
                    if (GetAsyncKeyState(g_EmuKey[1]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[1])))  // JOY_BUTTON1)))
                        bRet |= 0x01;
                    break;
                case 2:  // SELECT
                    if (GetAsyncKeyState(g_EmuKey[2]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[2])))  // JOY_BUTTON3)))
                        bRet |= 0x01;
                    break;
                case 3:  // START
                    if (GetAsyncKeyState(g_EmuKey[3]) ||
                        (!g_fSkipJoyRead &&
                         (JoyInfoEx.dwButtons & g_EmuJoyButton[3])))  // JOY_BUTTON4)))
                        bRet |= 0x01;
                    break;
                case 4:  // UP
                    if (GetAsyncKeyState(g_EmuKey[4]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwYpos < g_dwEmuJoyYUp) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVFORWARD))))
                        bRet |= 0x01;
                    break;
                case 5:  // DOWN
                    if (GetAsyncKeyState(g_EmuKey[5]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwYpos > g_dwEmuJoyYDown) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVBACKWARD))))
                        bRet |= 0x01;
                    break;
                case 6:  // LEFT
                    if (GetAsyncKeyState(g_EmuKey[6]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwXpos < g_dwEmuJoyXLeft) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVLEFT))))
                        bRet |= 0x01;
                    break;
                case 7:  // RIGHT
                    if (GetAsyncKeyState(g_EmuKey[7]) ||
                        (!g_fSkipJoyRead &&
                         ((JoyInfoEx.dwXpos > g_dwEmuJoyXRight) ||
                          (g_fUseJoyPOV && JoyInfoEx.dwPOV == JOY_POVRIGHT))))
                        bRet |= 0x01;
                    break;
            } /* switch */

            // Note: DemoRecorderHandler()関数を2Conで呼び出してはいけない。
            // Note: Do not call the DemoRecorderHandler() function with 2Con.

            return bRet;
        }
        case 0x2007:  // Read
        {
            if (blVRAMFirstRead) {
                blVRAMFirstRead = FALSE;
                return 0x00;
            }
            bRet = pbVRAM[wVRAMAddr];
            wVRAMAddr += (bPPUCtrlReg1 & 0x4) ? 32 : 1;
            return bRet;
        }
        case 0x2004: {
            bRet = pbSPRRAM[bSPRRAMAddr++];
            return bRet;
        }
    }
    return 0;
}

void WrNESReg(UINT32 Addr, UINT8 Value, struct MemoryWriteByte* psMemWrite) {
    switch (Addr) {
        case 0x2000:
            bPPUCtrlReg1 = Value;
            break;
        case 0x2001:
            bPPUCtrlReg2 = Value;
            break;
        case 0x2005:
            if (!blBGIsVertical) {
                bBGScrlH = Value;
                blBGIsVertical = TRUE;
                break;
            } else {
                // bBGScrlV=Value;
                blBGIsVertical = FALSE;
                break;
            }
            break;
        case 0x2006:  // Address Write
            blVRAMFirstRead = TRUE;
            if (g_blVRAMAddrLow) {
                wVRAMAddr = (wVRAMAddr & 0xFF00) | Value;
                g_blVRAMAddrLow = FALSE;
                break;
            } else {
                wVRAMAddr = (wVRAMAddr & 0xFF) | (Value << 8);
                g_blVRAMAddrLow = TRUE;
                break;
            }
        case 0x2007: {
            if ((wVRAMAddr & 0x3000) == 0x2000)
                pbVRAM[wVRAMAddr] = pbVRAM[wVRAMAddr ^ 0x800] = Value;
            else if (0x3f00 <= wVRAMAddr)  // Write to Palette
            {
                if (wVRAMAddr & 0x0003) {
                    pbVRAM[wVRAMAddr] = Value;
                    memcpy(&g_lpBmInfo->bmiColors[wVRAMAddr & 0x1F], &rgbNESPal[Value],
                           sizeof(RGBQUAD));
                }

                // Palette mirror
                else if (!(wVRAMAddr & 0x000F) && gbBGColor != Value) {
                    register int i = 0;
                    for (; i < 8; i++) {
                        pbVRAM[0x3f00 | (i << 2)] = Value;
                        memcpy(&g_lpBmInfo->bmiColors[(i << 2)], &rgbNESPal[Value],
                               sizeof(RGBQUAD));
                    }

                    // バックグラウンドの色データ
                    // background color data
                    gbBGColor = Value;
                }
            }

            wVRAMAddr += (bPPUCtrlReg1 & 0x4) ? 32 : 1;
        } break;
        case 0x4014:
            memcpy(pbSPRRAM, psM6502->m6502Base + (0x100 * Value), 0x100);
            break;
        case 0x4016:
            bJoy1Read = bJoy2Read = 0;
            break;
        case 0x2002:
            break;
        case 0x2003:
            bSPRRAMAddr = Value;
            break;
        case 0x2004:
            pbSPRRAM[bSPRRAMAddr++] = Value;
            break;
    }
}

/**********************

  Functions for M6502

***********************/
static int GetFirstNonTransparentLine(BYTE bTile) {
    BYTE* pbTileData;
    BYTE pbZero[8] = {0};
    static int i;

    if (bTile == wSprite0Tile) return i;

    wSprite0Tile = bTile;

    pbTileData = (bPPUCtrlReg1 & 0x08) ? pbVROMData + 0x4000 + 64 * bTile
                                       : pbVROMData + 64 * bTile;
    for (i = 0; i++ < 7; pbTileData += 8) {
        if (memcmp(pbTileData, pbZero, 8)) break;
    }
    return i;
}

static WORD ExecuteCPU() {
    // Execute CPU
    m6502zpexec(113);

    // Increment scanline
    wScanline++;

    // Set hity flag, if necessary
    if (wScanline == pbSPRRAM[0] + GetFirstNonTransparentLine(pbSPRRAM[1]))
        bPPUStaReg |= 0x40;

    if (wScanline > 7 && wScanline < 232) {
        // Update scanline
        RefreshLine(wScanline);
        return wScanline;
    } else if (wScanline == NES_SCANLINE_VBLANK) {
        PostMessage(GETFRAMEWNDHANDLE(ghEmuWnd), WM_UPDATEFRAME, 0,
                    0);  // g_fUpdate = TRUE;

        // VBlank begins
        g_bInVBlank = TRUE;

        // Set vblank flag
        bPPUStaReg |= 0x80;

        // if nessesary, execute NMI.
        if (bPPUCtrlReg1 & 0x80) m6502zpnmi();
        return wScanline;
    } else if (wScanline == NES_SCANLINE_BOTTOM) {
        // VBlank ends
        g_bInVBlank = FALSE;

        // Clear VBlank flag and hity flag
        bPPUStaReg &= 0x30;

        // Reset scanline
        wScanline = 0;
        return wScanline;
    }

    return wScanline;
}

void CALLBACK Run6502(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
    if (gblPause) return;

    while (ExecuteCPU())
        ;

    return;
}

/***********************

 used for Map Viewering

************************/

_inline void ExecuteCPUNoRefreshScreen(int* lpiScanlines) {
    m6502zpexec(113);

    int pc = m6502zppc();

    if (g_wPrevPC == pc) {
        // detect a follow code.
        // addr1 : jmp addr1
        LPBYTE lpbOpCode = &psM6502->m6502Base[pc];
        if (lpbOpCode[0] == 0x4C && lpbOpCode[1] == (pc & 0xFF) &&
            lpbOpCode[2] == (pc >> 8)) {
            if (wScanline < 242) {
                (*lpiScanlines) -= (241 - wScanline);
                wScanline = 241;
            }
        }
    }
    g_wPrevPC = pc;

    wScanline++;

    if (wScanline == pbSPRRAM[0]) {
        bPPUStaReg |= 0x40;
    }

    if (wScanline == 242) {
        bPPUStaReg |= 0x80;  // Set vblank flag
        if (bPPUCtrlReg1 & 0x80) {
            m6502zpnmi();
        }
    } else if (wScanline == 263) {
        bPPUStaReg &= 0x30;  // Unset vblank flag and hity flag
        wScanline = 0;
    }
}

void Run6502Ex(int iFrames) {
    register int n = 0;

    if (gblExecute) return;

    for (;;) {
        if (++n <= iFrames) {
            ExecuteCPUNoRefreshScreen(&iFrames);
            continue;
        }
        break;
    }
}

void DrawAllScanline() {
    WORD w;
    for (w = 8; w < 239; w++) RefreshLine(w);
}

/**********************

  memory map

***********************/
struct MemoryWriteByte NESWrite[] = {{0x2000, 0x6FFF, WrNESReg},
                                     {(UINT32)-1, (UINT32)-1, NULL}};

struct MemoryReadByte NESRead[] = {{0x2000, 0x6FFF, RdNESReg},
                                   {(UINT32)-1, (UINT32)-1, NULL}};

struct MemoryWriteByte NESWriteEx[] = {{0x2000, 0x6FFF, WrNESReg},
                                       {(UINT32)-1, (UINT32)-1, NULL}};

struct MemoryReadByte NESReadEx[] = {{0x2000, 0x6FFF, RdNESReg},
                                     {(UINT32)-1, (UINT32)-1, NULL}};
/***************************

  Nester Command Functions

****************************/

void DestoryNester() {
    if (psM6502) Mfree(psM6502);
    psM6502 = NULL;
    if (pb6502CPUMemory) Mfree(pb6502CPUMemory);
    pb6502CPUMemory = NULL;

    TrashGraphics();

    if (IsEmulatorSavePresent()) FreeEmulatorSaveBuffer();

    DeleteCriticalSection(&CriticalSection);
}

BOOL CreateNester() {
    psM6502 = (CONTEXTM6502*)Malloc(sizeof(CONTEXTM6502));
    if (!psM6502) return FALSE;
    memset(psM6502, 0, sizeof(CONTEXTM6502));

    pb6502CPUMemory = (BYTE*)Malloc(0x10000);
    if (!pb6502CPUMemory) return FALSE;

    psM6502->m6502Base = pb6502CPUMemory;
    psM6502->m6502MemoryRead = NESRead;
    psM6502->m6502MemoryWrite = NESWrite;

    psM6502->m6502MemoryRead->pUserArea = psM6502;
    psM6502->m6502MemoryWrite->pUserArea = psM6502;

    m6502zpSetContext(psM6502);

    InitGraphics(ghEmuWnd);  // After ROMs are loaded

    InitJoystick();

    InitializeCriticalSection(&CriticalSection);

    return TRUE;
}

void ResetEmulator() {
    m6502zpreset();

    ClearEmuBackBuffer();

    InitJoystick();

    // PrepareVROMData(pbVRAM);

    // TODO ここにリセットの処理を追加してください。
    // TODO Add the reset process here.
    gblPause = FALSE;
    gblDemoRecord = FALSE;

    g_bInVBlank = FALSE;

    blBGIsVertical = FALSE;
    g_blVRAMAddrLow = FALSE;
    blVRAMFirstRead = FALSE;

    wSprite0Tile = 0x100;

    bJoy1Read = bJoy2Read = 0x00;

    gbBGColor = 0x00;

    wScanline = 0x0000;

    g_wPrevPC = 0;
}

// blState == TRUE ->suspend, blState == FALSE ->start
BOOL SuspendEmulator(BOOL blState) {
    //	if(!hThread) return FALSE;

    if (blState)
        gblPause = TRUE;
    else {
        gblPause = FALSE;

        //		ResumeThread(hThread);
    }
    return TRUE;
}

BOOL IsEmulatorSuspended() { return gblPause; }

void StopEmulator() {
    EnterCriticalSection(&CriticalSection);

    if (!g_nTimerID) goto RET;

    timeKillEvent(g_nTimerID);
    timeEndPeriod(EMULATOR_TIMER_INTERVAL);
    g_nTimerID = 0;

    ClearEmuBackBuffer();
    InvalidateRect(ghEmuWnd, NULL, FALSE);

RET:
    LeaveCriticalSection(&CriticalSection);
}

void StartEmulator() {
    EnterCriticalSection(&CriticalSection);

    if (g_nTimerID) goto RET;

    timeBeginPeriod(EMULATOR_TIMER_INTERVAL);
    g_nTimerID = timeSetEvent(EMULATOR_TIMER_INTERVAL, 0, (LPTIMECALLBACK)Run6502, 0,
                              TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

RET:
    LeaveCriticalSection(&CriticalSection);
}

BOOL IsEmulatorRunning() { return (g_nTimerID) ? TRUE : FALSE; }

BOOL SetupEmulator(EMULATORSETUP* psEmuSetup) {
    if (pb6502CPUMemory) {
        if (psEmuSetup->pbTRAINER)
            memcpy(pb6502CPUMemory + 0x7000, psEmuSetup->pbTRAINER, 0x200);
        if (psEmuSetup->pbPRGROM)
            memcpy(pb6502CPUMemory + 0x8000, psEmuSetup->pbPRGROM, 0x8000);
        if (psEmuSetup->pbCHRROM) memcpy(pbVRAM, psEmuSetup->pbCHRROM, 0x2000);
        if (psEmuSetup->pbVRAM) memcpy(pbVRAM + 0x2000, psEmuSetup->pbVRAM, 0x2000);

        if (psEmuSetup->CPUReg.blValid) {
            m6502zpGetContext(psM6502);
            psM6502->m6502af = psEmuSetup->CPUReg.Reg.wAF;
            psM6502->m6502pc = psEmuSetup->CPUReg.Reg.wPC;
            psM6502->m6502s = psEmuSetup->CPUReg.Reg.wS;
            psM6502->m6502x = psEmuSetup->CPUReg.Reg.wX;
            psM6502->m6502y = psEmuSetup->CPUReg.Reg.wY;
            m6502zpSetContext(psM6502);
        }
    } else
        return FALSE;

    return TRUE;
}

static BOOL LoadRAMData(LPTSTR ResName) {
    HGLOBAL hRAMData;
    BYTE* pbRAMData;

    hRAMData = LoadResource(NULL, FindResource(NULL, ResName, __T("BINARY")));
    if (!hRAMData) return FALSE;
    pbRAMData = (BYTE*)LockResource(hRAMData);
    if (!pbRAMData) {
        FreeResource(hRAMData);
        return FALSE;
    }
    memcpy(pb6502CPUMemory, pbRAMData, 0x800);
    UnlockResource(hRAMData);
    FreeResource(hRAMData);
    return TRUE;
}

void TestPlaySetup(TESTPLAYSETUP* psTPS) {
    WORD wMapAddr;
    BYTE bTmpBuf;

    if (!LoadRAMData(__T("RAMDATA1"))) return;

    // Reg
    m6502zpGetContext(psM6502);
    psM6502->m6502pc = 0x8E16;
    psM6502->m6502af = 0xA493;
    psM6502->m6502x = 0x07;
    psM6502->m6502y = 0x10;
    psM6502->m6502s = 0xF5;
    m6502zpSetContext(psM6502);

    bPPUCtrlReg1 = 0x10;
    bPPUCtrlReg2 = 0x1E;
    bPPUStaReg = 0x00;
    wScanline = 0xF7;

    wVRAMAddr = 0x0000;

    // ルームの属性
    // attribute of room
    pb6502CPUMemory[0x74E] = ((psTPS->bRoomID >> 5) & 0x03);

    pb6502CPUMemory[0x750] = (psTPS->bRoomID & 0x1F);

    // マップヘッダの処理
    // Process map header
    bTmpBuf = pb6502CPUMemory[psTPS->MapAddress.word];

    pb6502CPUMemory[0x741] = psTPS->bBackObject1;
    pb6502CPUMemory[0x744] = psTPS->bBackObject2;

    // 出現位置
    // appearance position
    pb6502CPUMemory[0x710] = ((bTmpBuf >> 3) & 0x07);

    // 持ち時間
    // Duration
    if (((bTmpBuf >> 6) & 0x03))
        pb6502CPUMemory[0x715] = ((bTmpBuf >> 6) & 0x03);
    else {
        pb6502CPUMemory[0x715] = 0x00;
        pb6502CPUMemory[0x7F8] = 0x00;
    }

    bTmpBuf = pb6502CPUMemory[psTPS->MapAddress.word + 1];

    // 初期基本背景ブロック
    // Initial basic background block
    pb6502CPUMemory[0x727] = psTPS->bBasicBlock;

    // 初期景色
    // Initial Landscape
    pb6502CPUMemory[0x742] = psTPS->bBackView;

    if (((bTmpBuf >> 6) & 0x03) == 0x03) {
        pb6502CPUMemory[0x733] = 0x00;
        pb6502CPUMemory[0x743] = 0x03;
    } else
        pb6502CPUMemory[0x733] = ((bTmpBuf >> 6) & 0x03);

    // アドレス
    // address
    wMapAddr = psTPS->MapAddress.word + 2;
    pb6502CPUMemory[0xE7] = (BYTE)(wMapAddr & 0xFF);
    pb6502CPUMemory[0xE8] = (BYTE)((wMapAddr >> 8) & 0xFF);

    pb6502CPUMemory[0xE9] = psTPS->BadGuysAddress.byte.bLower;
    pb6502CPUMemory[0xEA] = psTPS->BadGuysAddress.byte.bUpper;

    pb6502CPUMemory[0x75F] = psTPS->bWorld;
    pb6502CPUMemory[0x75C] = psTPS->bArea;
    pb6502CPUMemory[0x760] = psTPS->bArea2;

    pb6502CPUMemory[0x6CC] = psTPS->bIsDifficult;
    if (pb6502CPUMemory[0x903B] <= psTPS->bWorld &&
        pb6502CPUMemory[0x9044] <= psTPS->bArea)
        pb6502CPUMemory[0x6CC] = 0x01;

    pb6502CPUMemory[0x725] = psTPS->bPage;

    memcpy(pb6502CPUMemory + 0x72D, psTPS->bLeftObjOfs, 3);
    memcpy(pb6502CPUMemory + 0x730, psTPS->bLeftObjNum, 3);

    // 階段
    // Stairs
    pb6502CPUMemory[0x734] = psTPS->bLeftObjData1;

    // きのこの島の茎
    // stem of mushrooms island
    memcpy(pb6502CPUMemory + 0x736, psTPS->bLeftObjData2, 3);

    pb6502CPUMemory[0x72C] = psTPS->bMapOfs;
    pb6502CPUMemory[0x72A] = psTPS->bMapPage;
    pb6502CPUMemory[0x72B] = psTPS->bMapPageFlag;

    pb6502CPUMemory[0x739] = psTPS->bBadGuysOfs;
    pb6502CPUMemory[0x73A] = psTPS->bBadGuysPage;
    pb6502CPUMemory[0x73B] = psTPS->bBadGuysPageFlag;
    pb6502CPUMemory[0x71B] = psTPS->bBadGuysPage2;

    pb6502CPUMemory[0x7FC] = psTPS->bIsCleared;
    pb6502CPUMemory[0x76A] = psTPS->bIsCleared;

    pb6502CPUMemory[0x754] = psTPS->bMarioSize;
    pb6502CPUMemory[0x756] = psTPS->bMarioCap;

    // 上の表示をクリアー
    // Clear the above display
    memset(pbVRAM + 0x2000, 0x24, 128);
    memset(pbVRAM + 0x23C0, 0xAA, 0x40);
    memset(pbVRAM + 0x2400, 0x24, 128);
    memset(pbVRAM + 0x27C0, 0xAA, 0x40);
}

void TestPlaySetupEx(TESTPLAYSETUPEX* psTPSEx) {
    if (!LoadRAMData(__T("RAMDATA2"))) return;

    // Reg
    m6502zpGetContext(psM6502);
    psM6502->m6502pc = 0x8E16;
    psM6502->m6502af = 0xA48F;
    psM6502->m6502x = 0xFF;
    psM6502->m6502y = 0x02;
    psM6502->m6502s = 0xF9;
    m6502zpSetContext(psM6502);

    bPPUCtrlReg1 = 0x10;
    bPPUCtrlReg2 = 0x1E;
    bPPUStaReg = 0x40;
    wScanline = 0x1F;

    wVRAMAddr = 0x0000;

    //----------------------------------
    // マリオの初期位置を設定するハック
    // Hack to set the initial position of Mario
    //----------------------------------
    // 00009165: B9 16 91	lda $9116,y  ; 初期横位置 [Initial lateral position]
    if (psTPSEx->fPosXHack) {
        pb6502CPUMemory[0x9165] = 0xA9;
        pb6502CPUMemory[0x9166] = psTPSEx->bPosX;
        pb6502CPUMemory[0x9167] = 0xEA;
    }

    // 0000916A: BD 1C 91	lda $911C,x  ; 初期縦位置 [Initial vertical position]
    if (psTPSEx->fPosYHack) {
        pb6502CPUMemory[0x916A] = 0xA9;
        pb6502CPUMemory[0x916B] = psTPSEx->bPosY;
        pb6502CPUMemory[0x916C] = 0xEA;
    }

    //-------------
    // 無敵ハック
    // Invincible Hack
    //-------------
    if (psTPSEx->bInvincible) {
        // ファイアバー
        // Fire bar
        pb6502CPUMemory[0xCE0D] = 0xA9;
        pb6502CPUMemory[0xCE0E] = 0x01;
        pb6502CPUMemory[0xCE0F] = 0xEA;

        // 普通の敵
        // Ordinary enemies
        pb6502CPUMemory[0xD92C] = 0xA9;
        pb6502CPUMemory[0xD92D] = 0x01;
        pb6502CPUMemory[0xD92E] = 0xEA;
    }

    //$075A：マリオの残機
    // ○ミスすると-1　00でミスをするとFF　-1したときの値が80-FFならゲームオーバー
    // $075A: the remainder of Mario
    // -If you make a mistake, if you make a mistake at -100, if the value at FF -1 is
    // 80-FF game over
    pb6502CPUMemory[0x715] = pb6502CPUMemory[0x7F8] = 0x00;

    pb6502CPUMemory[0x752] = 0x01;
    pb6502CPUMemory[0x751] = psTPSEx->bPage;
    pb6502CPUMemory[0x750] = psTPSEx->bRoomID;

    pb6502CPUMemory[0x7FC] = pb6502CPUMemory[0x76A] = psTPSEx->bIsCleared;

    pb6502CPUMemory[0x6CC] = psTPSEx->bIsDifficult;
    if (pb6502CPUMemory[0x903B] <= psTPSEx->bWorld &&
        pb6502CPUMemory[0x9044] <= psTPSEx->bArea)
        pb6502CPUMemory[0x6CC] = 0x01;

    pb6502CPUMemory[0x75F] = psTPSEx->bWorld;
    pb6502CPUMemory[0x75C] = psTPSEx->bArea;
    pb6502CPUMemory[0x760] = psTPSEx->bArea2;

    pb6502CPUMemory[0x754] = psTPSEx->bMarioSize;
    pb6502CPUMemory[0x756] = psTPSEx->bMarioCap;

    // 敵を0ページから表示するハック
    // hack to display enemies from 0 page
    if (psTPSEx->bBadGuyHack) {
        pb6502CPUMemory[0xC1BB] = 0x4C;
        pb6502CPUMemory[0xC1BC] = 0xCB;
        pb6502CPUMemory[0xC1BD] = 0xC1;
        pb6502CPUMemory[0xC1BE] = 0xEA;
        pb6502CPUMemory[0xC1BF] = 0xEA;
    }
}

void DirectWriteToEmulatorRAM(WORD wAddr, LPBYTE lpBuf, WORD wSize) {
    memcpy(pb6502CPUMemory + wAddr, lpBuf, wSize);
}

/****************

  セーブとロード

  Save and load

*****************/
typedef struct {
    // RAM
    BYTE pbRAM[0x800];

    // CPU registers
    CONTEXTM6502 sM6502;

    // PPU
    BYTE bPPUCtrlReg1;
    BYTE bPPUCtrlReg2;
    BYTE bPPUStaReg;

    // back ground scroll
    BOOL blBGIsVertical;
    BYTE bBGScrlH;
    BYTE bBGScrlV;

    WORD wScanline;

    // VRAM
    BYTE pbVRAM[0x2000];
    BOOL blVRAMAddrLow;
    WORD wVRAMAddr;
    BOOL blVRAMFirstRead;

    // sprite
    BYTE pbSPRRAM[0x100];
    BYTE bSPRRAMAddr;
    BYTE gbBGColor;
    WORD wSprite0Tile;

    // JoyStick
    BYTE bJoy1Read;
    BYTE bJoy2Read;
} SAVERAMSTRUCT;

SAVERAMSTRUCT* g_psSRS = NULL;

BOOL LoadEmulatorState(EMULATORSETUP* psEmuSetup) {
    if (!g_psSRS) return FALSE;

    ResetEmulator();
    StopEmulator();

    if (psEmuSetup) SetupEmulator(psEmuSetup);

    memcpy(pb6502CPUMemory, g_psSRS->pbRAM, 0x800);

    // CPU registers
    m6502zpSetContext(&g_psSRS->sM6502);

    // PPU
    bPPUCtrlReg1 = g_psSRS->bPPUCtrlReg1;
    bPPUCtrlReg2 = g_psSRS->bPPUCtrlReg2;
    bPPUStaReg = g_psSRS->bPPUStaReg;

    // back ground scroll
    blBGIsVertical = g_psSRS->blBGIsVertical;
    bBGScrlH = g_psSRS->bBGScrlH;
    bBGScrlV = g_psSRS->bBGScrlV;

    wScanline = g_psSRS->wScanline;

    // VRAM
    memcpy(pbVRAM + 0x2000, g_psSRS->pbVRAM, 0x2000);
    wVRAMAddr = g_psSRS->wVRAMAddr;
    g_blVRAMAddrLow = g_psSRS->blVRAMAddrLow;
    blVRAMFirstRead = g_psSRS->blVRAMFirstRead;

    // sprite
    memcpy(pbSPRRAM, g_psSRS->pbSPRRAM, 0x100);
    bSPRRAMAddr = g_psSRS->bSPRRAMAddr;
    gbBGColor = g_psSRS->gbBGColor;

    SetEmuBackBufferPallete(&g_psSRS->pbVRAM[0x1F00]);

    wSprite0Tile = g_psSRS->wSprite0Tile;

    // JoyStick
    bJoy1Read = g_psSRS->bJoy1Read;
    bJoy2Read = g_psSRS->bJoy2Read;

    StartEmulator();

    return TRUE;
}

BOOL IsEmulatorSavePresent() { return (g_psSRS) ? TRUE : FALSE; }

void FreeEmulatorSaveBuffer() {
    if (g_psSRS) Mfree(g_psSRS);
    g_psSRS = NULL;
}

BOOL SaveEmulatorState() {
    if (!g_nTimerID) return FALSE;

    if (!g_psSRS && (g_psSRS = (SAVERAMSTRUCT*)Malloc(sizeof(SAVERAMSTRUCT))) == NULL) return FALSE;

    StopEmulator();
    memcpy(g_psSRS->pbRAM, pb6502CPUMemory, 0x800);

    // CPU registers
    m6502zpGetContext(&g_psSRS->sM6502);

    // PPU
    g_psSRS->bPPUCtrlReg1 = bPPUCtrlReg1;
    g_psSRS->bPPUCtrlReg2 = bPPUCtrlReg2;
    g_psSRS->bPPUStaReg = bPPUStaReg;

    // back ground scroll
    g_psSRS->blBGIsVertical = blBGIsVertical;
    g_psSRS->bBGScrlH = bBGScrlH;
    g_psSRS->bBGScrlV = bBGScrlV;

    g_psSRS->wScanline = wScanline;

    // VRAM
    memcpy(g_psSRS->pbVRAM, pbVRAM + 0x2000, 0x2000);
    g_psSRS->wVRAMAddr = wVRAMAddr;
    g_psSRS->blVRAMAddrLow = g_blVRAMAddrLow;
    g_psSRS->blVRAMFirstRead = blVRAMFirstRead;

    // sprite
    memcpy(g_psSRS->pbSPRRAM, pbSPRRAM, 0x100);
    g_psSRS->bSPRRAMAddr = bSPRRAMAddr;
    g_psSRS->gbBGColor = gbBGColor;
    g_psSRS->wSprite0Tile = 0x100;  // wSprite0Tile;

    // JoyStick
    g_psSRS->bJoy1Read = bJoy1Read;
    g_psSRS->bJoy2Read = bJoy2Read;

    LoadEmulatorState(NULL);

    return TRUE;
}

LRESULT FAR PASCAL EmuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static RECT rcClient;
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc;

            BeginPaint(hWnd, &ps);

            hdc = ps.hdc;

            SelectPalette(hdc, g_hPal, FALSE);

            RealizePalette(hdc);

            SetStretchBltMode(hdc, COLORONCOLOR);

            StretchDIBits(hdc,  // Handle to the device context.
                          0, 0, rcClient.right, rcClient.bottom, 0, 8, NES_SCREENSIZEX,
                          NES_VISIBLESIZEY, gpbBmBufOffScreen, g_lpBmInfo,
                          DIB_RGB_COLORS, SRCCOPY);

            EndPaint(hWnd, &ps);

            break;
        }
        case WM_SIZE:
            GetClientRect(hWnd, &rcClient);
            break;
        case WM_MDIACTIVATE:
            if (!IsEmulatorRunning()) break;
            if ((HWND)lParam == hWnd)
                SuspendEmulator(FALSE);
            else
                SuspendEmulator(TRUE);
            break;
        case WM_CREATE: {
            ghEmuWnd = hWnd;

            // DisableIME
            ImmAssociateContext(hWnd, (HIMC)NULL);

            // Create Emulator
            if (ghEmuWnd) {
                // 初期化するために必要な値のロードもあるのでCreateNester()の前の呼び出す。
                // There is also loading of the necessary values ​​to initialize, so
                // call before CreateNester().
                LoadEmulatorSetting();
                LoadEmuKeySetting();
                CreateNester();
            }
        } break;
        case WM_DESTROY: {
            SaveEmulatorSetting();

            // Destroy Emulator
            StopEmulator();
            DestoryNester();
            ghEmuWnd = NULL;
        } break;
        case WM_SYSCOMMAND: {
            if (wParam == SC_CLOSE) {
                StopEmulator();
                ShowWindow(hWnd, SW_SHOWMINIMIZED);
                return 0;
            }
        } break;
        case WM_SIZING: {
            /*
            WMSZ_BOTTOM      Bottom edge
            WMSZ_BOTTOMLEFT  Bottom-left corner
            WMSZ_BOTTOMRIGHT Bottom-right corner
            WMSZ_LEFT        Left edge
            WMSZ_RIGHT       Right edge
            WMSZ_TOP         Top edge
            WMSZ_TOPLEFT     Top-left corner
            WMSZ_TOPRIGHT    Top-right corner
            */
            WPARAM fwSide = wParam;        // edge of window being sized
            LPRECT lprc = (LPRECT)lParam;  // screen coordinates of drag rectangle
            int iSizeRate, iLength;
            switch (fwSide) {
                case WMSZ_TOP:
                case WMSZ_BOTTOM:
                    iLength = lprc->bottom - lprc->top;
                    iSizeRate = (iLength - EMULATOR_WINDOWFRAMESIZEY -
                                 NES_VISIBLESIZEY / 2) /
                                NES_VISIBLESIZEY;
                    break;
                case WMSZ_RIGHT:
                case WMSZ_LEFT:
                    iLength = lprc->right - lprc->left;
                    iSizeRate = (iLength - EMULATOR_WINDOWFRAMESIZEX -
                                 NES_SCREENSIZEX / 2) /
                                NES_SCREENSIZEX;
                    break;
                case WMSZ_TOPLEFT:
                case WMSZ_TOPRIGHT:
                case WMSZ_BOTTOMLEFT:
                case WMSZ_BOTTOMRIGHT:
                    iLength = lprc->bottom - lprc->top;
                    iSizeRate = (iLength - EMULATOR_WINDOWFRAMESIZEY -
                                 NES_VISIBLESIZEY / 2) /
                                NES_VISIBLESIZEY;
                    break;
            }
            if (iSizeRate < 0) iSizeRate = 0;
            iSizeRate++;
            lprc->right = lprc->left + EMULATOR_WINDOWFRAMESIZEX +
                          NES_SCREENSIZEX * iSizeRate;
            lprc->bottom = lprc->top + EMULATOR_WINDOWFRAMESIZEY +
                           NES_VISIBLESIZEY * iSizeRate;

            return TRUE;
        } break;
    }
    return DefMDIChildProc(hWnd, message, wParam, lParam);
}

BOOL RegisterEmuWndClass(HINSTANCE hInstance) {
    WNDCLASS wc;

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
    wc.lpfnWndProc = EmuWndProc;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, __T("EMUICON"));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = EMUWNDCLASSNAME;
    if (!RegisterClass(&wc)) return FALSE;

    return TRUE;
}

HWND CreateEmulatorWnd(HINSTANCE hInstance, HWND hWndMDIClient) {
    HWND hWnd;

    if (ghEmuWnd) return FALSE;

    // WS_VISIBLEを指定して作成しないと、Windowﾒﾆｭｰにｳｲﾝﾄﾞｳが追加されない。
    // Windows are not added to the Window menu unless you specify WS_VISIBLE.
    hWnd = CreateMDIWindow(
            EMUWNDCLASSNAME, STRING_WINDOW_EMULATOR,
            WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE, 30,
            20, EMULATOR_WINDOWFRAMESIZEX + NES_SCREENSIZEX,
            EMULATOR_WINDOWFRAMESIZEY + NES_VISIBLESIZEY, hWndMDIClient, hInstance, 0);
    return hWnd;
}

HWND GetEmuWndHandle() { return ghEmuWnd; }
