/************************************************************************************

                                  smb Utility

  File: objlib.h
  Description:
  History:

 ************************************************************************************/
#ifndef OBJLIB_H
#define OBJLIB_H

/**************************************

  オブジェクトのアドレスに関する定数

***************************************/
#define SMB_OBJECT_START_ADDRESS 0x9D70
#define SMB_OBJECT_END_ADDRESS 0xAEDC   //このｱﾄﾞﾚｽには、書いてはいけない。（このｱﾄﾞﾚｽの１つ前までは、マップデータ）
#define SMB_ALL_OBJECT_SIZE SMB_OBJECT_END_ADDRESS-SMB_OBJECT_START_ADDRESS

LRESULT CALLBACK EditorOptionDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
void LoadEditorOption();

/***************************

  オブジェクトの探索関数

****************************/
typedef struct _tagOBJECTSEEKINFO
{
	LPBYTE pbData;//ポインタ
	DWORD  dwPage;//ページ
	DWORD  dwOfs;//データの先頭（マップは、２バイトのヘッダは除く）
	DWORD  dwLength;//アドレスから計算した全ルームデータの長さ
	DWORD  dwIndex;//先頭のオブジェクトを０としたインデックス
	DWORD  dwObjLen;//ポインタの指しているデータのサイズ
	BOOL   blIsPrevPageCom;//ページの処理に使われる
}OBJECTSEEKINFO, FAR *LPOBJECTSEEKINFO;

BOOL BadGuysSeekFirst(OBJECTSEEKINFO *psObjSeek,UINT uRoomID);
BOOL BadGuysSeekNext(OBJECTSEEKINFO *psObjSeek);

BOOL MapSeekFirst(OBJECTSEEKINFO *psObjSeek,UINT uRoomID);
BOOL MapSeekNext(OBJECTSEEKINFO *psObjSeek);

/*******************************


********************************/
#define GETDATAINDEX_ERROR_NOTFOUND -1
#define GETDATAINDEX_ERROR_PL       -2

#define GETINDEX_FLAG_BIN  1
#define GETINDEX_FLAG_XY   2
#define GETINDEX_FLAG_END  4
typedef struct _tagGETINDEXINFO
{
	DWORD dwFlag;
	LPBYTE pbBuf;
	int x;//coordinate of mapview
	int y;//coordinate of mapview
	int iIndex;
	int nNumSamePos;
}GETINDEXINFO, FAR *LPGETINDEXINFO;

int GetMapData(UINT uRoomID,int iIndex,BYTE *pbBuf,int *piPage);
void GetMapHeadData(UINT uRoomID,BYTE *pbBuf);
BOOL SortByPosXMap(UINT uRoomID,int *piCurIndex,BOOL IsResort);
BOOL SetMapData(UINT uRoomID,int iIndex,BYTE *bBuf);
BOOL SetMapDataBinary(UINT uRoomID,int iIndex, BYTE *bBuf,int iValidSize);
int GetMapDataIndex(UINT uRoomID,GETINDEXINFO *psGetIndex,int iPage,BOOL blPageOverDec);

int BadGuysGetDataLength(BYTE *pbBuf);
int GetBadGuysData(UINT uRoomID,int iIndex, BYTE *bBuf, int *piPage);
BOOL SortByPosXBadGuys(UINT uRoomID,int *piCurIndex,BOOL blIsResort);
BOOL SetBadGuysData(UINT uRoomID,int iIndex, BYTE *bBuf);
BOOL SetBadGuysDataBinary(UINT uRoomID,int iIndex, BYTE *bBuf,int iValidSize);
int GetBadGuysDataIndex(UINT uRoomID,GETINDEXINFO *psGetIndex,int iPage,BOOL blPageOverDec);

/*******************



*******************/
#define PAGEOBJECT_NO           0
#define PAGEOBJECT_NEXTPAGEFLAG 1
#define PAGEOBJECT_SETPAGE      2
DWORD BadGuysIsPageRelatedObject(LPBYTE lpbBuf);


int GetBadGuysYPos(BYTE *pbBuf);
int GetBadGuysXPos(BYTE *pbBuf);
int GetMapXPos(BYTE *pbBuf);
int GetMapYPos(BYTE *pbBuf);

BOOL BadGuysIsHardObject(BYTE *pbBuf);


#endif /* OBJLIB_H */