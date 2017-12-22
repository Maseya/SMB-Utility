/************************************************************************************

                                  smb Utility

  File: roomseldlg.h
  Description:
  History:

 ************************************************************************************/
#ifndef ROOMSELDLG_H
#define ROOMSELDLG_H

 //struct and function
typedef struct _tagROOMSELECT
{
    LPTSTR lpszTitle;
    BOOL blDoInit;
    BYTE bInitRoomID;
    UINT uInitPage;
    BYTE bNewRoomID;
    UINT uNewPage;
}ROOMSELECT, FAR * LPROOMSELECT;

INT_PTR RoomSelectDialogBox(HWND hWnd, LPROOMSELECT lpRoomSelect);

#endif
