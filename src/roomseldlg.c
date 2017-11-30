/************************************************************************************

                                  smb Utility

  File: roomseldlg.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "emuutil.h"
#include "roommng.h"
#include "objlib.h"
#include "roomseldlg.h"

void UpdatePreview(HWND hDlg, BOOL blGetRoomIDFromList)
{
    BYTE bRoomID;
    int iPage;
    BOOL blSuccess;
    char cBuf[10];
    HWND hPVWnd;
    HDC hPVdc;
    RECT rcPV;

    if (!blGetRoomIDFromList)
    {
        GetDlgItemText(hDlg, IDC_DATA, cBuf, 10);
        if (1 != sscanf(cBuf, "%x", &bRoomID)) return;
    }
    else
    {
        int iSel;
        BYTE bID[SMB_NUM_ADDRESSDATA];

        GetValidRoomIDs(bID);
        iSel = SendDlgItemMessage(hDlg, IDC_DATA, CB_GETCURSEL, 0, 0);
        if (iSel == CB_ERR) return;
        bRoomID = bID[iSel];
    }

    if (!IsRoomIDValid(bRoomID)) return;

    iPage = GetDlgItemInt(hDlg, IDC_PAGEEDIT2, &blSuccess, FALSE);
    if (!blSuccess) return;

    hPVWnd = GetDlgItem(hDlg, IDC_VIEW);
    GetClientRect(hPVWnd, &rcPV);
    hPVdc = GetDC(hPVWnd);
    if (hPVdc)
    {
        if (RunEmulatorViewPage(bRoomID, iPage))
            TransferFromEmuBackBuffer(hPVdc, 0, 0, rcPV.right - rcPV.left, rcPV.bottom - rcPV.top, TRUE);
        else
            FillRect(hPVdc, &rcPV, GetSysColorBrush(COLOR_3DFACE));
        ReleaseDC(hPVWnd, hPVdc);
    }
}

LRESULT CALLBACK RoomSelectDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        BYTE bRoomIDs[SMB_NUM_ADDRESSDATA];
        int n;
        char cBuf[5];
        LPROOMSELECT lpRoomSelect = (LPROOMSELECT)lParam;

        //
        SetWindowLong(hDlg, GWL_USERDATA, (LONG)lpRoomSelect);

        GetValidRoomIDs(bRoomIDs);
        for (n = 0; n < SMB_NUM_ADDRESSDATA; n++)
        {
            sprintf(cBuf, "%.2x", bRoomIDs[n]);
            SendDlgItemMessage(hDlg, IDC_DATA, CB_ADDSTRING, 0, (LPARAM)cBuf);
        }

        if (lpRoomSelect->blDoInit)
        {
            //
            sprintf(cBuf, "%.2x", lpRoomSelect->bInitRoomID & 0x7F);
            SetDlgItemText(hDlg, IDC_DATA, cBuf);

            //
            sprintf(cBuf, "%d", lpRoomSelect->uInitPage);
            SetDlgItemText(hDlg, IDC_PAGEEDIT2, cBuf);
        }

        if (lpRoomSelect->lpszTitle)
        {
            SetWindowText(hDlg, lpRoomSelect->lpszTitle);
        }
    }
    break;
    case WM_PAINT:
        UpdatePreview(hDlg, FALSE);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            char cBuf[20];
            BOOL blSuccess;
            BYTE bRoomID;
            UINT iPage;
            LPROOMSELECT lpRoomSelect;

            GetDlgItemText(hDlg, IDC_DATA, cBuf, 20);
            if (1 != sscanf(cBuf, "%x", &bRoomID)) return TRUE;
            if (!IsRoomIDValid(bRoomID)) return TRUE;
            iPage = GetDlgItemInt(hDlg, IDC_PAGEEDIT2, &blSuccess, FALSE);
            if (!blSuccess) return TRUE;

            lpRoomSelect = (LPROOMSELECT)GetWindowLong(hDlg, GWL_USERDATA);
            lpRoomSelect->bNewRoomID = bRoomID;
            lpRoomSelect->uNewPage = (UINT)iPage;

            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        case IDCANCEL:
        {
            EndDialog(hDlg, FALSE);
            return TRUE;
        }
        case IDC_DATA:
            if (HIWORD(wParam) == CBN_EDITCHANGE)
                UpdatePreview(hDlg, FALSE);
            else if (HIWORD(wParam) == CBN_SELCHANGE)
                UpdatePreview(hDlg, TRUE);
            return TRUE;
        case IDC_PAGEEDIT2:
            if (HIWORD(wParam) == EN_CHANGE)
            {
                UpdatePreview(hDlg, FALSE);
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL RoomSelectDialogBox(HWND hWnd, LPROOMSELECT lpRoomSelect)
{
    BOOL blRet;

    if (!lpRoomSelect) return FALSE;

    blRet = DialogBoxParam(GetModuleHandle(NULL), "SENDOBJECTDLG", hWnd, RoomSelectDlgProc, (LPARAM)lpRoomSelect);

    return blRet;
}
