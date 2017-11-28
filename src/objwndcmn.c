/************************************************************************************

                                  smb Utility

  File: objwndcmn.c
  Description:オブジェクトビューとオブジェクトリストで共通のコード
  History:

 ************************************************************************************/

#include "smbutil.h"
#include "objmng.h"
#include "objwndcmn.h"

int ObjwndSwitchEditMode(HWND hWnd)
{
	WPARAM wParam;
	int iNewMode = (EDITMODE_MAP == GetMapEditMode()) ? MAKEWPARAM(EDITMODE_BADGUYS, 0) : MAKEWPARAM(EDITMODE_MAP, 0);
	
	wParam = (EDITMODE_MAP == iNewMode) ? IDM_SETTING_MAP : IDM_SETTING_BADGUYS;
	PostMessage(GETFRAMEWNDHANDLE(hWnd), WM_COMMAND, wParam,(LPARAM)NULL);

	return iNewMode;
}
