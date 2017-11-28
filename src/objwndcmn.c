/************************************************************************************

                                  smb Utility

  File: objwndcmn.c
  Description:�I�u�W�F�N�g�r���[�ƃI�u�W�F�N�g���X�g�ŋ��ʂ̃R�[�h
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
