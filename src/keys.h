/************************************************************************************

                                  smb Utility

  File: keys.h
  Description:
  History:

 ************************************************************************************/
#ifndef KEYS_H
#define KEYS_H

LRESULT CALLBACK CustomizeDlgProc(HWND, UINT, WPARAM, LPARAM);


typedef enum _tagWHEELFUNC
{
	TYPE1 = 0,
	TYPE16 = 1,
	PAGE = 2,
	OBJ = 3
}WHEELFUNC;

HANDLE ky_Initialize();
VOID ky_WM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam);

#endif