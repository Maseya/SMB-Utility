/************************************************************************************

                                  smb Utility

  File: objview.h
  Description:
  History:

 ************************************************************************************/
#ifndef OBJVIEW_H
#define OBJVIEW_H

BOOL RegisterMapViewWndClass(HINSTANCE hInstance);
HWND CreateMapViewWnd(HINSTANCE hInstance,HWND hWndMDIClient);
void ClearObjectViewBackBuffer();
void UpdateObjectView(DWORD dwUpdateFlag);
void SetMapViewCursoleBadGuys(BYTE *pbBuf,int iPage);
void SetMapViewCursoleMap(BYTE *pbBuf,int iPage);
BOOL UpdateObjectViewCursole();
void EnsureMapViewCursoleVisible();
void SetObjectViewCursole(int iPage);
void InitMapViewGlobalValue();
LRESULT CALLBACK ObjectViewOptionDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

#define OBJVIEWSETTING_DEFAULT          0x00000000
#define OBJVIEWSETTING_NOTDRAWASSISTBMP 1
#define OBJVIEWSETTING_DRAWXPOSLINE     2
#define OBJVIEWSETTING_DRAWYPOSLINE     4
//#define OBJVIEWSETTING_NOPOPUP          8

DWORD ObjectViewGetSetting();
void  ObjectViewSetSetting(DWORD);
#endif /* OBJVIEW_H */