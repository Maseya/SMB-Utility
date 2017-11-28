/************************************************************************************

                                  smb Utility

  File: objwndcmn.h
  Description:オブジェクトビューとオブジェクトリストで共通のコード
  History:

 ************************************************************************************/
#ifndef OBJWNDCMN_H
#define OBJWNDCMN_H

#define GETFRAMEWNDHANDLE(H)  GetParent(GetParent(H))
#define GETCLIENTWNDHANDLE(H) GetParent(H)

int ObjwndSwitchEditMode(HWND hWnd);

#endif