/************************************************************************************

                                  smb Utility

  File: objmng.h
  Description:
  History:

 ************************************************************************************/
#ifndef OBJ_MNG
#define OBJ_MNG

#define INVALID_OBJECT_INDEX     -1

#define CHANGEEDITMODE_ASITIS    -2
#define CHANGEEDITMODE_SWITCHING -1
#define CHANGEEDITMODE_MAP       0
#define CHANGEEDITMODE_BADGUYS   1

void ChangeMapEditMode(int,BOOL);
//
void SetSelectedItem(int,BOOL);
//
#define EDITMODE_MAP        0
#define EDITMODE_BADGUYS    1
//0=Map 1=Bad Guys
int GetMapEditMode();
//
int GetSelectedIndex();

#endif