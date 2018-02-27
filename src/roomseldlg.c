/**********************************************************************

                                  smb Utility

  File: roomseldlg.c
  Description:
  History:

 *********************************************************************/
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
    TCHAR cBuf[10];
    HWND hPVWnd;
    HDC hPVdc;
    RECT rcPV;

    if (!blGetRoomIDFromList)
    {
        GetDlgItemText(hDlg, IDC_DATA, cBuf, 10);
        if (1 != _stscanf(cBuf, __T("%hhx"), &bRoomID)) return;
    }
    else
    {
        LRESULT iSel;
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
        TCHAR cBuf[5];
        LPROOMSELECT lpRoomSelect = (LPROOMSELECT)lParam;

        SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)lpRoomSelect);

        GetValidRoomIDs(bRoomIDs);
        for (n = 0; n < SMB_NUM_ADDRESSDATA; n++)
        {
            _stprintf(cBuf, __T("%.2x"), bRoomIDs[n]);
            SendDlgItemMessage(hDlg, IDC_DATA, CB_ADDSTRING, 0, (LPARAM)cBuf);
        }

        if (lpRoomSelect->blDoInit)
        {
            _stprintf(cBuf, __T("%.2x"), lpRoomSelect->bInitRoomID & 0x7F);
            SetDlgItemText(hDlg, IDC_DATA, cBuf);

            _stprintf(cBuf, __T("%d"), lpRoomSelect->uInitPage);
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
            TCHAR cBuf[20];
            BOOL blSuccess;
            BYTE bRoomID;
            UINT iPage;
            LPROOMSELECT lpRoomSelect;

            GetDlgItemText(hDlg, IDC_DATA, cBuf, 20);
            if (1 != _stscanf(cBuf, __T("%hhx"), &bRoomID)) return TRUE;
            if (!IsRoomIDValid(bRoomID)) return TRUE;
            iPage = GetDlgItemInt(hDlg, IDC_PAGEEDIT2, &blSuccess, FALSE);
            if (!blSuccess) return TRUE;

            lpRoomSelect = (LPROOMSELECT)GetWindowLongPtr(hDlg, GWLP_USERDATA);
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

INT_PTR RoomSelectDialogBox(HWND hWnd, LPROOMSELECT lpRoomSelect)
{
    INT_PTR blRet;

    if (!lpRoomSelect) return 0;

    blRet = DialogBoxParam(GetModuleHandle(NULL), __T("SENDOBJECTDLG"), hWnd, RoomSelectDlgProc, (LPARAM)lpRoomSelect);

    return blRet;
}
