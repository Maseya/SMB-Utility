﻿/**********************************************************************

                                  smb Utility

  File: objlist.h
  Description:
  History:

 *********************************************************************/
#ifndef OBJLIST_H
#define OBJLIST_H

#include <Windows.h>

/********************

  マップエディト関数

  Map edit function

*********************/
void InitMapEditGlobalValue();

LRESULT CALLBACK MapComEditDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                   LPARAM lParam);
LRESULT CALLBACK BadGuysComEditDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                       LPARAM lParam);
LRESULT CALLBACK MapComHeadEditDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                       LPARAM lParam);

void FormatMapString(LPBYTE lpbBuf, LPTSTR lpszBuf);
void FormatBadGuysString(LPBYTE lpbBuf, LPTSTR lpszBuf);

void ObjectListSetCursor(int iIndex);
void ObjectListClear();
void ObjectListShowCursor();
void UpdateObjectList(DWORD dwUpdateFlag);

HWND CreateMapEditWnd(HINSTANCE hInstance, HWND hWndMDIClient);
BOOL RegisterMapEditWndClass(HINSTANCE hInstance);

int GetHalfPointPage(int w, int a);

#endif /* OBJLIST_H */
