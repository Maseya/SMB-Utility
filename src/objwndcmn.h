/************************************************************************************

                                  smb Utility

  File: objwndcmn.h
  Description:�I�u�W�F�N�g�r���[�ƃI�u�W�F�N�g���X�g�ŋ��ʂ̃R�[�h
  History:

 ************************************************************************************/
#ifndef OBJWNDCMN_H
#define OBJWNDCMN_H

#define GETFRAMEWNDHANDLE(H)  GetParent(GetParent(H))
#define GETCLIENTWNDHANDLE(H) GetParent(H)

int ObjwndSwitchEditMode(HWND hWnd);

#endif