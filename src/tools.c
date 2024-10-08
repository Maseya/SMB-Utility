﻿/**********************************************************************

                                  smb Utility

  File: tools.c
  Description:
  History:

 *********************************************************************/
#include "tools.h"

#include "common.h"
#include "objdata.h"
#include "objlib.h"
#include "objlist.h"
#include "objview.h"
#include "roommng.h"
#include "roomseldlg.h"
#include "smbutil.h"

// プロパティシートの戻り値
// Property sheet return value
BOOL g_blOK;
/****************

  文字列の編集

  Edit String

*****************/
typedef struct {
    LPTSTR pName;
    BYTE bMaxLen;   // Max characters
    BYTE bRomType;  // 0=PRGROM 1=CHRROM
    WORD wOffset;   //
} SMBSTRINGINFO;

#define SMB_STRING_MAXCHARS (27 * 3 + 1)

SMBSTRINGINFO* smbStringData;

extern BYTE bPRGROM[INES_PRGROMBANKSIZE * SMB_NUM_PRGS + 0x8000];
extern BYTE bCHRROM[INES_CHRROMBANKSIZE * SMB_NUM_CHARS];

static TCHAR ConvertData2Char(BYTE bData) {
    TCHAR cRet;

    switch (bData) {
        case 0x00:
            cRet = __T('0');
            break;
        case 0x01:
            cRet = __T('1');
            break;
        case 0x02:
            cRet = __T('2');
            break;
        case 0x03:
            cRet = __T('3');
            break;
        case 0x04:
            cRet = __T('4');
            break;
        case 0x05:
            cRet = __T('5');
            break;
        case 0x06:
            cRet = __T('6');
            break;
        case 0x07:
            cRet = __T('7');
            break;
        case 0x08:
            cRet = __T('8');
            break;
        case 0x09:
            cRet = __T('9');
            break;
        case 0x0A:
            cRet = __T('A');
            break;
        case 0x0B:
            cRet = __T('B');
            break;
        case 0x0C:
            cRet = __T('C');
            break;
        case 0x0D:
            cRet = __T('D');
            break;
        case 0x0E:
            cRet = __T('E');
            break;
        case 0x0F:
            cRet = __T('F');
            break;
        case 0x10:
            cRet = __T('G');
            break;
        case 0x11:
            cRet = __T('H');
            break;
        case 0x12:
            cRet = __T('I');
            break;
        case 0x13:
            cRet = __T('J');
            break;
        case 0x14:
            cRet = __T('K');
            break;
        case 0x15:
            cRet = __T('L');
            break;
        case 0x16:
            cRet = __T('M');
            break;
        case 0x17:
            cRet = __T('N');
            break;
        case 0x18:
            cRet = __T('O');
            break;
        case 0x19:
            cRet = __T('P');
            break;
        case 0x1A:
            cRet = __T('Q');
            break;
        case 0x1B:
            cRet = __T('R');
            break;
        case 0x1C:
            cRet = __T('S');
            break;
        case 0x1D:
            cRet = __T('T');
            break;
        case 0x1E:
            cRet = __T('U');
            break;
        case 0x1F:
            cRet = __T('V');
            break;
        case 0x20:
            cRet = __T('W');
            break;
        case 0x21:
            cRet = __T('X');
            break;
        case 0x22:
            cRet = __T('Y');
            break;
        case 0x23:
            cRet = __T('Z');
            break;
        case 0x24:
            cRet = __T(' ');
            break;

        case 0x28:
            cRet = __T('-');
            break;
        case 0x29:
            cRet = __T('*');
            break;
        case 0x2B:
            cRet = __T('!');
            break;
        case 0xCF:
            cRet = __T('@');
            break;
        case 0xAF:
            cRet = __T('.');
            break;
        default:
            cRet = __T('?');
            break;  // Space
    }
    return cRet;
}

static BYTE ConvertChr2Data(TCHAR cChar, BOOL* blUnknown) {
    BYTE bRet;

    *blUnknown = FALSE;
    switch (cChar) {
        case __T('0'):
            bRet = 0x0;
            break;
        case __T('1'):
            bRet = 0x1;
            break;
        case __T('2'):
            bRet = 0x2;
            break;
        case __T('3'):
            bRet = 0x3;
            break;
        case __T('4'):
            bRet = 0x4;
            break;
        case __T('5'):
            bRet = 0x5;
            break;
        case __T('6'):
            bRet = 0x6;
            break;
        case __T('7'):
            bRet = 0x7;
            break;
        case __T('8'):
            bRet = 0x8;
            break;
        case __T('9'):
            bRet = 0x9;
            break;
        case __T('a'):
            bRet = 0xa;
            break;
        case __T('b'):
            bRet = 0xb;
            break;
        case __T('c'):
            bRet = 0xc;
            break;
        case __T('d'):
            bRet = 0xd;
            break;
        case __T('e'):
            bRet = 0xe;
            break;
        case __T('f'):
            bRet = 0xf;
            break;
        case __T('g'):
            bRet = 0x10;
            break;
        case __T('h'):
            bRet = 0x11;
            break;
        case __T('i'):
            bRet = 0x12;
            break;
        case __T('j'):
            bRet = 0x13;
            break;
        case __T('k'):
            bRet = 0x14;
            break;
        case __T('l'):
            bRet = 0x15;
            break;
        case __T('m'):
            bRet = 0x16;
            break;
        case __T('n'):
            bRet = 0x17;
            break;
        case __T('o'):
            bRet = 0x18;
            break;
        case __T('p'):
            bRet = 0x19;
            break;
        case __T('q'):
            bRet = 0x1a;
            break;
        case __T('r'):
            bRet = 0x1b;
            break;
        case __T('s'):
            bRet = 0x1c;
            break;
        case __T('t'):
            bRet = 0x1d;
            break;
        case __T('u'):
            bRet = 0x1e;
            break;
        case __T('v'):
            bRet = 0x1f;
            break;
        case __T('w'):
            bRet = 0x20;
            break;
        case __T('x'):
            bRet = 0x21;
            break;
        case __T('y'):
            bRet = 0x22;
            break;
        case __T('z'):
            bRet = 0x23;
            break;
        case __T(' '):
            bRet = 0x24;
            break;

        case __T('-'):
            bRet = 0x28;
            break;
        case __T('*'):
            bRet = 0x29;
            break;
        case __T('!'):
            bRet = 0x2B;
            break;
        case __T('@'):
            bRet = 0xCF;
            break;
        case __T('.'):
            bRet = 0xAF;
            break;
        default: {
            bRet = 0x24;
            *blUnknown = TRUE;
        } break;  // Space
    }
    return bRet;
}

static int numStrings;

static int GetNumStrings() { return numStrings; }

static void ChangeString(LRESULT iStringNum, LPTSTR pString) {
    BYTE* pbTmp;
    BYTE bTmp;
    BOOL blEOB = FALSE;
    BOOL blUnknown = FALSE;
    int i, n;

    CharLower(pString);  // small char
    pbTmp = (smbStringData[iStringNum].bRomType) ? bCHRROM : bPRGROM;
    pbTmp += smbStringData[iStringNum].wOffset;
    for (i = 0, n = 0; i < smbStringData[iStringNum].bMaxLen; i++) {
        if (*(pString + n) == __T('\0')) blEOB = TRUE;
        bTmp = 0x24;  // space
        if (!blEOB) {
            bTmp = ConvertChr2Data(*(pString + n), &blUnknown);
            if (blUnknown) {
                TCHAR cTmp[4] = {0};
                memcpy(cTmp, &pString[n], 3);
                _stscanf(cTmp, __T("#%hhx"), &bTmp);
                n += 3;
            } else
                n++;
        }
        pbTmp[i] = bTmp;
    }
}

static void GetString(UINT iStringNum, LPTSTR pString, UINT iBufSize) {
    UINT i, n;
    BYTE* pbTmp;
    TCHAR tcTmp;

    memset(pString, 0, iBufSize);
    pbTmp = (smbStringData[iStringNum].bRomType) ? bCHRROM : bPRGROM;
    pbTmp += smbStringData[iStringNum].wOffset;
    for (i = 0, n = 0; (i < smbStringData[iStringNum].bMaxLen) && (n < iBufSize); i++) {
        tcTmp = ConvertData2Char(pbTmp[i]);
        if (tcTmp == __T('?')) {
            _stprintf(&pString[n], __T("#%.2X"), pbTmp[i]);
            n += 3;
        } else
            pString[n++] = tcTmp;
    }
}

static int GetStringMaxChars(UINT iStringNum) {
    return smbStringData[iStringNum].bMaxLen;
}

LRESULT CALLBACK StringEditDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                   LPARAM lParam) {
    static BOOL sblWritten;
    switch (message) {
        case WM_INITDIALOG: {
            int i;
            TCHAR cBuf[SMB_STRING_MAXCHARS + 1];

            sblWritten = FALSE;

            memset(cBuf, 0, SMB_STRING_MAXCHARS + 1);
            GetString(0, cBuf, SMB_STRING_MAXCHARS);
            SetDlgItemText(hDlg, IDC_STRING, cBuf);
            for (i = 0; i < GetNumStrings(); i++)
                SendDlgItemMessage(hDlg, IDC_STRINGSELECT, CB_ADDSTRING, 0,
                                   (LPARAM)smbStringData[i].pName);

            SendDlgItemMessage(hDlg, IDC_STRINGSELECT, CB_SETCURSEL, 0, 0);

            SetDlgItemInt(hDlg, IDC_LEN, GetStringMaxChars(0), FALSE);
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDCANCEL:
                    EndDialog(hDlg, TRUE);
                    return TRUE;
                case IDC_STRINGSELECT: {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        int iSel = 0, iMaxLen;
                        TCHAR cBuf[SMB_STRING_MAXCHARS + 1];

                        memset(cBuf, 0, SMB_STRING_MAXCHARS + 1);
                        iSel = (int)SendDlgItemMessage(hDlg, IDC_STRINGSELECT,
                                                       CB_GETCURSEL, 0, 0);
                        if (iSel == CB_ERR) return TRUE;
                        GetString(iSel, cBuf, SMB_STRING_MAXCHARS);
                        SetDlgItemText(hDlg, IDC_STRING, cBuf);

                        iMaxLen = GetStringMaxChars(iSel);
                        SetDlgItemInt(hDlg, IDC_LEN, iMaxLen, FALSE);

                        return TRUE;
                    }
                }
            }
        case BN_CLICKED:
            if (LOWORD(wParam) == IDOK) {
                LRESULT iSel = 0;
                TCHAR cBuf[SMB_STRING_MAXCHARS + 1];

                memset(cBuf, 0, SMB_STRING_MAXCHARS + 1);

                iSel = SendDlgItemMessage(hDlg, IDC_STRINGSELECT, CB_GETCURSEL, 0, 0);
                if (iSel == CB_ERR) return TRUE;
                GetDlgItemText(hDlg, IDC_STRING, cBuf, SMB_STRING_MAXCHARS);
                if (!sblWritten) {
                    undoPrepare(UNDONAME_TOOLSTR);
                    sblWritten = TRUE;
                }
                ChangeString(iSel, cBuf);

                fr_SetDataChanged(TRUE);
                return TRUE;
            }
    }
    return FALSE;
}

/********************

  ループコマンド

  Loop command

*********************/

LRESULT CALLBACK LoopEditDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                 LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            BYTE* bTmp;
            TCHAR cBuf[34];

            bTmp = bPRGROM + SMBADDRESS_LOOP_WORLD;
            _stprintf(cBuf,
                      __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"),
                      bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6],
                      bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
            SetDlgItemText(hDlg, IDC_WORLD, cBuf);
            bTmp = bPRGROM + SMBADDRESS_LOOP_PAGE;
            _stprintf(cBuf,
                      __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"),
                      bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6],
                      bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
            SetDlgItemText(hDlg, IDC_PAGE, cBuf);
            bTmp = bPRGROM + SMBADDRESS_LOOP_YPOS;
            _stprintf(cBuf,
                      __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"),
                      bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6],
                      bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
            SetDlgItemText(hDlg, IDC_YPOS, cBuf);
            bTmp = bPRGROM + SMBADDRESS_LOOP_RETURNPOS;
            _stprintf(cBuf,
                      __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"),
                      bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6],
                      bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
            SetDlgItemText(hDlg, IDC_RETURNPOS, cBuf);

            _stprintf(cBuf, __T("%.2x"), bPRGROM[SMBADDRESS_LOOP_W7DATA1]);
            SetDlgItemText(hDlg, IDC_DATA1, cBuf);
            _stprintf(cBuf, __T("%.2x"), bPRGROM[SMBADDRESS_LOOP_W7DATA2]);
            SetDlgItemText(hDlg, IDC_DATA2, cBuf);
            return TRUE;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK: {
                    BYTE bTmp[4][11] = {0};
                    BYTE bData[2] = {0};
                    TCHAR cBuf[34];

                    memset(cBuf, 0, 34);
                    GetDlgItemText(hDlg, IDC_WORLD, cBuf, 34);
                    if (11 != _stscanf(cBuf,
                                       __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx "
                                           "%hhx %hhx %hhx"),
                                       &bTmp[0][0], &bTmp[0][1], &bTmp[0][2],
                                       &bTmp[0][3], &bTmp[0][4], &bTmp[0][5],
                                       &bTmp[0][6], &bTmp[0][7], &bTmp[0][8],
                                       &bTmp[0][9], &bTmp[0][10]))
                        return TRUE;

                    memset(cBuf, 0, 34);
                    GetDlgItemText(hDlg, IDC_PAGE, cBuf, 34);
                    if (11 != _stscanf(cBuf,
                                       __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx "
                                           "%hhx %hhx %hhx"),
                                       &bTmp[1][0], &bTmp[1][1], &bTmp[1][2],
                                       &bTmp[1][3], &bTmp[1][4], &bTmp[1][5],
                                       &bTmp[1][6], &bTmp[1][7], &bTmp[1][8],
                                       &bTmp[1][9], &bTmp[1][10]))
                        return TRUE;

                    memset(cBuf, 0, 34);
                    GetDlgItemText(hDlg, IDC_YPOS, cBuf, 34);
                    if (11 != _stscanf(cBuf,
                                       __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx "
                                           "%hhx %hhx %hhx"),
                                       &bTmp[2][0], &bTmp[2][1], &bTmp[2][2],
                                       &bTmp[2][3], &bTmp[2][4], &bTmp[2][5],
                                       &bTmp[2][6], &bTmp[2][7], &bTmp[2][8],
                                       &bTmp[2][9], &bTmp[2][10]))
                        return TRUE;

                    memset(cBuf, 0, 34);
                    GetDlgItemText(hDlg, IDC_RETURNPOS, cBuf, 34);
                    if (11 != _stscanf(cBuf,
                                       __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx "
                                           "%hhx %hhx %hhx"),
                                       &bTmp[3][0], &bTmp[3][1], &bTmp[3][2],
                                       &bTmp[3][3], &bTmp[3][4], &bTmp[3][5],
                                       &bTmp[3][6], &bTmp[3][7], &bTmp[3][8],
                                       &bTmp[3][9], &bTmp[3][10]))
                        return TRUE;

                    memset(cBuf, 0, 34);
                    GetDlgItemText(hDlg, IDC_DATA1, cBuf, 3);
                    if (1 != _stscanf(cBuf, __T("%hhx"), &bData[0])) return TRUE;

                    memset(cBuf, 0, 34);
                    GetDlgItemText(hDlg, IDC_DATA2, cBuf, 3);
                    if (1 != _stscanf(cBuf, __T("%hhx"), &bData[1])) return TRUE;

                    undoPrepare(UNDONAME_TOOLLOOPBIN);

                    fr_SetDataChanged(TRUE);

                    memcpy(bPRGROM + SMBADDRESS_LOOP_WORLD, bTmp[0], 11);
                    memcpy(bPRGROM + SMBADDRESS_LOOP_PAGE, bTmp[1], 11);
                    memcpy(bPRGROM + SMBADDRESS_LOOP_YPOS, bTmp[2], 11);
                    memcpy(bPRGROM + SMBADDRESS_LOOP_RETURNPOS, bTmp[3], 11);
                    bPRGROM[SMBADDRESS_LOOP_W7DATA1] = bData[0];
                    bPRGROM[SMBADDRESS_LOOP_W7DATA2] = bData[1];
                }
                case IDCANCEL: {
                    EndDialog(hDlg, TRUE);
                    return TRUE;
                } break;
            }
    }

    return FALSE;
}

/*******************

  ゲーム全般の設定

  General game settings

********************/

typedef struct {
    LPTSTR Name;
    BYTE bGfxData[4];
} POLEGFXDATAINFO;

POLEGFXDATAINFO* PoleGfxInfo;
int PoleGfxInfoSize;

int GetPoleGfxDatas() { return PoleGfxInfoSize; }

LRESULT CALLBACK GameSettingDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                    LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            TCHAR cBuf[20];
            BYTE bNewFlower[] = "\xEA\xEA\xEA\xEA\xEA";
            int n, i;

            // 残りマリオの設定
            // Remaining Mario settings
            SendDlgItemMessage(hDlg, IDC_MARIOLEFTSPIN, UDM_SETRANGE, 0,
                               MAKEWPARAM(128, 1));
            SetDlgItemInt(hDlg, IDC_MARIOLEFT, bPRGROM[SMB_MARIO_LEFT] + 1, FALSE);

            // パックンフラワー
            // Fire Flower
            if (!memcmp(bPRGROM + SMB_FLOWER, bNewFlower, 5))
                CheckDlgButton(hDlg, IDC_FLOWER, BST_CHECKED);

            // ポールのグラフィックス
            // Pole GFX
            n = GetPoleGfxDatas() - 1;
            for (i = 0; i < GetPoleGfxDatas(); i++) {
                SendDlgItemMessage(hDlg, IDC_POLEGFX, CB_ADDSTRING, 0,
                                   (LPARAM)PoleGfxInfo[i].Name);
                if (!memcmp(bPRGROM + SMB_POLEGFX, PoleGfxInfo[i].bGfxData, 4)) n = i;
            }
            SendDlgItemMessage(hDlg, IDC_POLEGFX, CB_SETCURSEL, n, 0);

            for (i = 0; i < 10; i++) {
                _stprintf(cBuf, __T("%d"), i * 100);
                SendDlgItemMessage(hDlg, IDC_TIME400, CB_ADDSTRING, 0, (LPARAM)cBuf);
                SendDlgItemMessage(hDlg, IDC_TIME300, CB_ADDSTRING, 0, (LPARAM)cBuf);
                SendDlgItemMessage(hDlg, IDC_TIME200, CB_ADDSTRING, 0, (LPARAM)cBuf);
            }

            _stprintf(cBuf, STRING_UNKNOWN);
            i = bPRGROM[SMB_TIME];
            if (i > 9) {
                SendDlgItemMessage(hDlg, IDC_TIME400, CB_ADDSTRING, 0, (LPARAM)cBuf);
                i = 10;
            }
            SendDlgItemMessage(hDlg, IDC_TIME400, CB_SETCURSEL, i, 0);
            i = bPRGROM[SMB_TIME + 1];
            if (i > 9) {
                SendDlgItemMessage(hDlg, IDC_TIME300, CB_ADDSTRING, 0, (LPARAM)cBuf);
                i = 10;
            }
            SendDlgItemMessage(hDlg, IDC_TIME300, CB_SETCURSEL, i, 0);
            i = bPRGROM[SMB_TIME + 2];
            if (i > 9) {
                SendDlgItemMessage(hDlg, IDC_TIME200, CB_ADDSTRING, 0, (LPARAM)cBuf);
                i = 10;
            }
            SendDlgItemMessage(hDlg, IDC_TIME200, CB_SETCURSEL, i, 0);
            return TRUE;
        }
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch (pnmh->code) {
                case PSN_APPLY: {
                    BOOL blSuccess;
                    LRESULT iRet;
                    LRESULT iPoleGfx;
                    BYTE bFlower[] = "\x0D\x5F\x07\xF0\x2B";
                    BYTE bNewFlower[] = "\xEA\xEA\xEA\xEA\xEA";

                    if (!g_blOK) {
                        undoPrepare(UNDONAME_TOOLOTHER);
                        g_blOK = TRUE;
                    }

                    // 残りマリオ
                    // Remaining Mario
                    iRet = GetDlgItemInt(hDlg, IDC_MARIOLEFT, &blSuccess, FALSE) - 1;
                    if (!blSuccess || (iRet < 0 || iRet > 127)) {
                        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
                        return TRUE;
                    }
                    bPRGROM[SMB_MARIO_LEFT] = (BYTE)iRet;

                    // パックンフラワー
                    // Fire Flower
                    if (BST_CHECKED & IsDlgButtonChecked(hDlg, IDC_FLOWER))
                        memset(bPRGROM + SMB_FLOWER, 0xEA, 5);
                    else if (!memcmp(bPRGROM + SMB_FLOWER, bNewFlower, 5))
                        memcpy(bPRGROM + SMB_FLOWER, bFlower, 5);

                    // ポールのグラフィックス
                    // Pole GFX
                    iPoleGfx =
                            SendDlgItemMessage(hDlg, IDC_POLEGFX, CB_GETCURSEL, 0, 0);
                    if (iPoleGfx == CB_ERR) return TRUE;

                    // その他でなければ…
                    // Otherwise ...
                    if (iPoleGfx != GetPoleGfxDatas() - 1) {
                        memcpy(bPRGROM + SMB_POLEGFX, PoleGfxInfo[iPoleGfx].bGfxData,
                               4);
                    }

                    iRet = SendDlgItemMessage(hDlg, IDC_TIME400, CB_GETCURSEL, 0, 0);
                    if (iRet != 10 && iRet != CB_ERR) bPRGROM[SMB_TIME] = (BYTE)iRet;
                    iRet = SendDlgItemMessage(hDlg, IDC_TIME300, CB_GETCURSEL, 0, 0);
                    if (iRet != 10 && iRet != CB_ERR)
                        bPRGROM[SMB_TIME + 1] = (BYTE)iRet;
                    iRet = SendDlgItemMessage(hDlg, IDC_TIME200, CB_GETCURSEL, 0, 0);
                    if (iRet != 10 && iRet != CB_ERR)
                        bPRGROM[SMB_TIME + 2] = (BYTE)iRet;
                    return TRUE;
                } break;
            }
        } break;
    }
    return FALSE;
}

LRESULT CALLBACK GameSetting1upDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                       LPARAM lParam) {
    static BYTE lpbBuf[SMB_NUM_WORLDS];
    static LRESULT iCurSel;
    switch (message) {
        case WM_INITDIALOG: {
            int n;
            TCHAR cBuf[20];
            ADDRESSDATA ad1up;

            if (lpbBuf) {
                ADDRESSDATA_LOAD(ad1up, SMB_COINSFOR1UP_ADDRESS);

                // １upｷﾉｺのためのｺｲﾝの枚数
                // Number of coins for 1up mushrooms
                memcpy(lpbBuf, bPRGROM + ADDRESSDATA_GET(ad1up), GetNumWorlds());

                for (n = 0; n < GetNumWorlds(); n++) {
                    wsprintf(cBuf, STRING_SETTING_WORLDX, n + 1);
                    SendDlgItemMessage(hDlg, IDC_WORLD, CB_ADDSTRING, 0, (LPARAM)cBuf);
                }
                SendDlgItemMessage(hDlg, IDC_WORLD, CB_SETCURSEL, 0, 0);
                iCurSel = 0;

                SendDlgItemMessage(hDlg, IDC_COINSFOR1UPSPIN, UDM_SETRANGE, 0,
                                   MAKEWPARAM(255, 0));
                SetDlgItemInt(hDlg, IDC_COINSFOR1UP, lpbBuf[iCurSel], FALSE);
            }
            return TRUE;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDC_WORLD: {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        LRESULT iRet;
                        iRet = SendDlgItemMessage(hDlg, IDC_WORLD, CB_GETCURSEL, 0, 0);
                        if (iRet != CB_ERR) {
                            iCurSel = iRet;
                            if (lpbBuf)
                                SetDlgItemInt(hDlg, IDC_COINSFOR1UP, lpbBuf[iRet],
                                              FALSE);
                        }
                    }
                } break;
                case IDC_COINSFOR1UP: {
                    if (HIWORD(wParam) == EN_CHANGE) {
                        BOOL blSuccess;
                        int iTmp;

                        iTmp = GetDlgItemInt(hDlg, IDC_COINSFOR1UP, &blSuccess, FALSE);
                        if (lpbBuf && iCurSel < GetNumWorlds() && blSuccess)
                            lpbBuf[iCurSel] = (BYTE)iTmp;
                    }
                } break;
            }
        } break;
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch (pnmh->code) {
                case PSN_APPLY: {
                    ADDRESSDATA ad1up;
                    if (lpbBuf) {
                        if (!g_blOK) {
                            undoPrepare(UNDONAME_TOOLOTHER);
                            g_blOK = TRUE;
                        }
                        ADDRESSDATA_LOAD(ad1up, SMB_COINSFOR1UP_ADDRESS);
                        memcpy(bPRGROM + ADDRESSDATA_GET(ad1up), lpbBuf,
                               GetNumWorlds());
                    }

                    return TRUE;
                } break;
            }
        } break;
    }
    return FALSE;
}

LRESULT CALLBACK GameSettingWarpZoneDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                            LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            // ワープゾーン
            // Warp zone
            SendDlgItemMessage(hDlg, IDC_WARPASPIN1, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPASPIN2, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPASPIN3, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPBSPIN1, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPBSPIN2, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPBSPIN3, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPCSPIN1, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPCSPIN2, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));
            SendDlgItemMessage(hDlg, IDC_WARPCSPIN3, UDM_SETRANGE, 0,
                               MAKEWPARAM(255, 0));

            SetDlgItemInt(hDlg, IDC_WARPA1, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS], FALSE);
            SetDlgItemInt(hDlg, IDC_WARPA2, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 1],
                          FALSE);
            SetDlgItemInt(hDlg, IDC_WARPA3, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 2],
                          FALSE);
            SetDlgItemInt(hDlg, IDC_WARPB1, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 4],
                          FALSE);
            SetDlgItemInt(hDlg, IDC_WARPB2, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 5],
                          FALSE);
            SetDlgItemInt(hDlg, IDC_WARPB3, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 6],
                          FALSE);
            SetDlgItemInt(hDlg, IDC_WARPC1, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 8],
                          FALSE);
            SetDlgItemInt(hDlg, IDC_WARPC2, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 9],
                          FALSE);
            SetDlgItemInt(hDlg, IDC_WARPC3, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 10],
                          FALSE);

            // プロパテイシートを中央に持ってくる
            // Bring the property sheet to the center
            CenterPropatySheet(hDlg);

            return TRUE;
        }
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch (pnmh->code) {
                case PSN_APPLY: {
                    BOOL blSuccess;
                    int iRet;
                    int n;
                    int iAddrDelta[] = {0, 1, 2, 4, 5, 6, 8, 9, 10};
                    int iCtrlID[] = {IDC_WARPA1, IDC_WARPA2, IDC_WARPA3,
                                     IDC_WARPB1, IDC_WARPB2, IDC_WARPB3,
                                     IDC_WARPC1, IDC_WARPC2, IDC_WARPC3};

                    if (!g_blOK) {
                        undoPrepare(UNDONAME_TOOLOTHER);
                        g_blOK = TRUE;
                    }

                    for (n = 0; n < 9; n++) {
                        iRet = GetDlgItemInt(hDlg, iCtrlID[n], &blSuccess, FALSE);
                        if (!blSuccess || (iRet < 0 || iRet > 255)) {
                            SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
                            return TRUE;
                        }
                        bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + iAddrDelta[n]] = iRet;
                    }
                    return TRUE;
                } break;
                case PSN_RESET: {
                } break;
            }
        } break;
    }
    return FALSE;
}

LRESULT CALLBACK GameSettingKoopaDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                         LPARAM lParam) {
    static BYTE lpbBuf[SMB_NUM_WORLDS];
    static LRESULT iCurSel;

    switch (message) {
        case WM_INITDIALOG: {
            TCHAR cBuf[50];
            int n;
            ADDRESSDATA adKoopa;
            extern SMBBADGUYSINFO* smbBadGuysInfo;

            ADDRESSDATA_LOAD(adKoopa, SMB_KOOPAREALCHARCTER_ADDRESS);

            if (lpbBuf) {
                // １upｷﾉｺのためのｺｲﾝの枚数
                // Number of coins for 1up mushrooms
                memcpy(lpbBuf, bPRGROM + ADDRESSDATA_GET(adKoopa), GetNumWorlds());

                for (n = 0; n < GetNumWorlds(); n++) {
                    wsprintf(cBuf, STRING_SETTING_WORLDX, n + 1);
                    SendDlgItemMessage(hDlg, IDC_KOOPAWORLD, CB_ADDSTRING, 0,
                                       (LPARAM)cBuf);
                }
                SendDlgItemMessage(hDlg, IDC_KOOPAWORLD, CB_SETCURSEL, 0, 0);
                iCurSel = 0;

                for (n = 0; n < 0x40; n++) {
                    SendDlgItemMessage(hDlg, IDC_KOOPA, CB_ADDSTRING, 0,
                                       (LPARAM)smbBadGuysInfo[n].Name);
                }
                SendDlgItemMessage(hDlg, IDC_KOOPA, CB_SETCURSEL, lpbBuf[iCurSel], 0);
            }

            SendDlgItemMessage(hDlg, IDC_WORLDSPIN, UDM_SETRANGE, 0,
                               MAKEWPARAM(256, 1));
            SetDlgItemInt(hDlg, IDC_WORLD, bPRGROM[SMB_KOOPAHAMMER] + 1, FALSE);

            return TRUE;
        } break;
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDC_KOOPAWORLD: {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        LRESULT iRet;

                        iRet = SendDlgItemMessage(hDlg, IDC_KOOPAWORLD, CB_GETCURSEL, 0,
                                                  0);
                        if (iRet != CB_ERR) {
                            iCurSel = iRet;
                            if (lpbBuf)
                                SendDlgItemMessage(hDlg, IDC_KOOPA, CB_SETCURSEL,
                                                   lpbBuf[iRet], 0);
                        }
                    }
                } break;
                case IDC_KOOPA: {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        LRESULT iRet;
                        iRet = SendDlgItemMessage(hDlg, IDC_KOOPA, CB_GETCURSEL, 0, 0);
                        if (lpbBuf && iCurSel < GetNumWorlds() && iRet != CB_ERR)
                            lpbBuf[iCurSel] = (BYTE)iRet;
                    }
                } break;
            }
        } break;
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch (pnmh->code) {
                case PSN_APPLY: {
                    ADDRESSDATA adKoopa;
                    BOOL blSuccess;
                    int iRet;

                    if (!g_blOK) {
                        undoPrepare(UNDONAME_TOOLOTHER);
                        g_blOK = TRUE;
                    }

                    iRet = GetDlgItemInt(hDlg, IDC_WORLD, &blSuccess, FALSE);
                    if (!blSuccess || iRet < 1 || iRet > 256) {
                        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
                        return TRUE;
                    }

                    bPRGROM[SMB_KOOPAHAMMER] = (BYTE)(iRet - 1);

                    if (lpbBuf) {
                        ADDRESSDATA_LOAD(adKoopa, SMB_KOOPAREALCHARCTER_ADDRESS);
                        memcpy(bPRGROM + ADDRESSDATA_GET(adKoopa), lpbBuf,
                               GetNumWorlds());
                    }

                    return TRUE;
                } break;
            }
        } break;
    }
    return FALSE;
}

LRESULT CALLBACK GameSettingWorldDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                         LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            SendDlgItemMessage(hDlg, IDC_DIFFICULTYWORLDSPIN, UDM_SETRANGE, 0,
                               MAKEWPARAM(256, 1));
            SetDlgItemInt(hDlg, IDC_DIFFICULTYWORLD, bPRGROM[SMB_DIFFICULTYWORLD] + 1,
                          FALSE);
            SendDlgItemMessage(hDlg, IDC_DIFFICULTYAREASPIN, UDM_SETRANGE, 0,
                               MAKEWPARAM(256, 1));
            SetDlgItemInt(hDlg, IDC_DIFFICULTYAREA, bPRGROM[SMB_DIFFICULTYAREA] + 1,
                          FALSE);
            SendDlgItemMessage(hDlg, IDC_SEABLOCKWORLDSPIN, UDM_SETRANGE, 0,
                               MAKEWPARAM(256, 1));
            SetDlgItemInt(hDlg, IDC_SEABLOCKWORLD, bPRGROM[SMB_SEABLOCKWORLD] + 1,
                          FALSE);

            return TRUE;
        }
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch (pnmh->code) {
                case PSN_APPLY: {
                    BOOL blSuccess;
                    int iRet;

                    if (!g_blOK) {
                        undoPrepare(UNDONAME_TOOLOTHER);
                        g_blOK = TRUE;
                    }

                    iRet = GetDlgItemInt(hDlg, IDC_DIFFICULTYWORLD, &blSuccess, FALSE) -
                           1;
                    if (!blSuccess || iRet < 0 || iRet > 255) {
                        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
                        return TRUE;
                    }
                    bPRGROM[SMB_DIFFICULTYWORLD] = (BYTE)iRet;

                    iRet = GetDlgItemInt(hDlg, IDC_DIFFICULTYAREA, &blSuccess, FALSE) -
                           1;
                    if (!blSuccess || iRet < 0 || iRet > 255) {
                        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
                        return TRUE;
                    }
                    bPRGROM[SMB_DIFFICULTYAREA] = (BYTE)iRet;

                    iRet = GetDlgItemInt(hDlg, IDC_SEABLOCKWORLD, &blSuccess, FALSE) -
                           1;
                    if (!blSuccess || iRet < 0 || iRet > 255) {
                        SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
                        return TRUE;
                    }
                    bPRGROM[SMB_SEABLOCKWORLD] = (BYTE)iRet;

                    return TRUE;
                } break;
            }
        } break;
    }
    return FALSE;
}

void GameSettingPropertySheet(HWND hwndOwner) {
    // TODO
#define OPTPS_NUM_PAGES 5
    LPTSTR lpTitle[OPTPS_NUM_PAGES] = {STRING_SETTING_WARPZONE, STRING_SETTING_1UP,
                                       STRING_SETTING_KOOPA, STRING_SETTING_WORLD,
                                       STRING_SETTING_OTHER};
    LPTSTR lpDlgResName[OPTPS_NUM_PAGES] = {
            __T("GAMESETTINGWARPZONEDLG"), __T("GAMESETTING1UPDLG"),
            __T("GAMESETTINGKOOPADLG"), __T("GAMESETTINGWORLDDLG"),
            __T("GAMESETTINGDLG")};
    DLGPROC pfnDlgProc[OPTPS_NUM_PAGES] = {
            GameSettingWarpZoneDlgProc, GameSetting1upDlgProc, GameSettingKoopaDlgProc,
            GameSettingWorldDlgProc, GameSettingDlgProc};

    // Local
    PROPSHEETPAGE psp[OPTPS_NUM_PAGES];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < OPTPS_NUM_PAGES; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE;
        psp[i].hInstance = GetModuleHandle(NULL);
        psp[i].pszTemplate = lpDlgResName[i];
        psp[i].pszIcon = NULL;
        psp[i].pfnDlgProc = pfnDlgProc[i];
        psp[i].pszTitle = lpTitle[i];
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = GetModuleHandle(NULL);
    psh.pszIcon = NULL;
    psh.pszCaption = (LPTSTR)STRING_SETTING_TITLE;
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE)&psp;
    psh.pfnCallback = NULL;

    g_blOK = FALSE;
    PropertySheet(&psh);

    if (g_blOK) {
        fr_SetDataChanged(TRUE);
        UpdateObjectView(0);
    }

    return;
}

int InitSmbStringData() {
    SMBSTRINGINFO tmp[] = {
            STRING_STRINGDATA_01, 5,  0, 0x8755, STRING_STRINGDATA_02, 5,  0, 0x879B,
            STRING_STRINGDATA_03, 5,  0, 0x87AE, STRING_STRINGDATA_04, 5,  0, 0x87ed,
            STRING_STRINGDATA_05, 11, 0, 0x875D, STRING_STRINGDATA_06, 5,  0, 0x8786,
            STRING_STRINGDATA_07, 7,  0, 0x87a3, STRING_STRINGDATA_08, 9,  0, 0x87b6,
            STRING_STRINGDATA_09, 21, 0, 0x87c3, STRING_STRINGDATA_10, 16, 0, 0x8d57,
            STRING_STRINGDATA_11, 16, 0, 0x8d6b, STRING_STRINGDATA_12, 22, 0, 0x8d7f,
            STRING_STRINGDATA_13, 15, 0, 0x8d98, STRING_STRINGDATA_14, 19, 0, 0x8dab,
            STRING_STRINGDATA_15, 27, 0, 0x8dc2, STRING_STRINGDATA_16, 13, 0, 0x8de1,
            STRING_STRINGDATA_17, 17, 0, 0x8df2, STRING_STRINGDATA_18, 14, 1, 0x1fa5,
            STRING_STRINGDATA_19, 13, 1, 0x1fb6, STRING_STRINGDATA_20, 13, 1, 0x1fc6};

    smbStringData = (SMBSTRINGINFO*)malloc(sizeof(tmp));
    memcpy(smbStringData, tmp, sizeof(tmp));

    return sizeof(tmp) / sizeof(SMBSTRINGINFO);
}

int InitPoleGfxInfo() {
    POLEGFXDATAINFO tmp[] = {STRING_POLEGFX_DEFAULT, {0x24, 0x2F, 0x24, 0x3D},
                             STRING_POLEGFX_ROPE,    {0xA2, 0xA2, 0xA3, 0xA3},
                             STRING_POLEGFX_TREE,    {0xBE, 0xBE, 0xBF, 0xBF},
                             STRING_POLEGFX_OTHER,   {0xFF, 0xFF, 0xFF, 0xFF}};

    PoleGfxInfo = (POLEGFXDATAINFO*)malloc(sizeof(tmp));
    memcpy(PoleGfxInfo, tmp, sizeof(tmp));

    return sizeof(tmp) / sizeof(POLEGFXDATAINFO);
}

void InitToolData() {
    numStrings = InitSmbStringData();
    PoleGfxInfoSize = InitPoleGfxInfo();
}
