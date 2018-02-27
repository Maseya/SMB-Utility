/**********************************************************************

                                  smb Utility

  File: objlib.h
  Description:
  History:

 *********************************************************************/
#ifndef OBJLIB_H
#define OBJLIB_H

 /**************************************

   オブジェクトのアドレスに関する定数

   Constant on the address of the object

 ***************************************/
#define SMB_OBJECT_START_ADDRESS 0x9D70

 // このｱﾄﾞﾚｽには、書いてはいけない。（このｱﾄﾞﾚｽの１つ前までは、マップデータ）
 // Do not write on this address. (Map data up to one point before this address)
#define SMB_OBJECT_END_ADDRESS 0xAEDC
#define SMB_ALL_OBJECT_SIZE SMB_OBJECT_END_ADDRESS-SMB_OBJECT_START_ADDRESS

LRESULT CALLBACK EditorOptionDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void LoadEditorOption();

/***************************

  オブジェクトの探索関数

  Object search function

****************************/
typedef struct _tagOBJECTSEEKINFO
{
    // ポインタ
    // pointer
    LPBYTE pbData;

    // ページ
    // page
    DWORD  dwPage;

    // データの先頭（マップは、２バイトのヘッダは除く）
    // Start of data (Map does not include 2-byte header)
    DWORD  dwOfs;

    // アドレスから計算した全ルームデータの長さ
    // length of all room data calculated from address
    DWORD  dwLength;

    // 先頭のオブジェクトを０としたインデックス
    // Index with 0 as the first object
    DWORD  dwIndex;

    // ポインタの指しているデータのサイズ
    // Size of the data pointed to by the pointer
    DWORD  dwObjLen;

    // ページの処理に使われる
    // used for processing pages
    BOOL   blIsPrevPageCom;
}OBJECTSEEKINFO, FAR *LPOBJECTSEEKINFO;

BOOL BadGuysSeekFirst(OBJECTSEEKINFO *psObjSeek, UINT uRoomID);
BOOL BadGuysSeekNext(OBJECTSEEKINFO *psObjSeek);

BOOL MapSeekFirst(OBJECTSEEKINFO *psObjSeek, UINT uRoomID);
BOOL MapSeekNext(OBJECTSEEKINFO *psObjSeek);

#define GETDATAINDEX_ERROR_NOTFOUND -1
#define GETDATAINDEX_ERROR_PL       -2

#define GETINDEX_FLAG_BIN  1
#define GETINDEX_FLAG_XY   2
#define GETINDEX_FLAG_END  4
typedef struct _tagGETINDEXINFO
{
    DWORD dwFlag;
    LPBYTE pbBuf;
    int x;
    int y;
    int iIndex;
    int nNumSamePos;
}GETINDEXINFO, FAR *LPGETINDEXINFO;

int GetMapData(UINT uRoomID, int iIndex, BYTE *pbBuf, int *piPage);
void GetMapHeadData(UINT uRoomID, BYTE *pbBuf);
BOOL SortByPosXMap(UINT uRoomID, int *piCurIndex, BOOL IsResort);
BOOL SetMapData(UINT uRoomID, int iIndex, BYTE *bBuf);
BOOL SetMapDataBinary(UINT uRoomID, int iIndex, BYTE *bBuf, int iValidSize);
int GetMapDataIndex(UINT uRoomID, GETINDEXINFO *psGetIndex, int iPage, BOOL blPageOverDec);

int BadGuysGetDataLength(BYTE *pbBuf);
int GetBadGuysData(UINT uRoomID, int iIndex, BYTE *bBuf, int *piPage);
BOOL SortByPosXBadGuys(UINT uRoomID, int *piCurIndex, BOOL blIsResort);
BOOL SetBadGuysData(UINT uRoomID, int iIndex, BYTE *bBuf);
BOOL SetBadGuysDataBinary(UINT uRoomID, int iIndex, BYTE *bBuf, int iValidSize);
int GetBadGuysDataIndex(UINT uRoomID, GETINDEXINFO *psGetIndex, int iPage, BOOL blPageOverDec);

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
