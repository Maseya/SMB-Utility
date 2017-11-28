/************************************************************************************

                                  smb Utility

  File: roommng.h
  Description:
  History:

 ************************************************************************************/
#ifndef ROOMMNG_H
#define ROOMMNG_H

#define SMB_NUM_ADDRESSDATA       34
#define SMB_NUM_AREAS             36
#define SMB_NUM_WORLDS            8

#define MAKE_ROOMID(b) ((BYTE)( (b)&0x7F ))

//PRG ADDRESS
#define SMB_WORLD_SETTING         0x9CB4
#define SMB_AREA_SETTING          0x9CBC
#define SMB_BADGUYS_ADDRESS_HEAD  0x9CE0
#define SMB_BADGUYS_ADDRESS_LOW   0x9CE4
#define SMB_BADGUYS_ADDRESS_HIGH  0x9D06
#define SMB_MAP_ADDRESS_HEAD      0x9D28
#define SMB_MAP_ADDRESS_LOW       0x9D2C
#define SMB_MAP_ADDRESS_HIGH      0x9D4E

//
#define MOVEOBJ_ERR_SUCCESS     0
#define MOVEOBJ_ERR_SRCOBJ      1
#define MOVEOBJ_ERR_DSTPAGE     2
#define MOVEOBJ_ERR_OBJOVER     4


extern int g_iPage;
extern int g_iWorld;
extern int g_iArea;
extern int g_iArea2;

//uRoomIDとして指定すると現在エディットしているアドレスが返ってくる
// 旧
#define GETADDRESS_CURRENT_EDITTING 0x100
// 新
#define CURRENT_ROOMID GETADDRESS_CURRENT_EDITTING

/**************************************

**************************************/
WORD GetBadGuysAddress(UINT uRoomID);

/**************************************

**************************************/
WORD GetMapAddress(UINT uRoomID);

/**************************************

**************************************/
WORD MapGetAllDataLength(UINT uRoomID);

/**************************************

**************************************/
WORD BadGuysGetAllDataLength(UINT uRoomID);

/**************************************

**************************************/
//現在編集しているルームのルームIDを得る
BYTE GetRoomID();
BYTE rm_GetStartPage();
BYTE rm_GetWorld();
BYTE rm_GetArea();
BYTE rm_GetArea2();

BYTE rm_GetMainRoomID(int iAreaIndex);

BOOL rm_IsThereObject();

BOOL rm_IsSubRoom();

BOOL rm_Initialize();

void rm_UpdateGlobalRoomData();

int GetNumWorlds();

/**************************************

**************************************/
void ChangeRoomAttribute(BYTE bData,int iNewAttr);

void GetValidRoomIDs(LPBYTE pbBuf);

BOOL IsRoomIDValid(BYTE bRoomID);

/**************************************

  ﾙｰﾑを新たに開いた時の初期化を行う

**************************************/
void OpenNewRoomProcess();

void UpdateWorldData(BOOL);



LRESULT CALLBACK AreaSettingDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK AreaSortDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK SendObjectDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK GeneralSettingDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
#endif /* ROOMMNG_H */