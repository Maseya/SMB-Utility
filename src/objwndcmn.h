/**********************************************************************

                                  smb Utility

  File: objwndcmn.h
  Description: オブジェクトビューとオブジェクトリストで共通のコード
  Description: Common code in object view and object list
  History:

 *********************************************************************/
#ifndef OBJWNDCMN_H
#define OBJWNDCMN_H

#include <Windows.h>

#define GETFRAMEWNDHANDLE(H) GetParent(GetParent(H))
#define GETCLIENTWNDHANDLE(H) GetParent(H)

int ObjwndSwitchEditMode(HWND hWnd);

#endif
