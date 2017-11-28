/************************************************************************************

                                  smb Utility

  File: frame.h
  Description:
  History:

 ************************************************************************************/
#ifndef FRAME_H
#define FRAME_H

int Msg(LPCTSTR lpText,UINT uType);
void SetStatusBarText(LPTSTR lpText);
void RefreshWindowTitle(BOOL);
void UpdateStatusBarRoomInfoText(HWND hSbWnd);
void CenterPropatySheet(HWND hDlg);
HWND fr_GetStatusBar();
BOOL fr_SetDataChanged(BOOL);
BOOL fr_GetDataChanged();

#endif /* FRAME_H */