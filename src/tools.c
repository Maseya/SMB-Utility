/************************************************************************************

                                  smb Utility

  File: tools.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "roomseldlg.h"
#include "objlib.h"
#include "roommng.h"
#include "objlist.h"
#include "objview.h"
#include "tools.h"

 //プロパティシートの戻り値
BOOL g_blOK;
/****************

  文字列の編集

*****************/
typedef struct
{
    LPTSTR pName;
    BYTE bMaxLen; //Max characters
    BYTE bRomType;//0=PRGROM 1=CHRROM
    WORD wOffset; //
}SMBSTRINGINFO;

#define SMB_STRING_MAXCHARS (27 * 3 + 1)

SMBSTRINGINFO smbStringData[] = {STRING_STRINGDATA_01, 5,  0, 0x8755,
                               STRING_STRINGDATA_02, 5,  0, 0x879B,
                               STRING_STRINGDATA_03, 5,  0, 0x87AE,
                               STRING_STRINGDATA_04, 5,  0, 0x87ed,
                               STRING_STRINGDATA_05, 11, 0, 0x875D,
                               STRING_STRINGDATA_06, 5,  0, 0x8786,
                               STRING_STRINGDATA_07, 7,  0, 0x87a3,
                               STRING_STRINGDATA_08, 9,  0, 0x87b6,
                               STRING_STRINGDATA_09, 21, 0, 0x87c3,
                               STRING_STRINGDATA_10, 16, 0, 0x8d57,
                               STRING_STRINGDATA_11, 16, 0, 0x8d6b,
                               STRING_STRINGDATA_12, 22, 0, 0x8d7f,
                               STRING_STRINGDATA_13, 15, 0, 0x8d98,
                               STRING_STRINGDATA_14, 19, 0, 0x8dab,
                               STRING_STRINGDATA_15, 27, 0, 0x8dc2,
                               STRING_STRINGDATA_16, 13, 0, 0x8de1,
                               STRING_STRINGDATA_17, 17, 0, 0x8df2,
                               STRING_STRINGDATA_18, 14, 1, 0x1fa5,
                               STRING_STRINGDATA_19, 13, 1, 0x1fb6,
                               STRING_STRINGDATA_20, 13, 1, 0x1fc6};

static TCHAR ConvertData2Char(BYTE bData)
{
    TCHAR cRet;

    // All ASCII characters will correctly cast to TCHAR.
    switch (bData)
    {
    case 0x00:cRet = '0'; break;
    case 0x01:cRet = '1'; break;
    case 0x02:cRet = '2'; break;
    case 0x03:cRet = '3'; break;
    case 0x04:cRet = '4'; break;
    case 0x05:cRet = '5'; break;
    case 0x06:cRet = '6'; break;
    case 0x07:cRet = '7'; break;
    case 0x08:cRet = '8'; break;
    case 0x09:cRet = '9'; break;
    case 0x0A:cRet = 'A'; break;
    case 0x0B:cRet = 'B'; break;
    case 0x0C:cRet = 'C'; break;
    case 0x0D:cRet = 'D'; break;
    case 0x0E:cRet = 'E'; break;
    case 0x0F:cRet = 'F'; break;
    case 0x10:cRet = 'G'; break;
    case 0x11:cRet = 'H'; break;
    case 0x12:cRet = 'I'; break;
    case 0x13:cRet = 'J'; break;
    case 0x14:cRet = 'K'; break;
    case 0x15:cRet = 'L'; break;
    case 0x16:cRet = 'M'; break;
    case 0x17:cRet = 'N'; break;
    case 0x18:cRet = 'O'; break;
    case 0x19:cRet = 'P'; break;
    case 0x1A:cRet = 'Q'; break;
    case 0x1B:cRet = 'R'; break;
    case 0x1C:cRet = 'S'; break;
    case 0x1D:cRet = 'T'; break;
    case 0x1E:cRet = 'U'; break;
    case 0x1F:cRet = 'V'; break;
    case 0x20:cRet = 'W'; break;
    case 0x21:cRet = 'X'; break;
    case 0x22:cRet = 'Y'; break;
    case 0x23:cRet = 'Z'; break;
    case 0x24:cRet = ' '; break;

        //
    case 0x28:cRet = '-'; break;
    case 0x29:cRet = '*'; break;
    case 0x2B:cRet = '!'; break;
    case 0xCF:cRet = '@'; break;
    case 0xAF:cRet = '.'; break;
    default:cRet = '?'; break;//Space
    }
    return cRet;
}

static BYTE ConvertChr2Data(TCHAR cChar, BOOL *blUnknown)
{
    BYTE bRet;

    *blUnknown = FALSE;
    switch (cChar)
    {
    case '0':bRet = 0x0; break;
    case '1':bRet = 0x1; break;
    case '2':bRet = 0x2; break;
    case '3':bRet = 0x3; break;
    case '4':bRet = 0x4; break;
    case '5':bRet = 0x5; break;
    case '6':bRet = 0x6; break;
    case '7':bRet = 0x7; break;
    case '8':bRet = 0x8; break;
    case '9':bRet = 0x9; break;
    case 'a':bRet = 0xa; break;
    case 'b':bRet = 0xb; break;
    case 'c':bRet = 0xc; break;
    case 'd':bRet = 0xd; break;
    case 'e':bRet = 0xe; break;
    case 'f':bRet = 0xf; break;
    case 'g':bRet = 0x10; break;
    case 'h':bRet = 0x11; break;
    case 'i':bRet = 0x12; break;
    case 'j':bRet = 0x13; break;
    case 'k':bRet = 0x14; break;
    case 'l':bRet = 0x15; break;
    case 'm':bRet = 0x16; break;
    case 'n':bRet = 0x17; break;
    case 'o':bRet = 0x18; break;
    case 'p':bRet = 0x19; break;
    case 'q':bRet = 0x1a; break;
    case 'r':bRet = 0x1b; break;
    case 's':bRet = 0x1c; break;
    case 't':bRet = 0x1d; break;
    case 'u':bRet = 0x1e; break;
    case 'v':bRet = 0x1f; break;
    case 'w':bRet = 0x20; break;
    case 'x':bRet = 0x21; break;
    case 'y':bRet = 0x22; break;
    case 'z':bRet = 0x23; break;
    case ' ':bRet = 0x24; break;

        //
    case '-':bRet = 0x28; break;
    case '*':bRet = 0x29; break;
    case '!':bRet = 0x2B; break;
    case '@':bRet = 0xCF; break;
    case '.':bRet = 0xAF; break;
    default:
    {
        bRet = 0x24;
        *blUnknown = TRUE;
    }
    break;//Space
    }
    return bRet;
}

static int GetNumStrings()
{
    return sizeof(smbStringData) / sizeof(SMBSTRINGINFO);
}

static void ChangeString(UINT iStringNum, LPTSTR pString)
{
    BYTE *pbTmp;
    BYTE bTmp;
    BOOL blEOB = FALSE;
    BOOL blUnknown = FALSE;
    int i, n;

    CharLower(pString);//small char
    pbTmp = (smbStringData[iStringNum].bRomType) ? bCHRROM : bPRGROM;
    pbTmp += smbStringData[iStringNum].wOffset;
    for (i = 0, n = 0; i < smbStringData[iStringNum].bMaxLen; i++)
    {
        if (*(pString + n) == '\0')
            blEOB = TRUE;
        bTmp = 0x24;// space
        if (!blEOB)
        {
            bTmp = ConvertChr2Data(*(pString + n), &blUnknown);
            if (blUnknown)
            {
                TCHAR cTmp[4] = {0};
                memcpy(cTmp, &pString[n], 3);
                sscanf(cTmp, __T("#%hhx"), &bTmp);
                n += 3;
            }
            else
                n++;
        }
        pbTmp[i] = bTmp;
    }
}

static void GetString(UINT iStringNum, LPTSTR pString, UINT iBufSize)
{
    UINT i, n;
    BYTE *pbTmp;
    TCHAR tcTmp;

    memset(pString, 0, iBufSize);
    pbTmp = (smbStringData[iStringNum].bRomType) ? bCHRROM : bPRGROM;
    pbTmp += smbStringData[iStringNum].wOffset;
    for (i = 0, n = 0; (i < smbStringData[iStringNum].bMaxLen) && (n < iBufSize); i++)
    {
        tcTmp = ConvertData2Char(pbTmp[i]);
        if (tcTmp == '?')
        {
            sprintf(&pString[n], __T("#%.2X"), pbTmp[i]);
            n += 3;
        }
        else
            pString[n++] = tcTmp;
    }
}

static int GetStringMaxChars(UINT iStringNum)
{
    return smbStringData[iStringNum].bMaxLen;
}

LRESULT CALLBACK StringEditDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL sblWritten;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        int i;
        TCHAR cBuf[SMB_STRING_MAXCHARS + 1];

        //
        sblWritten = FALSE;

        memset(cBuf, 0, SMB_STRING_MAXCHARS + 1);
        GetString(0, cBuf, SMB_STRING_MAXCHARS);
        SetDlgItemText(hDlg, IDC_STRING, cBuf);
        for (i = 0; i < GetNumStrings(); i++)
            SendDlgItemMessage(hDlg, IDC_STRINGSELECT, CB_ADDSTRING, 0, (LPARAM)smbStringData[i].pName);

        SendDlgItemMessage(hDlg, IDC_STRINGSELECT, CB_SETCURSEL, 0, 0);

        SetDlgItemInt(hDlg, IDC_LEN, GetStringMaxChars(0), FALSE);
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            //			   case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return TRUE;
        case IDC_STRINGSELECT:
        {
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                int iSel = 0, iMaxLen;
                TCHAR cBuf[SMB_STRING_MAXCHARS + 1];

                memset(cBuf, 0, SMB_STRING_MAXCHARS + 1);
                iSel = SendDlgItemMessage(hDlg, IDC_STRINGSELECT, CB_GETCURSEL, 0, 0);
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
        if (LOWORD(wParam) == IDOK)//IDC_STRINGWRITE)
        {
            int iSel = 0;
            TCHAR cBuf[SMB_STRING_MAXCHARS + 1];

            memset(cBuf, 0, SMB_STRING_MAXCHARS + 1);

            iSel = SendDlgItemMessage(hDlg, IDC_STRINGSELECT, CB_GETCURSEL, 0, 0);
            if (iSel == CB_ERR) return TRUE;
            GetDlgItemText(hDlg, IDC_STRING, cBuf, SMB_STRING_MAXCHARS);
            if (!sblWritten)
            {
                undoPrepare(UNDONAME_TOOLSTR);
                sblWritten = TRUE;
            }
            ChangeString(iSel, cBuf);

            //					   gblDataChanged=TRUE;
            fr_SetDataChanged(TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

/********************

  ループコマンド

*********************/

LRESULT CALLBACK LoopEditDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        BYTE *bTmp;
        TCHAR cBuf[34];

        bTmp = bPRGROM + SMBADDRESS_LOOP_WORLD;
        sprintf(cBuf, __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"), bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6], bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
        SetDlgItemText(hDlg, IDC_WORLD, cBuf);
        bTmp = bPRGROM + SMBADDRESS_LOOP_PAGE;
        sprintf(cBuf, __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"), bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6], bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
        SetDlgItemText(hDlg, IDC_PAGE, cBuf);
        bTmp = bPRGROM + SMBADDRESS_LOOP_YPOS;
        sprintf(cBuf, __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"), bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6], bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
        SetDlgItemText(hDlg, IDC_YPOS, cBuf);
        bTmp = bPRGROM + SMBADDRESS_LOOP_RETURNPOS;
        sprintf(cBuf, __T("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x"), bTmp[0], bTmp[1], bTmp[2], bTmp[3], bTmp[4], bTmp[5], bTmp[6], bTmp[7], bTmp[8], bTmp[9], bTmp[10]);
        SetDlgItemText(hDlg, IDC_RETURNPOS, cBuf);

        sprintf(cBuf, __T("%.2x"), bPRGROM[SMBADDRESS_LOOP_W7DATA1]);
        SetDlgItemText(hDlg, IDC_DATA1, cBuf);
        sprintf(cBuf, __T("%.2x"), bPRGROM[SMBADDRESS_LOOP_W7DATA2]);
        SetDlgItemText(hDlg, IDC_DATA2, cBuf);
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            BYTE bTmp[4][11] = {0};
            BYTE bData[2] = {0};
            TCHAR cBuf[34];

            memset(cBuf, 0, 34);
            GetDlgItemText(hDlg, IDC_WORLD, cBuf, 34);
            if (11 != sscanf(cBuf, __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx"), &bTmp[0][0], &bTmp[0][1], &bTmp[0][2], &bTmp[0][3], &bTmp[0][4], &bTmp[0][5], &bTmp[0][6], &bTmp[0][7], &bTmp[0][8], &bTmp[0][9], &bTmp[0][10])) return TRUE;

            memset(cBuf, 0, 34);
            GetDlgItemText(hDlg, IDC_PAGE, cBuf, 34);
            if (11 != sscanf(cBuf, __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx"), &bTmp[1][0], &bTmp[1][1], &bTmp[1][2], &bTmp[1][3], &bTmp[1][4], &bTmp[1][5], &bTmp[1][6], &bTmp[1][7], &bTmp[1][8], &bTmp[1][9], &bTmp[1][10])) return TRUE;

            memset(cBuf, 0, 34);
            GetDlgItemText(hDlg, IDC_YPOS, cBuf, 34);
            if (11 != sscanf(cBuf, __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx"), &bTmp[2][0], &bTmp[2][1], &bTmp[2][2], &bTmp[2][3], &bTmp[2][4], &bTmp[2][5], &bTmp[2][6], &bTmp[2][7], &bTmp[2][8], &bTmp[2][9], &bTmp[2][10])) return TRUE;

            memset(cBuf, 0, 34);
            GetDlgItemText(hDlg, IDC_RETURNPOS, cBuf, 34);
            if (11 != sscanf(cBuf, __T("%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx"), &bTmp[3][0], &bTmp[3][1], &bTmp[3][2], &bTmp[3][3], &bTmp[3][4], &bTmp[3][5], &bTmp[3][6], &bTmp[3][7], &bTmp[3][8], &bTmp[3][9], &bTmp[3][10])) return TRUE;

            memset(cBuf, 0, 34);
            GetDlgItemText(hDlg, IDC_DATA1, cBuf, 3);
            if (1 != sscanf(cBuf, __T("%hhx"), &bData[0]))return TRUE;

            memset(cBuf, 0, 34);
            GetDlgItemText(hDlg, IDC_DATA2, cBuf, 3);
            if (1 != sscanf(cBuf, __T("%hhx"), &bData[1])) return TRUE;

            undoPrepare(UNDONAME_TOOLLOOPBIN);

            //					   gblDataChanged=TRUE;
            fr_SetDataChanged(TRUE);

            memcpy(bPRGROM + SMBADDRESS_LOOP_WORLD, bTmp[0], 11);
            memcpy(bPRGROM + SMBADDRESS_LOOP_PAGE, bTmp[1], 11);
            memcpy(bPRGROM + SMBADDRESS_LOOP_YPOS, bTmp[2], 11);
            memcpy(bPRGROM + SMBADDRESS_LOOP_RETURNPOS, bTmp[3], 11);
            bPRGROM[SMBADDRESS_LOOP_W7DATA1] = bData[0];
            bPRGROM[SMBADDRESS_LOOP_W7DATA2] = bData[1];
        }
        case IDCANCEL:
        {
            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        break;
        }
    }

    return FALSE;
}

/*******************

  ゲーム全般の設定

********************/

typedef struct
{
    LPTSTR Name;
    BYTE bGfxData[4];
}POLEGFXDATAINFO;

POLEGFXDATAINFO PoleGfxInfo[] = {
    STRING_POLEGFX_DEFAULT, "\x24\x2F\x24\x3D",
    STRING_POLEGFX_ROPE,    "\xA2\xA2\xA3\xA3",
    STRING_POLEGFX_TREE,    "\xBE\xBE\xBF\xBF",
    STRING_POLEGFX_OTHER,   "\xFF\xFF\xFF\xFF"};

int GetPoleGfxDatas()
{
    return (sizeof(PoleGfxInfo) / sizeof(POLEGFXDATAINFO));
}

LRESULT CALLBACK GameSettingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        TCHAR cBuf[20];
        BYTE bNewFlower[] = "\xEA\xEA\xEA\xEA\xEA";
        int n, i;

        //残りマリオの設定
        SendDlgItemMessage(hDlg, IDC_MARIOLEFTSPIN, UDM_SETRANGE, 0, MAKEWPARAM(128, 1));
        SetDlgItemInt(hDlg, IDC_MARIOLEFT, bPRGROM[SMB_MARIO_LEFT] + 1, FALSE);

        //パックンフラワー
        if (!memcmp(bPRGROM + SMB_FLOWER, bNewFlower, 5))
            CheckDlgButton(hDlg, IDC_FLOWER, BST_CHECKED);

        //ポールのグラフィックス
        n = GetPoleGfxDatas() - 1;
        for (i = 0; i < GetPoleGfxDatas(); i++)
        {
            SendDlgItemMessage(hDlg, IDC_POLEGFX, CB_ADDSTRING, 0, (LPARAM)PoleGfxInfo[i].Name);
            if (!memcmp(bPRGROM + SMB_POLEGFX, PoleGfxInfo[i].bGfxData, 4)) n = i;
        }
        SendDlgItemMessage(hDlg, IDC_POLEGFX, CB_SETCURSEL, n, 0);

        //
        for (i = 0; i < 10; i++)
        {
            sprintf(cBuf, __T("%d"), i * 100);
            SendDlgItemMessage(hDlg, IDC_TIME400, CB_ADDSTRING, 0, (LPARAM)cBuf);
            SendDlgItemMessage(hDlg, IDC_TIME300, CB_ADDSTRING, 0, (LPARAM)cBuf);
            SendDlgItemMessage(hDlg, IDC_TIME200, CB_ADDSTRING, 0, (LPARAM)cBuf);
        }

        sprintf(cBuf, STRING_UNKNOWN);
        i = bPRGROM[SMB_TIME];
        if (i > 9)
        {
            SendDlgItemMessage(hDlg, IDC_TIME400, CB_ADDSTRING, 0, (LPARAM)cBuf);
            i = 10;
        }
        SendDlgItemMessage(hDlg, IDC_TIME400, CB_SETCURSEL, i, 0);
        i = bPRGROM[SMB_TIME + 1];
        if (i > 9)
        {
            SendDlgItemMessage(hDlg, IDC_TIME300, CB_ADDSTRING, 0, (LPARAM)cBuf);
            i = 10;
        }
        SendDlgItemMessage(hDlg, IDC_TIME300, CB_SETCURSEL, i, 0);
        i = bPRGROM[SMB_TIME + 2];
        if (i > 9)
        {
            SendDlgItemMessage(hDlg, IDC_TIME200, CB_ADDSTRING, 0, (LPARAM)cBuf);
            i = 10;
        }
        SendDlgItemMessage(hDlg, IDC_TIME200, CB_SETCURSEL, i, 0);
        return TRUE;
    }
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        switch (pnmh->code)
        {
        case PSN_APPLY:
        {
            BOOL blSuccess;
            int iRet;
            int iPoleGfx;
            BYTE bFlower[] = "\x0D\x5F\x07\xF0\x2B";
            BYTE bNewFlower[] = "\xEA\xEA\xEA\xEA\xEA";

            if (!g_blOK)
            {
                undoPrepare(UNDONAME_TOOLOTHER);
                g_blOK = TRUE;
            }

            //残りマリオ
            iRet = GetDlgItemInt(hDlg, IDC_MARIOLEFT, &blSuccess, FALSE) - 1;
            if (!blSuccess || (iRet < 0 || iRet>127))
            {
                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                return TRUE;
            }
            bPRGROM[SMB_MARIO_LEFT] = iRet;

            //パックンフラワー
            if (BST_CHECKED&IsDlgButtonChecked(hDlg, IDC_FLOWER))
                memset(bPRGROM + SMB_FLOWER, 0xEA, 5);
            else if (!memcmp(bPRGROM + SMB_FLOWER, bNewFlower, 5))
                memcpy(bPRGROM + SMB_FLOWER, bFlower, 5);

            //ポールのグラフィックス
            iPoleGfx = SendDlgItemMessage(hDlg, IDC_POLEGFX, CB_GETCURSEL, 0, 0);
            if (iPoleGfx == CB_ERR)return TRUE;
            if (iPoleGfx != GetPoleGfxDatas() - 1)//その他でなければ…
            {
                memcpy(bPRGROM + SMB_POLEGFX, PoleGfxInfo[iPoleGfx].bGfxData, 4);
            }

            //
            iRet = SendDlgItemMessage(hDlg, IDC_TIME400, CB_GETCURSEL, 0, 0);
            if (iRet != 10 && iRet != CB_ERR) bPRGROM[SMB_TIME] = iRet;
            iRet = SendDlgItemMessage(hDlg, IDC_TIME300, CB_GETCURSEL, 0, 0);
            if (iRet != 10 && iRet != CB_ERR) bPRGROM[SMB_TIME + 1] = iRet;
            iRet = SendDlgItemMessage(hDlg, IDC_TIME200, CB_GETCURSEL, 0, 0);
            if (iRet != 10 && iRet != CB_ERR) bPRGROM[SMB_TIME + 2] = iRet;
            return TRUE;
        }
        break;
        }
    }
    break;
    }
    return FALSE;
}

//

LRESULT CALLBACK GameSetting1upDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BYTE lpbBuf[SMB_NUM_WORLDS];
    static int iCurSel;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        int n;
        TCHAR cBuf[20];
        ADDRESSDATA ad1up;

        //lpbBuf=Malloc(GetNumWorlds());
        if (lpbBuf)
        {
            ADDRESSDATA_LOAD(ad1up, SMB_COINSFOR1UP_ADDRESS);

            //１upｷﾉｺのためのｺｲﾝの枚数
            memcpy(lpbBuf, bPRGROM + ADDRESSDATA_GET(ad1up), GetNumWorlds());

            for (n = 0; n < GetNumWorlds(); n++)
            {
                wsprintf(cBuf, STRING_SETTING_WORLDX, n + 1);
                SendDlgItemMessage(hDlg, IDC_WORLD, CB_ADDSTRING, 0, (LPARAM)cBuf);
            }
            SendDlgItemMessage(hDlg, IDC_WORLD, CB_SETCURSEL, 0, 0);
            iCurSel = 0;

            //
            SendDlgItemMessage(hDlg, IDC_COINSFOR1UPSPIN, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
            SetDlgItemInt(hDlg, IDC_COINSFOR1UP, lpbBuf[iCurSel], FALSE);
        }
        return TRUE;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_WORLD:
        {
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                int iRet;
                iRet = SendDlgItemMessage(hDlg, IDC_WORLD, CB_GETCURSEL, 0, 0);
                if (iRet != CB_ERR)
                {
                    iCurSel = iRet;
                    if (lpbBuf)
                        SetDlgItemInt(hDlg, IDC_COINSFOR1UP, lpbBuf[iRet], FALSE);
                }
            }
        }
        break;
        case IDC_COINSFOR1UP:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                BOOL blSuccess;
                int iTmp;

                iTmp = GetDlgItemInt(hDlg, IDC_COINSFOR1UP, &blSuccess, FALSE);
                if (lpbBuf && iCurSel < GetNumWorlds() && blSuccess)
                    lpbBuf[iCurSel] = (BYTE)iTmp;
            }
        }
        break;
        }
    }
    break;
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        switch (pnmh->code)
        {
        case PSN_APPLY:
        {
            ADDRESSDATA ad1up;
            if (lpbBuf)
            {
                if (!g_blOK)
                {
                    undoPrepare(UNDONAME_TOOLOTHER);
                    g_blOK = TRUE;
                }
                ADDRESSDATA_LOAD(ad1up, SMB_COINSFOR1UP_ADDRESS);
                memcpy(bPRGROM + ADDRESSDATA_GET(ad1up), lpbBuf, GetNumWorlds());

                //Mfree(lpbBuf);
                //lpbBuf=NULL;
            }

            return TRUE;
        }
        break;
        }
    }
    break;
    }
    return FALSE;
}

LRESULT CALLBACK GameSettingWarpZoneDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        //ワープゾーン
        SendDlgItemMessage(hDlg, IDC_WARPASPIN1, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPASPIN2, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPASPIN3, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPBSPIN1, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPBSPIN2, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPBSPIN3, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPCSPIN1, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPCSPIN2, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));
        SendDlgItemMessage(hDlg, IDC_WARPCSPIN3, UDM_SETRANGE, 0, MAKEWPARAM(255, 0));

        SetDlgItemInt(hDlg, IDC_WARPA1, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPA2, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 1], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPA3, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 2], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPB1, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 4], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPB2, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 5], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPB3, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 6], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPC1, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 8], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPC2, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 9], FALSE);
        SetDlgItemInt(hDlg, IDC_WARPC3, bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + 10], FALSE);

        //プロパテイシートを中央に持ってくる
        CenterPropatySheet(hDlg);

        return TRUE;
    }
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        switch (pnmh->code)
        {
        case PSN_APPLY:
        {
            BOOL blSuccess;
            int iRet;
            int n;
            int iAddrDelta[] = {0,1,2,4,5,6,8,9,10};
            int iCtrlID[] = {IDC_WARPA1,IDC_WARPA2,IDC_WARPA3,IDC_WARPB1,IDC_WARPB2,IDC_WARPB3,IDC_WARPC1,IDC_WARPC2,IDC_WARPC3};

            if (!g_blOK)
            {
                undoPrepare(UNDONAME_TOOLOTHER);
                g_blOK = TRUE;
            }

            for (n = 0; n < 9; n++)
            {
                iRet = GetDlgItemInt(hDlg, iCtrlID[n], &blSuccess, FALSE);
                if (!blSuccess || (iRet < 0 || iRet>255))
                {
                    SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                    return TRUE;
                }
                bPRGROM[SMB_WARPZONE_WORLD_ADDRESS + iAddrDelta[n]] = iRet;
            }
            return TRUE;
        }
        break;
        case PSN_RESET:
        {
        }
        break;
        }
    }
    break;
    }
    return FALSE;
}

LRESULT CALLBACK GameSettingKoopaDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BYTE lpbBuf[SMB_NUM_WORLDS];
    static int iCurSel;

    switch (message)
    {
    case WM_INITDIALOG:
    {
        TCHAR cBuf[50];
        int n;
        ADDRESSDATA adKoopa;
        extern struct
        {
            LPTSTR Name; int YDelta; BYTE bFixedYPos; int XDelta;
        }smbBudGuysInfo[];

        ADDRESSDATA_LOAD(adKoopa, SMB_KOOPAREALCHARCTER_ADDRESS);

        //lpbBuf = Malloc(GetNumWorlds());
        if (lpbBuf)
        {
            //１upｷﾉｺのためのｺｲﾝの枚数
            memcpy(lpbBuf, bPRGROM + ADDRESSDATA_GET(adKoopa), GetNumWorlds());

            for (n = 0; n < GetNumWorlds(); n++)
            {
                wsprintf(cBuf, STRING_SETTING_WORLDX, n + 1);
                SendDlgItemMessage(hDlg, IDC_KOOPAWORLD, CB_ADDSTRING, 0, (LPARAM)cBuf);
            }
            SendDlgItemMessage(hDlg, IDC_KOOPAWORLD, CB_SETCURSEL, 0, 0);
            iCurSel = 0;

            //
            for (n = 0; n < 0x40; n++)
            {
                SendDlgItemMessage(hDlg, IDC_KOOPA, CB_ADDSTRING, 0, (LPARAM)smbBudGuysInfo[n].Name);
            }
            SendDlgItemMessage(hDlg, IDC_KOOPA, CB_SETCURSEL, lpbBuf[iCurSel], 0);
        }

        //
        SendDlgItemMessage(hDlg, IDC_WORLDSPIN, UDM_SETRANGE, 0, MAKEWPARAM(256, 1));
        SetDlgItemInt(hDlg, IDC_WORLD, bPRGROM[SMB_KOOPAHAMMER] + 1, FALSE);

        return TRUE;
    }
    break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_KOOPAWORLD:
        {
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                int iRet;

                iRet = SendDlgItemMessage(hDlg, IDC_KOOPAWORLD, CB_GETCURSEL, 0, 0);
                if (iRet != CB_ERR)
                {
                    iCurSel = iRet;
                    if (lpbBuf)
                        SendDlgItemMessage(hDlg, IDC_KOOPA, CB_SETCURSEL, lpbBuf[iRet], 0);
                }
            }
        }
        break;
        case IDC_KOOPA:
        {
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                int iRet;
                iRet = SendDlgItemMessage(hDlg, IDC_KOOPA, CB_GETCURSEL, 0, 0);
                if (lpbBuf && iCurSel < GetNumWorlds() && iRet != CB_ERR)
                    lpbBuf[iCurSel] = (BYTE)iRet;
            }
        }
        break;
        }
    }
    break;
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        switch (pnmh->code)
        {
        case PSN_APPLY:
        {
            ADDRESSDATA adKoopa;
            BOOL blSuccess;
            int iRet;

            if (!g_blOK)
            {
                undoPrepare(UNDONAME_TOOLOTHER);
                g_blOK = TRUE;
            }

            iRet = GetDlgItemInt(hDlg, IDC_WORLD, &blSuccess, FALSE);
            if (!blSuccess || iRet < 1 || iRet>256)
            {
                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                return TRUE;
            }

            bPRGROM[SMB_KOOPAHAMMER] = (BYTE)(iRet - 1);

            if (lpbBuf)
            {
                ADDRESSDATA_LOAD(adKoopa, SMB_KOOPAREALCHARCTER_ADDRESS);
                memcpy(bPRGROM + ADDRESSDATA_GET(adKoopa), lpbBuf, GetNumWorlds());

                //Mfree(lpbBuf);
                //lpbBuf = NULL;
            }

            return TRUE;
        }
        break;
        }
    }
    break;
    }
    return FALSE;
}

LRESULT CALLBACK GameSettingWorldDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        SendDlgItemMessage(hDlg, IDC_DIFFICULTYWORLDSPIN, UDM_SETRANGE, 0, MAKEWPARAM(256, 1));
        SetDlgItemInt(hDlg, IDC_DIFFICULTYWORLD, bPRGROM[SMB_DIFFICULTYWORLD] + 1, FALSE);
        SendDlgItemMessage(hDlg, IDC_DIFFICULTYAREASPIN, UDM_SETRANGE, 0, MAKEWPARAM(256, 1));
        SetDlgItemInt(hDlg, IDC_DIFFICULTYAREA, bPRGROM[SMB_DIFFICULTYAREA] + 1, FALSE);
        SendDlgItemMessage(hDlg, IDC_SEABLOCKWORLDSPIN, UDM_SETRANGE, 0, MAKEWPARAM(256, 1));
        SetDlgItemInt(hDlg, IDC_SEABLOCKWORLD, bPRGROM[SMB_SEABLOCKWORLD] + 1, FALSE);

        return TRUE;
    }
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        switch (pnmh->code)
        {
        case PSN_APPLY:
        {
            BOOL blSuccess;
            int iRet;

            if (!g_blOK)
            {
                undoPrepare(UNDONAME_TOOLOTHER);
                g_blOK = TRUE;
            }

            iRet = GetDlgItemInt(hDlg, IDC_DIFFICULTYWORLD, &blSuccess, FALSE) - 1;
            if (!blSuccess || iRet < 0 || iRet>255)
            {
                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                return TRUE;
            }
            bPRGROM[SMB_DIFFICULTYWORLD] = (BYTE)iRet;

            iRet = GetDlgItemInt(hDlg, IDC_DIFFICULTYAREA, &blSuccess, FALSE) - 1;
            if (!blSuccess || iRet < 0 || iRet>255)
            {
                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                return TRUE;
            }
            bPRGROM[SMB_DIFFICULTYAREA] = (BYTE)iRet;

            iRet = GetDlgItemInt(hDlg, IDC_SEABLOCKWORLD, &blSuccess, FALSE) - 1;
            if (!blSuccess || iRet < 0 || iRet>255)
            {
                SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
                return TRUE;
            }
            bPRGROM[SMB_SEABLOCKWORLD] = (BYTE)iRet;

            return TRUE;
        }
        break;
        }
    }
    break;
    }
    return FALSE;
}

void GameSettingPropertySheet(HWND hwndOwner)
{
    //TODO
#define OPTPS_NUM_PAGES 5
    LPTSTR lpTitle[OPTPS_NUM_PAGES] = {STRING_SETTING_WARPZONE, STRING_SETTING_1UP, STRING_SETTING_KOOPA, STRING_SETTING_WORLD, STRING_SETTING_OTHER};
    LPTSTR lpDlgResName[OPTPS_NUM_PAGES] = {__T("GAMESETTINGWARPZONEDLG"),__T("GAMESETTING1UPDLG"),__T("GAMESETTINGKOOPADLG"),__T("GAMESETTINGWORLDDLG"),__T("GAMESETTINGDLG")};
    DLGPROC pfnDlgProc[OPTPS_NUM_PAGES] = {GameSettingWarpZoneDlgProc,GameSetting1upDlgProc,GameSettingKoopaDlgProc,GameSettingWorldDlgProc,GameSettingDlgProc};

    //Local
    PROPSHEETPAGE psp[OPTPS_NUM_PAGES];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < OPTPS_NUM_PAGES; i++)
    {
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

    if (g_blOK)
    {
        //		gblDataChanged=TRUE;
        fr_SetDataChanged(TRUE);
        UpdateObjectView(0);

        //	UpdateObjectList(0);
    }

    return;
}
