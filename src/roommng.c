/************************************************************************************

                                  smb Utility

  File: roommng.c
  Description:
  History:

 ************************************************************************************/
 /*

   アドレス・ワールドの区分・メインル－ムIDを操作する

   アドレス管理(RM)
   オブジェクト管理(OM)

   ルームと敵・マップデータのアドレスの関係
   ・読み込み
   （１）敵・マップデータの開始アドレス(RM)
   （２）有効なルームIDの列挙(RM)
   （３）ルームIDが有効であるか無効であるかのチェック(RM)
   ・書き込み
   （１）ルームのタイプの変更(RM, OM)
   （２）ルーム間のオブジェクトの移動(RM, OM)

   ワールド・エリアとルームの関係

   「Ｂ」現在編集中のルームに関連
   ・ルーム選択ダイアログの表示(RM,OM)
   ・情報の取得(RM,OM)
   　ルームID、開始ページ、ワールド、エリア、エリア２

   ワールドに関連
   ・クリアとなるワールドの管理(RM)
   ・ワールドの区分のデータの管理(RM, OM)

   Address · division of the world · Manipulate the main room ID

   Address management (RM)
   Object management (OM)

   Relationship between room and enemy / map data address
   · Reading
   (1) Start address (RM) of enemy / map data
   (2) Enumeration of valid room ID (RM)
   (3) Check whether the room ID is valid or invalid (RM)
   ·writing
   (1) Changing the type of room (RM, OM)
   (2) Object movement between rooms (RM, OM)

   Relationship between world area and room

   "B" related to the room currently being edited
   · Display of room selection dialog (RM, OM)
   · Information acquisition (RM, OM)
   Room ID, Start page, World, Area, Area 2

   Relating to the world
   · Management of clear world (RM)
   · Data management of world classification (RM, OM)

 */
#include "smbutil.h"
#include "roommng.h"
#include "objview.h"
#include "objlist.h"
#include "objlib.h"
#include "roomseldlg.h"
#include "objmng.h"
#include "emuutil.h"
#include "emulator.h"

 // ﾜｰﾙﾄﾞの区分
 // World division
BYTE        bWorldData[SMB_NUM_WORLDS];

// ﾒｲﾝﾙｰﾑのﾙｰﾑIDの指定
// Designation of the room ID of the main room
BYTE        bAreaData[SMB_NUM_AREAS];
BYTE        addrHeadMap[4];
ADDRESSDATA addrDataMap[SMB_NUM_ADDRESSDATA];
BYTE        addrHeadBadGuys[4];
ADDRESSDATA addrDataBadGuys[SMB_NUM_ADDRESSDATA];

static void LoadCommandAddrData()
{
    int i;

    // Load Value
    memcpy(bWorldData, bPRGROM + SMB_WORLD_SETTING, SMB_NUM_WORLDS);
    memcpy(bAreaData, bPRGROM + SMB_AREA_SETTING, 36);
    memcpy(addrHeadMap, bPRGROM + SMB_MAP_ADDRESS_HEAD, 4);
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        addrDataMap[i].byte.bLower = *(bPRGROM + SMB_MAP_ADDRESS_LOW + i);
        addrDataMap[i].byte.bUpper = *(bPRGROM + SMB_MAP_ADDRESS_HIGH + i);
    }
    memcpy(addrHeadBadGuys, bPRGROM + SMB_BADGUYS_ADDRESS_HEAD, 4);
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        addrDataBadGuys[i].byte.bLower = *(bPRGROM + SMB_BADGUYS_ADDRESS_LOW + i);
        addrDataBadGuys[i].byte.bUpper = *(bPRGROM + SMB_BADGUYS_ADDRESS_HIGH + i);
    }
}

static void SaveCommandAddrData()
{
    int i;

    // Save Value
    memcpy(bPRGROM + SMB_WORLD_SETTING, bWorldData, SMB_NUM_WORLDS);
    memcpy(bPRGROM + SMB_AREA_SETTING, bAreaData, 36);
    memcpy(bPRGROM + SMB_MAP_ADDRESS_HEAD, addrHeadMap, 4);
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        *(bPRGROM + SMB_MAP_ADDRESS_LOW + i) = addrDataMap[i].byte.bLower;
        *(bPRGROM + SMB_MAP_ADDRESS_HIGH + i) = addrDataMap[i].byte.bUpper;
    }
    memcpy(bPRGROM + SMB_BADGUYS_ADDRESS_HEAD, addrHeadBadGuys, 4);
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        *(bPRGROM + SMB_BADGUYS_ADDRESS_LOW + i) = addrDataBadGuys[i].byte.bLower;
        *(bPRGROM + SMB_BADGUYS_ADDRESS_HIGH + i) = addrDataBadGuys[i].byte.bUpper;
    }
}

BYTE rm_GetMainRoomID(int iAreaIndex)
{
    if (iAreaIndex < SMB_NUM_AREAS)
    {
        LoadCommandAddrData();
        return MAKE_ROOMID(bAreaData[iAreaIndex]);
    }
    return 0x80;
}

void rm_UpdateGlobalRoomData()
{
    LoadCommandAddrData();
}

/***********************

  有効なROOMIDを列挙する

  Enumerate valid ROOMID

************************/
typedef struct
{
    BYTE bHead;
    BYTE bAttr;
}GETVALIDROONIDS;

static int compare(const void *arg1, const void *arg2)
{
    if (((GETVALIDROONIDS*)arg1)->bHead < ((GETVALIDROONIDS*)arg2)->bHead)
        return -1;
    else if (((GETVALIDROONIDS*)arg1)->bHead == ((GETVALIDROONIDS*)arg2)->bHead)
        return 0;
    else
        return 1;
}

// pbBufは、SMB_NUM_ADDRESSDATAバイト以上のバッファー
// pbBuf has more than SMB_NUM_ADDRESSDATA bytes of buffer
void GetValidRoomIDs(LPBYTE pbBuf)
{
    GETVALIDROONIDS gvrhMap[4 + 1];
    GETVALIDROONIDS gvrBadGuys[4 + 1];
    BYTE bAttr[] = {0x00,0x20,0x40,0x60};
    int n, b;

    for (n = 0; n < 4; n++)
    {
        gvrhMap[n].bHead = addrHeadMap[n];
        gvrhMap[n].bAttr = bAttr[n];
        gvrBadGuys[n].bHead = addrHeadBadGuys[n];
        gvrBadGuys[n].bAttr = bAttr[n];
    }
    gvrhMap[4].bHead = SMB_NUM_ADDRESSDATA;
    gvrBadGuys[4].bHead = SMB_NUM_ADDRESSDATA;

    qsort(gvrhMap, 4, sizeof(GETVALIDROONIDS), compare);
    qsort(gvrBadGuys, 4, sizeof(GETVALIDROONIDS), compare);

    for (n = 0, b = 0; n < 4; n++)
    {
        int i, max;
        max = gvrhMap[n + 1].bHead - gvrhMap[n].bHead;
        for (i = 0; i < max&&b < SMB_NUM_ADDRESSDATA; i++)
            pbBuf[b++] = (gvrhMap[n].bAttr | (i & 0x1F));
    }
}

// 指定したルームIDが有効かチェック
// Check if the specified room ID is valid
BOOL IsRoomIDValid(BYTE bRoomID)
{
    BYTE bTmpMap;
    BYTE bTmpBadGuys;
    int n;
    int iOldAttr;
    int iOldAreaNum;

    bTmpMap = 0x20;
    bTmpBadGuys = 0x20;
    iOldAttr = ((bRoomID >> 5) & 0x03);
    iOldAreaNum = (bRoomID & 0x1F);

    for (n = 0; n < 4; n++)
    {
        // マップ
        // map
        if (addrHeadMap[iOldAttr] < addrHeadMap[n])
        {
            if (bTmpMap > addrHeadMap[n]) bTmpMap = addrHeadMap[n];
        }

        // 敵
        // enemy
        if (addrHeadBadGuys[iOldAttr] < addrHeadBadGuys[n])
        {
            if (bTmpBadGuys > addrHeadBadGuys[n]) bTmpBadGuys = addrHeadBadGuys[n];
        }
    }

    if (bTmpMap == 0x20) bTmpMap = 34 - addrHeadMap[iOldAttr];
    else bTmpMap -= addrHeadMap[iOldAttr];

    if (bTmpBadGuys == 0x20) bTmpBadGuys = 34 - addrHeadBadGuys[iOldAttr];
    else bTmpBadGuys -= addrHeadBadGuys[iOldAttr];

    if (bTmpMap <= iOldAreaNum || bTmpBadGuys <= iOldAreaNum) return FALSE;

    return TRUE;
}

/*************************

  ワールド数の取得・変更

  Acquisition / change of world number

**************************/

// ワールドの数、1==ワールド1でクリア,2==ワールド2でクリア
// Number of worlds, 1 == cleared in world 1, 2 == cleared in world 2
int  g_iNumWorlds;

#define SMB_CLEAR_STRINGMUSIC 0x8428
#define SMB_CLEAR_JUDGE       0x846A
#define SMB_CLEAR_PEACH       0xEA17

// 3つとも等しい場合、そのワールドを、等しくない場合-1を返す。
// If both are equal, return that world, or -1 if not equal.
int GetClearWorld()
{
    BYTE bStr, bJdg, bPch;

    bStr = bPRGROM[SMB_CLEAR_STRINGMUSIC];
    bJdg = bPRGROM[SMB_CLEAR_JUDGE];
    bPch = bPRGROM[SMB_CLEAR_PEACH];
    if (bStr == bJdg && bJdg == bPch) return bStr;
    return -1;
}

void SetClearWorld(int iWorld)
{
    bPRGROM[SMB_CLEAR_STRINGMUSIC] = bPRGROM[SMB_CLEAR_JUDGE] = bPRGROM[SMB_CLEAR_PEACH] = (BYTE)iWorld;
}

void SetNumWorlds(int iWorld)
{
    if (iWorld<0
        || iWorld>SMB_NUM_WORLDS) return;

    g_iNumWorlds = iWorld;
}

int GetNumWorlds()
{
    return g_iNumWorlds;
}

// FALSE==メインルーム, TRUE==サブルーム
// FALSE == Main room, TRUE == SUBROOM
BOOL g_fSubRoom;

// メインルームの場合のルームID取得用
// For acquiring room ID in case of main room
int  g_iAreaIndex;

// サブルームの場合のルームID
// Room ID in case of subroom
BYTE g_bRoomID;

// サブルームの開始ページ（メインルームでは、必ず0）
// The start page of the subrobum (always 0 in the main room)
int  g_iPage;

// ワールド
// world
int  g_iWorld;

// エリア（通常）
// Area (normal)
int  g_iArea;

// エリア（導入面も1つのエリアとして数える
// Area (the introduction side also counts as one area
int  g_iArea2;

/*****************************

  編集中のルームの情報を得る

  Get information on the room you are editing

******************************/
BYTE GetRoomID()
{
    return (g_fSubRoom) ? g_bRoomID : MAKE_ROOMID(bAreaData[g_iAreaIndex]);
}

BYTE rm_GetStartPage()
{
    return g_iPage;
}

BYTE rm_GetWorld()
{
    return g_iWorld;
}

BYTE rm_GetArea()
{
    return g_iArea;
}

BYTE rm_GetArea2()
{
    return g_iArea2;
}

BOOL rm_IsSubRoom()
{
    return g_fSubRoom;
}

BOOL rm_IsThereObject()
{
    OBJECTSEEKINFO ObjSeek;

    if (EDITMODE_MAP == GetMapEditMode())
    {
        return MapSeekFirst(&ObjSeek, GETADDRESS_CURRENT_EDITTING);
    }
    else
    {
        return BadGuysSeekFirst(&ObjSeek, GETADDRESS_CURRENT_EDITTING);
    }
}
/************************************************************************

  それぞれのルームの敵・マップデータの先頭アドレス(6502CPUメモリ)を取得

  Acquire the head address (6502 CPU memory) of the enemy / map data of each room

*************************************************************************/

WORD GetBadGuysAddress(UINT uRoomID)
{
    if (uRoomID == CURRENT_ROOMID)
        uRoomID = GetRoomID();
    return addrDataBadGuys[addrHeadBadGuys[((BYTE)uRoomID >> 5) & 0x3] + ((BYTE)uRoomID & 0x1F)].word;
}

WORD GetMapAddress(UINT uRoomID)
{
    if (uRoomID == CURRENT_ROOMID)
        uRoomID = GetRoomID();
    return addrDataMap[addrHeadMap[((BYTE)uRoomID >> 5) & 0x3] + ((BYTE)uRoomID & 0x1F)].word;
}

WORD BadGuysGetAllDataLength(UINT uRoomID)
{
    WORD wRoomAddr;
    WORD wTmpAddr;
    WORD wNextAddr = SMB_OBJECT_END_ADDRESS;
    int n;

    wRoomAddr = GetBadGuysAddress(uRoomID);

    for (n = 0; n < SMB_NUM_ADDRESSDATA; n++)
    {
        wTmpAddr = addrDataBadGuys[n].word;
        if (wRoomAddr < wTmpAddr && wTmpAddr < wNextAddr) wNextAddr = wTmpAddr;
    }

    for (n = 0; n < SMB_NUM_ADDRESSDATA; n++)
    {
        wTmpAddr = addrDataMap[n].word;
        if (wRoomAddr < wTmpAddr && wTmpAddr < wNextAddr) wNextAddr = wTmpAddr;
    }

    return (wNextAddr - wRoomAddr);
}

WORD MapGetAllDataLength(UINT uRoomID)
{
    WORD wRoomAddr;
    WORD wTmpAddr;
    WORD wNextAddr = SMB_OBJECT_END_ADDRESS;
    int n;

    wRoomAddr = GetMapAddress(uRoomID);
    for (n = 0; n < SMB_NUM_ADDRESSDATA; n++)
    {
        wTmpAddr = addrDataMap[n].word;
        if (wRoomAddr < wTmpAddr && wTmpAddr < wNextAddr) wNextAddr = wTmpAddr;
    }
    for (n = 0; n < SMB_NUM_ADDRESSDATA; n++)
    {
        wTmpAddr = addrDataBadGuys[n].word;
        if (wRoomAddr < wTmpAddr && wTmpAddr < wNextAddr) wNextAddr = wTmpAddr;
    }

    return (wNextAddr - wRoomAddr - 2);
}

/***********************

  編集するルームの変更

  Change room to edit

************************/
typedef struct _tagROOMINFO
{
    BYTE bRoomID;
    int iAreaIndex;
    int iWorld;
    int iArea;
    int iArea2;
    int iPage;
    BOOL blAreaStart;
}ROOMINFO, FAR * LPROOMINFO;

int g_iTVImgList[4] = {0};

static HTREEITEM InsertRoomDependencyTreeViewItem(LPROOMINFO *lpRoomInfo, int *piCurRoom, BYTE** pbParentRoom, int iNumParentRooms, HWND hDlg, HTREEITEM hParentItem, HTREEITEM hPrevItem)
{
    OBJECTSEEKINFO sObjSeek;

    // 現在、処理しているルームの情報が保存されているROOMINFO構造体のインデックス
    // The index of the ROOMINFO structure currently storing the information of the room being processed
    UINT nCurRoom = *piCurRoom;

    {
        TVITEM tvi;
        TVINSERTSTRUCT tvins;
        TCHAR cStrBuf[50];

        tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

        // Set the text of the item.
        if (hParentItem == TVI_ROOT)
            wsprintf(cStrBuf, __T("%d-%d (%d) [%.2xH]"), (*lpRoomInfo)[nCurRoom].iWorld + 1, (*lpRoomInfo)[nCurRoom].iArea + 1, (*lpRoomInfo)[nCurRoom].iAreaIndex, (*lpRoomInfo)[nCurRoom].bRoomID);
        else
            wsprintf(cStrBuf, __T("[%.2xH] p=%d"), (*lpRoomInfo)[nCurRoom].bRoomID, (*lpRoomInfo)[nCurRoom].iPage);
        tvi.pszText = cStrBuf;
        tvi.cchTextMax = lstrlen(cStrBuf);

        // Assume the item is not a parent item, so give it a
        // document image.
        tvi.iImage = g_iTVImgList[((*lpRoomInfo)[nCurRoom].bRoomID >> 5) & 0x03];
        tvi.iSelectedImage = g_iTVImgList[((*lpRoomInfo)[nCurRoom].bRoomID >> 5) & 0x03];

        // Save
        tvi.lParam = (LPARAM)nCurRoom;

        tvins.item = tvi;
        tvins.hInsertAfter = hPrevItem;
        tvins.hParent = hParentItem;

        hParentItem = hPrevItem = (HTREEITEM)SendDlgItemMessage(hDlg, IDC_ROOM, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
    }

    if ((*lpRoomInfo)[nCurRoom].bRoomID == GetRoomID() &&
        (*lpRoomInfo)[nCurRoom].iPage == g_iPage &&
        (*lpRoomInfo)[nCurRoom].blAreaStart != g_fSubRoom &&
        (*lpRoomInfo)[nCurRoom].iAreaIndex == g_iAreaIndex &&
        (*lpRoomInfo)[nCurRoom].iWorld == g_iWorld &&
        (*lpRoomInfo)[nCurRoom].iArea == g_iArea &&
        (*lpRoomInfo)[nCurRoom].iArea2 == g_iArea2)
        SendDlgItemMessage(hDlg, IDC_ROOM, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)hParentItem);

    if ((*lpRoomInfo)[nCurRoom].iPage >= 0 && BadGuysSeekFirst(&sObjSeek, (*lpRoomInfo)[nCurRoom].bRoomID))
    {
        for (;;)
        {
            if ((DWORD)(*lpRoomInfo)[nCurRoom].iPage <= sObjSeek.dwPage
                && (sObjSeek.pbData[0] & 0x0f) == 0x0E
                && ((sObjSeek.pbData[2] >> 5) & 0x07) == (*lpRoomInfo)[nCurRoom].iWorld)
            {
                int n;
                for (n = 0; n < iNumParentRooms; n++)
                {
                    if ((*pbParentRoom)[n] == (sObjSeek.pbData[1] & 0x7F)) goto NEXTOBJ;
                }

                iNumParentRooms++;
                (*pbParentRoom) = Mrealloc(*pbParentRoom, iNumParentRooms);
                if (!(*pbParentRoom)) break;
                (*pbParentRoom)[iNumParentRooms - 1] = (*lpRoomInfo)[nCurRoom].bRoomID;

                (*piCurRoom) += 1;
                (*lpRoomInfo) = Mrealloc((*lpRoomInfo), ((*piCurRoom) + 1) * sizeof(ROOMINFO));
                if (!(*lpRoomInfo)) break;
                (*lpRoomInfo)[(*piCurRoom)].bRoomID = (BYTE)(sObjSeek.pbData[1] & 0x7F);
                (*lpRoomInfo)[(*piCurRoom)].iWorld = (*lpRoomInfo)[(*piCurRoom) - 1].iWorld;
                (*lpRoomInfo)[(*piCurRoom)].iArea = (*lpRoomInfo)[(*piCurRoom) - 1].iArea;
                (*lpRoomInfo)[(*piCurRoom)].iArea2 = (*lpRoomInfo)[(*piCurRoom) - 1].iArea2;
                (*lpRoomInfo)[(*piCurRoom)].iAreaIndex = (*lpRoomInfo)[(*piCurRoom) - 1].iAreaIndex;
                (*lpRoomInfo)[(*piCurRoom)].iPage = (int)(sObjSeek.pbData[2] & 0x1F);
                (*lpRoomInfo)[(*piCurRoom)].blAreaStart = FALSE;
                InsertRoomDependencyTreeViewItem(lpRoomInfo, piCurRoom, pbParentRoom, iNumParentRooms, hDlg, hPrevItem, hParentItem);
            }
        NEXTOBJ:
            if (!BadGuysSeekNext(&sObjSeek)) break;
        }
    }

    return hPrevItem;
}

// 指定されたルームID、エリアのインデックスからワールドとエリアを取得
// iArea  --- 通常のもの
// iArea2 --- 導入面も1つのエリアとして数える
// Get world and area from specified room ID, area index
// iArea --- normal things
// iArea 2 --- The introduction side also counts as one area
static BOOL GetWorldArea(int *piWorld, int *piAreaNormal, int *piArea2, int iAreaNumber, BYTE bRoomData)
{
    int iWRet = 0;
    int iARet = 0;
    int iARet2 = 0;
    register int n;
    BOOL blAutoWalk;

    bRoomData = MAKE_ROOMID(bRoomData);

    blAutoWalk = FALSE;
    for (n = 0; n < SMB_NUM_AREAS; n++)
    {
        BYTE *pbData;
        BYTE bRoomID;

        bRoomID = MAKE_ROOMID(bAreaData[n]);

        // エリア内を検索
        // Search within the area
        pbData = bPRGROM + GetMapAddress(bRoomID);

        // pbDataは、マップヘッダを指している
        // pbData points to the map header
        blAutoWalk = (((pbData[0] >> 3) & 0x06) == 0x06) ? TRUE : FALSE;

        // マップヘッドを飛ばす。
        // Skip the map head.
        pbData += 2;

        for (;;)
        {
            if (pbData > bPRGROM + 0xFFFF) return FALSE;

            // 検索したエリアが求めるエリアなら終了
            // If the area that the searched area requires is finished
            if (bRoomID == bRoomData && iAreaNumber == n)
            {
                if (piWorld) *piWorld = iWRet;
                if (piAreaNormal) *piAreaNormal = iARet;
                if (piArea2) *piArea2 = iARet2;

                if (iWRet >= GetNumWorlds())
                    return FALSE;

                goto ENDSEEK;
            }

            if (*pbData == 0xFD) break;

            // ”おの”のオブジェクト
            // TODO: Translate
            if (((pbData[0] & 0x0F) == 0x0D) && ((pbData[1] & 0x7F) == 0x42))
            {
                iWRet++;
                iARet = -1;
                iARet2 = -1;
                break;
            }
            pbData += 2;
        }

        if (!blAutoWalk) iARet++;

        iARet2++;
    }
ENDSEEK:
    return TRUE;
}

static LPVOID UpdateRoomDepedencyTreeView(HWND hDlg)
{
    int a, r;
    HTREEITEM hPrevItem = TVI_FIRST;
    ROOMINFO *lpRoomInfo;

    r = 0;
    lpRoomInfo = (LPROOMINFO)Malloc(sizeof(ROOMINFO));
    if (!lpRoomInfo) return (LPVOID)lpRoomInfo;

    for (a = 0; a < SMB_NUM_AREAS; a++, r++)
    {
        int iWorld, iArea, iArea2;
        BYTE *pbParentRoom;

        pbParentRoom = Malloc(1);
        if (!pbParentRoom) return (LPVOID)lpRoomInfo;

        lpRoomInfo = (LPROOMINFO)Mrealloc(lpRoomInfo, sizeof(ROOMINFO)*(r + 1));
        if (!lpRoomInfo) return (LPVOID)lpRoomInfo;

        lpRoomInfo[r].bRoomID = pbParentRoom[0] = (MAKE_ROOMID(bAreaData[a]));
        GetWorldArea(&iWorld, &iArea, &iArea2, a, bAreaData[a]);
        lpRoomInfo[r].iAreaIndex = a;
        lpRoomInfo[r].iWorld = iWorld;
        lpRoomInfo[r].iArea = iArea;
        lpRoomInfo[r].iArea2 = iArea2;
        lpRoomInfo[r].iPage = 0;
        lpRoomInfo[r].blAreaStart = TRUE;

        if (iWorld < GetNumWorlds())
        {
            hPrevItem = InsertRoomDependencyTreeViewItem(&lpRoomInfo,
                                                         &r,
                                                         &pbParentRoom,
                                                         1,
                                                         hDlg,
                                                         (HTREEITEM)TVI_ROOT,
                                                         hPrevItem);
        }
        if (pbParentRoom)
        {
            Mfree(pbParentRoom);
            pbParentRoom = NULL;
        }
    }

    return (LPVOID)lpRoomInfo;
}

static BOOL SetRoomDepedencyTreeViewImageList(HWND hDlg)
{
#define NUM_BITMAPS 4
#define CX_BITMAP 16
#define CY_BITMAP 16
    HIMAGELIST himl;  // handle to image list
    HBITMAP hbmp, hmask;     // handle to bitmap
    int n;
    LPTSTR lpImgResName[] = {__T("SEA_IMG"),__T("SKY_IMG"),__T("UG_IMG"),__T("CASTLE_IMG")};

    // Create the image list.
    if ((himl = ImageList_Create(CX_BITMAP, CY_BITMAP, ILC_MASK, NUM_BITMAPS, 0)) == NULL) return FALSE;

    // Add the open file, closed file, and document bitmaps.
    hmask = LoadBitmap(GetModuleHandle(NULL), __T("TVMASK"));
    for (n = 0; n < NUM_BITMAPS; n++)
    {
        hbmp = LoadBitmap(GetModuleHandle(NULL), lpImgResName[n]);
        g_iTVImgList[n] = ImageList_Add(himl, hbmp, (HBITMAP)hmask);
        DeleteObject(hbmp);
    }
    DeleteObject(hmask);

    // Fail if not all of the images were added.
    if (ImageList_GetImageCount(himl) < NUM_BITMAPS)return FALSE;

    // Associate the image list with the tree view control.
    SendDlgItemMessage(hDlg, IDC_ROOM, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)himl);

    return TRUE;
}

BOOL rm_Initialize()
{
    int i;

    g_iAreaIndex = 0;
    g_fSubRoom = 0;
    g_iPage = 0;
    g_iWorld = 0;
    g_iArea = 0;
    g_iArea2 = 0;
    g_fSubRoom = FALSE;

    // Set num worlds
    SetNumWorlds(8);
    i = GetClearWorld();
    if (i != -1) SetNumWorlds(i + 1);

    LoadCommandAddrData();

    return TRUE;
}

void OpenNewRoomProcess()
{
    SetObjectViewCursole(g_iPage);

    UpdateObjectViewCursole();
    UpdateObjectView(0);
    UpdateObjectList(0);
    UpdateStatusBarRoomInfoText(NULL);
}

LRESULT CALLBACK AreaSettingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        LPROOMINFO lpRoomInfo;

        SetRoomDepedencyTreeViewImageList(hDlg);
        lpRoomInfo = (LPROOMINFO)UpdateRoomDepedencyTreeView(hDlg);
        SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)lpRoomInfo);
        return 0;
    }
    case WM_DESTROY:
    {
        LPROOMINFO lpRoomInfo;

        HIMAGELIST himl = (HIMAGELIST)SendDlgItemMessage(hDlg, IDC_ROOM, TVM_GETIMAGELIST, TVSIL_NORMAL, 0);
        DestroyWindow(GetDlgItem(hDlg, IDC_ROOM));
        ImageList_Destroy(himl);

        lpRoomInfo = (LPROOMINFO)GetWindowLongPtr(hDlg, GWLP_USERDATA);
        if (lpRoomInfo) Mfree(lpRoomInfo);
        return TRUE;
    }
    case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDOK:
    {
        HTREEITEM hSelItem;
        TVITEM tviSelItem;
        LPROOMINFO lpRoomInfo;
        int iIndex;

        // Get handle of selected item
        hSelItem = (HTREEITEM)SendDlgItemMessage(hDlg, IDC_ROOM, TVM_GETNEXTITEM, (WPARAM)TVGN_CARET, (LPARAM)NULL);
        if (hSelItem)
        {
            memset(&tviSelItem, 0, sizeof(TVITEM));
            tviSelItem.mask = TVIF_HANDLE;
            tviSelItem.hItem = hSelItem;
            if (SendDlgItemMessage(hDlg, IDC_ROOM, TVM_GETITEM, 0, (LPARAM)&tviSelItem))
            {
                lpRoomInfo = (LPROOMINFO)GetWindowLongPtr(hDlg, GWLP_USERDATA);
                if (lpRoomInfo)
                {
                    iIndex = (int)tviSelItem.lParam;

                    g_iAreaIndex = lpRoomInfo[iIndex].iAreaIndex;
                    g_bRoomID = lpRoomInfo[iIndex].bRoomID;
                    g_iWorld = lpRoomInfo[iIndex].iWorld;
                    g_iArea = lpRoomInfo[iIndex].iArea;
                    g_iArea2 = lpRoomInfo[iIndex].iArea2;
                    g_iPage = lpRoomInfo[iIndex].iPage;
                    g_fSubRoom = 1;
                    if (lpRoomInfo[iIndex].blAreaStart) g_fSubRoom = 0;
                }
            }
        }

        OpenNewRoomProcess();
    }
    case IDCANCEL:
    EndDialog(hDlg, TRUE);
    return TRUE;
    }
    break;
    }
    return FALSE;
}

/********************

  ルームの並べ替え

  Sort the rooms

*********************/

static void UpdateAreaSortPreview(HWND hDlg)
{
    LRESULT I;
    BOOL fTranslated;
    HWND hPWnd;
    HDC hPDC;
    RECT rcP;

    I = SendDlgItemMessage(hDlg, IDC_AREA, LB_GETCURSEL, 0, 0);
    if (I == LB_ERR) return;

    INT P = GetDlgItemInt(hDlg, IDC_PAGEEDIT, &fTranslated, FALSE);
    if (!fTranslated) return;

    hPWnd = GetDlgItem(hDlg, IDC_VIEW);
    GetClientRect(hPWnd, &rcP);
    hPDC = GetDC(hPWnd);
    if (hPDC)
    {
        if (RunEmulatorViewPage(bAreaData[I], P))
            TransferFromEmuBackBuffer(hPDC, 0, 0, rcP.right - rcP.left, rcP.bottom - rcP.top, TRUE);
        else
            FillRect(hPDC, &rcP, GetSysColorBrush(COLOR_3DFACE));
        ReleaseDC(hPWnd, hPDC);
    }
    ClearEmuBackBuffer();
}

static void UpdateAreaSortListBox(HWND hDlg)
{
    int n;

    LPTSTR szBuf = GetTempStringBuffer();
    int iWorld;
    int iArea;
    LPTSTR lpAttr[] = {STRING_SEA, STRING_SKY, STRING_UNDERGROUND, STRING_CASTLE};

    //clear all item
    SendDlgItemMessage(hDlg, IDC_AREA, LB_RESETCONTENT, 0, 0);

    //show
    for (n = 0; n < SMB_NUM_AREAS; n++)
    {
        GetWorldArea(&iWorld, &iArea, NULL, n, bAreaData[n]);
        _stprintf(szBuf, __T("%d-%d %.2xH %s"), iWorld + 1, iArea + 1, bAreaData[n], lpAttr[(bAreaData[n] >> 5) & 0x03]);
        SendDlgItemMessage(hDlg, IDC_AREA, LB_ADDSTRING, 0, (LPARAM)szBuf);
    }
}

LRESULT CALLBACK AreaSortDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL sblWritten;
    switch (message)
    {
    case WM_PAINT:
    UpdateAreaSortPreview(hDlg);

    // 重要
    // important
    return FALSE;
    case WM_INITDIALOG:
    {
        sblWritten = FALSE;

        LoadCommandAddrData();
        UpdateAreaSortListBox(hDlg);
        if (!g_fSubRoom)
            SendDlgItemMessage(hDlg, IDC_AREA, LB_SETCURSEL, g_iAreaIndex, 0);
        else
            SendDlgItemMessage(hDlg, IDC_AREA, LB_SETCURSEL, 0, 0);
        SendDlgItemMessage(hDlg, IDC_PAGEEDITSPIN, UDM_SETRANGE, 0, MAKEWPARAM(SMB_MAX_PAGE, 0));
        return TRUE;
    }
    case WM_COMMAND:
    {
        WORD wNotifyCode = HIWORD(wParam);
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
        if (sblWritten)
        {
            undoPrepare(UNDONAME_TOOLAREAROOM);
            SaveCommandAddrData();
            OpenNewRoomProcess();
            UpdateWorldData(FALSE);

            fr_SetDataChanged(TRUE);
        }
        EndDialog(hDlg, TRUE);
        return TRUE;
        case IDC_PAGEEDIT:
        {
            if (wNotifyCode == EN_CHANGE)
            {
                UpdateAreaSortPreview(hDlg);
            }
        }
        return TRUE;
        case IDC_UP:
        {
            LRESULT iSel;
            BYTE bTmp;

            iSel = SendDlgItemMessage(hDlg, IDC_AREA, LB_GETCURSEL, 0, 0);
            if (iSel == 0 || iSel == LB_ERR) return TRUE;

            sblWritten = TRUE;

            bTmp = bAreaData[iSel];
            bAreaData[iSel] = bAreaData[iSel - 1];
            bAreaData[iSel - 1] = bTmp;

            UpdateAreaSortListBox(hDlg);
            SendDlgItemMessage(hDlg, IDC_AREA, LB_SETCURSEL, iSel - 1, 0);
            UpdateAreaSortPreview(hDlg);
        }
        return TRUE;
        case IDC_DOWN:
        {
            LRESULT iSel;
            BYTE bTmp;

            iSel = SendDlgItemMessage(hDlg, IDC_AREA, LB_GETCURSEL, 0, 0);
            if (iSel == SMB_NUM_AREAS - 1 || iSel == LB_ERR) return TRUE;

            sblWritten = TRUE;

            bTmp = bAreaData[iSel];
            bAreaData[iSel] = bAreaData[iSel + 1];
            bAreaData[iSel + 1] = bTmp;

            UpdateAreaSortListBox(hDlg);
            SendDlgItemMessage(hDlg, IDC_AREA, LB_SETCURSEL, iSel + 1, 0);
            UpdateAreaSortPreview(hDlg);
            return TRUE;
        }
        case IDC_AREA:
        {
            if (wNotifyCode == LBN_DBLCLK)
            {
                ROOMSELECT sRoomSel;
                LRESULT iTmpCurSel;

                iTmpCurSel = SendDlgItemMessage(hDlg, IDC_AREA, LB_GETCURSEL, 0, 0);
                if (iTmpCurSel == LB_ERR) return TRUE;
                sRoomSel.blDoInit = TRUE;
                sRoomSel.bInitRoomID = bAreaData[iTmpCurSel];
                sRoomSel.uInitPage = 0;
                sRoomSel.lpszTitle = STRING_AREASORT_TITLE;
                if (RoomSelectDialogBox(hDlg, &sRoomSel))
                {
                    sblWritten = TRUE;
                    bAreaData[iTmpCurSel] = sRoomSel.bNewRoomID;
                    UpdateAreaSortListBox(hDlg);
                    SendDlgItemMessage(hDlg, IDC_AREA, LB_SETCURSEL, iTmpCurSel, 0);
                    UpdateAreaSortPreview(hDlg);
                }
                return TRUE;
            }
            else if (wNotifyCode == LBN_SELCHANGE)
            {
                UpdateAreaSortPreview(hDlg);
            }
        }
        break;
        }
    }
    }
    return FALSE;
}

LRESULT CALLBACK GeneralSettingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        int iClrWorld;

        SendDlgItemMessage(hDlg, IDC_CLEARWORLDSPIN, UDM_SETRANGE, 0, MAKEWPARAM(8, 1));
        iClrWorld = GetClearWorld();
        if (iClrWorld != -1)
        {
            SetDlgItemInt(hDlg, IDC_CLEARWORLD, iClrWorld + 1, FALSE);
        }
        else
        {
            CheckDlgButton(hDlg, IDC_ISCLEARWORLD, BST_CHECKED);
            SetDlgItemInt(hDlg, IDC_CLEARWORLD, 1, FALSE);
        }
    }
    break;
    case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDOK:
    {
        BOOL blSuccess;
        int iRet;
        if (BST_UNCHECKED == IsDlgButtonChecked(hDlg, IDC_ISCLEARWORLD))
        {
            iRet = GetDlgItemInt(hDlg, IDC_CLEARWORLD, &blSuccess, FALSE);
            if (blSuccess && (iRet > 0 && iRet <= 8))
            {
                SetClearWorld(iRet - 1);
                SetNumWorlds(iRet);
            }
            else
                return TRUE;
        }

        fr_SetDataChanged(TRUE);
    }
    case IDCANCEL:
    {
        EndDialog(hDlg, TRUE);
        return TRUE;
    }
    break;
    }
    }

    return FALSE;
}
/********************

  ワールドの自動設定

  Automatic setting of world

*********************/
void UpdateWorldData(BOOL fCommand)
{
    int w, a;
    int iWorld;

    if (fCommand)
        undoPrepare(UNDONAME_TOOLWORLD);

    LoadCommandAddrData();
    for (w = 0, a = 0; w < GetNumWorlds(); w++)
    {
        bWorldData[w] = a;
        for (;;)
        {
            if (!GetWorldArea(&iWorld, NULL, NULL, a, bAreaData[a]))
                goto CANCEL;
            if (w != iWorld || a >= SMB_NUM_AREAS) break;
            a++;
        }
    }

CANCEL:

    SaveCommandAddrData();

    fr_SetDataChanged(TRUE);
}

/**********

  smbattr

***********/
void ChangeRoomAttribute(BYTE bData, int iNewAttr)
{
    int iOldAttr;
    int iOldAreaNum;
    BYTE bOldAddrHeadMap[4];
    BYTE bOldAddrHeadBadGuys[4];
    int iNewAreaNum;
    int n;
    BYTE bBuf;
    ADDRESSDATA addrDataBuf;

    LoadCommandAddrData();

    iOldAttr = ((bData >> 5) & 0x3);
    iOldAreaNum = (bData & 0x1F);

    memcpy(bOldAddrHeadMap, addrHeadMap, 4);
    memcpy(bOldAddrHeadBadGuys, addrHeadBadGuys, 4);

    // 新しい属性と現在の属性が違うかチェックする。
    // Check whether the new attribute is different from the current attribute.
    if (iOldAttr == iNewAttr) return;

    // 新しい敵キャラコマンドアドレスヘッダ
    // New enemy character command address header
    /*
    0（海）の属性が増える  　1,2,3の属性 を +1
    1（地上）の属性が増える 　2,3 の属性 を +1
    2（地下）の属性が増える  　3  の属性 を +1
    3（城）の属性が増える

    0（海）の属性が減る  　1,2,3の属性 を -1
    1（地上）の属性が減る 　2,3 の属性 を -1
    2（地下）の属性が減る  　3  の属性 を -1
    3（城）の属性が減る

    この2つの操作の組み合わせ

    Attribute of 0 (sea) increases 1, 2, 3 attributes +1
    Attributes of 1 (above ground) increase 2, 3 attributes +1
    Increase the attribute of 2 (underground) Attribute 3 to +1
    3 (castle) attributes increase

    Attributes of 0 (sea) decrease 1, 2, 3 attributes -1
    Attributes of 1 (above ground) decreasing 2, 3 attributes -1
    Attribute of 3 (underground) decreases Attributes of 3 to -1
    Attribution of 3 (castle) decreases

    Combination of these two operations
    */

    // 敵キャラ
    // Enemy character
    for (n = 0; n <= 3; n++)
    {
        if (bOldAddrHeadBadGuys[n] > bOldAddrHeadBadGuys[iNewAttr])
            addrHeadBadGuys[n]++;
    }
    for (n = 0; n <= 3; n++)
    {
        if (bOldAddrHeadBadGuys[n] > bOldAddrHeadBadGuys[iOldAttr])
            addrHeadBadGuys[n]--;
    }

    // マップ
    // map
    for (n = 0; n <= 3; n++)
    {
        if (bOldAddrHeadMap[n] > bOldAddrHeadMap[iNewAttr])
            addrHeadMap[n]++;
    }
    for (n = 0; n <= 3; n++)
    {
        if (bOldAddrHeadMap[n] > bOldAddrHeadMap[iOldAttr])
            addrHeadMap[n]--;
    }

    // iNewAreaNum の計算（新しいヘッダを計算した後）
    // Calculation of iNewAreaNum (after calculating new header)
    bBuf = 34;
    for (n = 0; n <= 3; n++)
    {
        if (addrHeadBadGuys[n] > addrHeadBadGuys[iNewAttr] && bBuf > addrHeadBadGuys[n])
            bBuf = addrHeadBadGuys[n];
    }
    iNewAreaNum = bBuf - addrHeadBadGuys[iNewAttr] - 1;

    // 敵キャラコマンドアドレスデータの入れ換え
    // Exchange enemy character command address data
    memcpy(&addrDataBuf, &addrDataBadGuys[bOldAddrHeadBadGuys[iOldAttr] + iOldAreaNum], sizeof(ADDRESSDATA));
    for (n = bOldAddrHeadBadGuys[iOldAttr] + iOldAreaNum; n < 33; n++)
        memcpy(&addrDataBadGuys[n], &addrDataBadGuys[n + 1], sizeof(ADDRESSDATA));
    for (n = 33; n > addrHeadBadGuys[iNewAttr] + iNewAreaNum; n--)
        memcpy(&addrDataBadGuys[n], &addrDataBadGuys[n - 1], sizeof(ADDRESSDATA));
    memcpy(&addrDataBadGuys[n], &addrDataBuf, sizeof(ADDRESSDATA));

    // マップ
    // map
    memcpy(&addrDataBuf, &addrDataMap[bOldAddrHeadMap[iOldAttr] + iOldAreaNum], sizeof(ADDRESSDATA));
    for (n = bOldAddrHeadMap[iOldAttr] + iOldAreaNum; n < 33; n++)
        memcpy(&addrDataMap[n], &addrDataMap[n + 1], sizeof(ADDRESSDATA));
    for (n = 33; n > addrHeadMap[iNewAttr] + iNewAreaNum; n--)
        memcpy(&addrDataMap[n], &addrDataMap[n - 1], sizeof(ADDRESSDATA));
    memcpy(&addrDataMap[n], &addrDataBuf, sizeof(ADDRESSDATA));

    // エリアデータの書き換え
    // エリアのデータを新しい属性の書き換える
    // Rewrite area data
    // Rewrite the data of the area with the new attribute
    for (n = 0; n < 36; n++)
    {
        if (((bAreaData[n] >> 5) & 0x3) == iOldAttr)
        {
            if (iOldAreaNum == (bAreaData[n] & 0x1f))
                bAreaData[n] = (iNewAttr << 5) | iNewAreaNum;
            else if (iOldAreaNum < (bAreaData[n] & 0x1f))
                bAreaData[n]--;
        }
    }

    // ルーム変更コマンドの書き換え
    // ・属性を変更したルームへの
    // ・ルーム指定を変更したルームへの
    // Rewrite the room change command
    // · To the room whose attribute was changed
    // · To the room where room specification was changed
    for (n = 0; n < 34; n++)
    {
        BYTE *pbData;

        pbData = bPRGROM + (addrDataBadGuys[n].word);

        for (;;)
        {
            if (*pbData == 0xFF) break;
            if ((*pbData & 0x0f) == 0x0E)
            {
                int iPageFlag = 0;

                if (((*(pbData + 1) >> 5) & 0x3) == iOldAttr)
                {
                    if (iOldAreaNum == (*(pbData + 1) & 0x1f))
                    {
                        if (*(pbData + 1) & 0x80) iPageFlag = 1;
                        *(pbData + 1) = (iNewAttr << 5) | iNewAreaNum;
                        if (iPageFlag) *(pbData + 1) |= 0x80;
                    }
                    else if (iOldAreaNum < (*(pbData + 1) & 0x1f))
                    {
                        if (*(pbData + 1) & 0x80) iPageFlag = 1;
                        *(pbData + 1) -= 1;
                        if (iPageFlag) *(pbData + 1) |= 0x80;
                    }
                }
                pbData += 3;
            }
            else
                pbData += 2;
        }
    }

    // グローバルな値の再設定
    // Resetting global values
    if (g_fSubRoom) g_bRoomID = ((iNewAttr << 5) | iNewAreaNum);

    SaveCommandAddrData();
}

/********************************

  オブジェクトのルーム間の移動

  Move objects between rooms

*********************************/

/*
＊＊＊＊＊＊＊＊＊＊＊＊＊重要＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊＊
　敵オブジェクトとマップオブジェクトでは、
    ＊　ページ送りコマンド
    ＊　改ページフラグを持ったオブジェクト
　のようなオブジェクトデータの場合、実際に表示されるオブジェクトの様子には、違いがある。
　具体的には、敵オブジェクトの場合、
    P ページ　ページ送りコマンド
    P ページ　改ページフラグを持ったオブジェクト
　マップオブジェクトの場合
    P ページ　　　ページ送りコマンド
    (P+1) ページ　改ページフラグを持ったオブジェクト
　のように表示される。

　以上の

 *************important********************
 For enemy objects and map objects,
 * Page feed command
 * Object with page break flag
 In the case of object data such as, there are differences in the appearance of objects actually displayed.
 Specifically, in the case of an enemy object,
 P page page feed command
 Object with P page page break flag
 For map objects
 P page page feed command
 (P + 1) Object with page break page flag
 As shown in FIG.

 More than
*/

// マップ用
// for map
static BOOL IsMapPageRelatedObject(LPBYTE lpbBuf)
{
    if ((lpbBuf[1] & 0x80) || (((lpbBuf[0] & 0x0F) == 0x0D) && !((lpbBuf[1]) & 0x40)))
        return TRUE;
    return FALSE;
}

static UINT MapMoveObject(UINT uRoomIDSrc, int iIndexSrc, UINT uRoomIDDst, int iPageDst)
{
    BYTE bBufSrc[2];
    OBJECTSEEKINFO ObjSeekSrc;
    DWORD dwAddrSrc;
    OBJECTSEEKINFO ObjSeekDst;
    DWORD dwAddrDst;
    register int i;
    BOOL fPrevDst;

    //Get information of source object
    if (iIndexSrc < 0 || iPageDst < 0 || !MapSeekFirst(&ObjSeekSrc, uRoomIDSrc)) return FALSE;
    for (;;)
    {
        if (ObjSeekSrc.dwIndex == (DWORD)iIndexSrc && !IsMapPageRelatedObject(ObjSeekSrc.pbData))
            break;
        if (!MapSeekNext(&ObjSeekSrc))
        {
            return MOVEOBJ_ERR_SRCOBJ;
        }
    }
    dwAddrSrc = GetMapAddress(uRoomIDSrc) + ObjSeekSrc.dwOfs + 2;

    // 送り先のルームは、SMBエンジンで処理できる範囲のオブジェクト数か
    // The destination room is the number of objects that can be handled by the SMB engine
    if (MapSeekFirst(&ObjSeekDst, uRoomIDDst))
    {
        for (;;)
        {
            if (!MapSeekNext(&ObjSeekDst))
                break;
        }
    }
    if (ObjSeekDst.dwOfs + 2 > 0xFF)
        return MOVEOBJ_ERR_OBJOVER;

    // 送り先のページにオブジェクトが1つしかない場合への対応のため
    // To deal with cases where there is only one object on the destination page
    fPrevDst = FALSE;
    if (MapSeekFirst(&ObjSeekDst, uRoomIDDst))
    {
        for (;;)
        {
            BOOL fPageObj = IsMapPageRelatedObject(ObjSeekDst.pbData);
            if ((ObjSeekDst.dwPage == (DWORD)iPageDst))
            {
                if (!fPageObj) break;
                fPrevDst = TRUE;
            }
            else if (fPrevDst)
            {
                if (fPageObj) break;
                fPrevDst = FALSE;
            }
            if (!MapSeekNext(&ObjSeekDst))
            {
                return MOVEOBJ_ERR_DSTPAGE;
            }
        }
    }

    LoadCommandAddrData();

    memcpy(bBufSrc, ObjSeekSrc.pbData, 2);

    // -2は、切り取るオブジェクトのデータ分
    // -2 is the amount of data of the object to cut away
    memmove(ObjSeekSrc.pbData, ObjSeekSrc.pbData + 2, SMB_OBJECT_END_ADDRESS - dwAddrSrc - 2);
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        if (dwAddrSrc < addrDataMap[i].word)
            addrDataMap[i].word -= 2;
    }

    // 送り先のページにオブジェクトが1つしかない場合への対応のため
    // To deal with cases where there is only one object on the destination page
    fPrevDst = FALSE;
    if (MapSeekFirst(&ObjSeekDst, uRoomIDDst))
    {
        for (;;)
        {
            BOOL fPageObj = IsMapPageRelatedObject(ObjSeekDst.pbData);
            if ((ObjSeekDst.dwPage == (DWORD)iPageDst))
            {
                if (!fPageObj) break;
                fPrevDst = TRUE;
            }
            else if (fPrevDst)
            {
                if (fPageObj) break;
                fPrevDst = FALSE;
            }
            if (!MapSeekNext(&ObjSeekDst)) return MOVEOBJ_ERR_DSTPAGE;
        }
    }

    // +2は、ヘッダ分
    // + 2 is the header part
    dwAddrDst = GetMapAddress(uRoomIDDst) + ObjSeekDst.dwOfs + 2;

    // -2は、切り取ったオブジェクトのデータ分
    // -2 is the amount of data of the cut object
    memmove(ObjSeekDst.pbData + 2, ObjSeekDst.pbData, SMB_OBJECT_END_ADDRESS - dwAddrDst - 2);
    memcpy(ObjSeekDst.pbData, bBufSrc, 2);
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        if (dwAddrDst < addrDataMap[i].word)
            addrDataMap[i].word += 2;
    }

    SaveCommandAddrData();

    return MOVEOBJ_ERR_SUCCESS;
}

// 敵用
// Enemy use
static UINT BadGuysMoveObject(UINT uRoomIDSrc, int iIndexSrc, UINT uRoomIDDst, int iPageDst)
{
    BYTE bBufSrc[3];
    OBJECTSEEKINFO ObjSeekSrc;
    DWORD dwAddrSrc;
    int iSrcSize;
    OBJECTSEEKINFO ObjSeekDst;
    DWORD dwAddrDst;
    register int i;
    BOOL fPrevDst;
    DWORD dwPrevPageRelated;

    //Get information of source object
    dwPrevPageRelated = PAGEOBJECT_NO;
    if (iIndexSrc < 0 || iPageDst < 0 || !BadGuysSeekFirst(&ObjSeekSrc, uRoomIDSrc)) return FALSE;
    for (;;)
    {
        /*
            なぜ、敵オブジェクトの場合だけ
            　ページ送りコマンド
            　オブジェクト…（＊）
            　改ページフラグを持ったオブジェクト…（＊＊）
            の（＊）のオブジェクトをページの関係を保つためのオブジェクトとみなす
            のかについて。
            まず、ページの関係を保つ必要がある理由については、ユーザーにとって”
            送る”によってページの関係が変わるのは、予期しないことであるため、エ
            ラーとして扱うべきであることと、何よりも、実装が、ページによる探索を
            行っているため、ページの関係が狂った場合、切り取りは成功しても貼り付け
            でページが存在しないために失敗し、データを破壊してしまう。
            つぎに、上記の（＊）を送ってしまった場合、なぜ敵オブジェクトの場合だけ
            ページの関係が崩れてしまうかについては、
            　ページ送りコマンド
            　改ページフラグを持ったオブジェクト…（＠）
            のようなオブジェクトがあった場合、敵と地形オブジェクトでは2つのオブジェ
            クトのページの扱われ方が違うからである。地形の場合は、（＠）は、ページ
            送りコマンドで指定されたページの次のページにセットされるのに対し、敵の
            場合は、ページ送りコマンドと（＠）のオブジェクトは同じページセットされ
            る。であるから、（＊）のオブジェクトを送った場合、（＊＊）のオブジェク
            トのページは、１つ前という扱いになってしまい、ページの関係がくずれてし
            まう。これでは、送り先ページにオブジェクトが全く存在しなくなる可能性が
            出てきて、仮にそうなった場合貼り付けに失敗する。

            Why, only for enemy objects
            Page feed command
            object…(*)
            Object with page break flag ... (**)
            Consider an object of (*) as an object for maintaining the relation of pages
            About.
            First of all, as to why it is necessary to maintain the relationship of the page,
            Because it is unexpected that the relationship of pages will change depending on "send", D
            What should be treated as a raider, and above all, the implementation is to search by page
            Because it is going, if the relation of the page goes wrong, cutting is successful even if pasted
            Because the page does not exist, it fails and destroys the data.
            Next, if you sent the above (*), why only enemy objects
            As to whether the relation of the page collapses,
            Page feed command
            Object with page break flag ... (@)
            If there is an object like the enemy and the terrain object two objects
            This is because the way pages are handled is different. In the case of terrain, (@), page
            Will be set to the next page of the page specified by the feed command, whereas the enemy's
            If the page feed command and the object of (@) are the same page set
            . Therefore, if you send an object of (*), the object of (**)
            The page of the page was handled as one before, the relationship of the page collapsed
            Watch. With this, there is a possibility that no object will be present at the destination page
            If it comes out, it will fail to paste if that happens.
        */
        DWORD dwPageRelated = BadGuysIsPageRelatedObject(ObjSeekSrc.pbData);
        if (ObjSeekSrc.dwIndex == (DWORD)iIndexSrc
            && !dwPageRelated
            && dwPrevPageRelated != PAGEOBJECT_SETPAGE)
            break;

        dwPrevPageRelated = dwPageRelated;

        if (!BadGuysSeekNext(&ObjSeekSrc))
        {
            return MOVEOBJ_ERR_SRCOBJ;
        }
    }

    dwAddrSrc = GetBadGuysAddress(uRoomIDSrc) + ObjSeekSrc.dwOfs;
    iSrcSize = ObjSeekSrc.dwObjLen;

    // 送り先のルームは、SMBエンジンで処理できる範囲のオブジェクト数か
    // The destination room is the number of objects that can be handled by the SMB engine
    if (BadGuysSeekFirst(&ObjSeekDst, uRoomIDDst))
    {
        for (;;)
        {
            if (!BadGuysSeekNext(&ObjSeekDst))
                break;
        }
    }
    if (ObjSeekDst.dwOfs + iSrcSize > 0xFF)
        return MOVEOBJ_ERR_OBJOVER;

    // 送り先のページにオブジェクトが1つしかない場合への対応のため
    // To deal with cases where there is only one object on the destination page
    fPrevDst = FALSE;
    if (BadGuysSeekFirst(&ObjSeekDst, uRoomIDDst))
    {
        //for no room object
        for (;;)
        {
            BOOL fPageObj = BadGuysIsPageRelatedObject(ObjSeekDst.pbData);
            if ((ObjSeekDst.dwPage == (DWORD)iPageDst))
            {
                if (!fPageObj) break;
                fPrevDst = TRUE;
            }
            else if (fPrevDst)
            {
                if (fPageObj) break;
                fPrevDst = FALSE;
            }

            if (!BadGuysSeekNext(&ObjSeekDst))
            {
                return MOVEOBJ_ERR_DSTPAGE;
            }
        }
    }

    LoadCommandAddrData();

    memcpy(bBufSrc, ObjSeekSrc.pbData, iSrcSize);
    memmove(ObjSeekSrc.pbData, ObjSeekSrc.pbData + iSrcSize, SMB_OBJECT_END_ADDRESS - dwAddrSrc - iSrcSize);//-2は、切り取るオブジェクトのデータ分
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        if (dwAddrSrc < addrDataBadGuys[i].word)
            addrDataBadGuys[i].word -= iSrcSize;
    }

    //ソースのオブジェクトを切り取ったことによって、送り先のポインターが無効になった
    // Destination pointer was invalidated by cutting source object

    // 送り先のページにオブジェクトが1つしかない場合への対応のため
    // To deal with cases where there is only one object on the destination page
    fPrevDst = FALSE;
    if (BadGuysSeekFirst(&ObjSeekDst, uRoomIDDst))
    {
        for (;;)
        {
            BOOL fPageObj = BadGuysIsPageRelatedObject(ObjSeekDst.pbData);
            if ((ObjSeekDst.dwPage == (DWORD)iPageDst))
            {
                if (!fPageObj) break;
                fPrevDst = TRUE;
            }
            else if (fPrevDst)
            {
                if (fPageObj) break;
                fPrevDst = FALSE;
            }
            if (!BadGuysSeekNext(&ObjSeekDst)) return MOVEOBJ_ERR_DSTPAGE;
        }
    }
    dwAddrDst = GetBadGuysAddress(uRoomIDDst) + ObjSeekDst.dwOfs;

    memmove(ObjSeekDst.pbData + iSrcSize, ObjSeekDst.pbData, SMB_OBJECT_END_ADDRESS - dwAddrDst - iSrcSize);//-2は、切り取ったオブジェクトのデータ分
    memcpy(ObjSeekDst.pbData, bBufSrc, iSrcSize);
    for (i = 0; i < SMB_NUM_ADDRESSDATA; i++)
    {
        if (dwAddrDst < addrDataBadGuys[i].word)
            addrDataBadGuys[i].word += iSrcSize;
    }

    SaveCommandAddrData();

    return MOVEOBJ_ERR_SUCCESS;
}

extern void UpdateBadguysEditDlgPreview(HWND hDlg, BOOL blGetRoomIDFromList);

static void ShowSendDlgError(UINT uError)
{
    LPTSTR szError;
    switch (uError)
    {
    case MOVEOBJ_ERR_SRCOBJ:
    szError = STRING_SENDOBJECT_SRCERROR;
    break;
    case MOVEOBJ_ERR_DSTPAGE:
    szError = STRING_SENDOBJECT_DSTERROR;
    break;
    case MOVEOBJ_ERR_OBJOVER:
    szError = STRING_SENDOBJECT_OVEROBJ;
    break;
    }

    Msg(szError, MB_OK | MB_ICONWARNING);
}

LRESULT CALLBACK SendObjectDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BYTE bRoomID;
    static int iPage;
    static BOOL blIsInit = FALSE;

    switch (message)
    {
    case WM_INITDIALOG:
    {
        BYTE bRoomIDs[SMB_NUM_ADDRESSDATA];
        int n;
        TCHAR cBuf[5];

        SendDlgItemMessage(hDlg, IDC_PAGEEDIT2SPIN, UDM_SETRANGE, 0, MAKEWPARAM(SMB_MAX_PAGE, 0));

        if (!blIsInit)
        {
            bRoomID = GetRoomID();
            bRoomID &= 0x7F;
            iPage = 0;
            blIsInit = TRUE;
        }

        wsprintf(cBuf, __T("%.2x"), bRoomID & 0x7F);
        SetDlgItemText(hDlg, IDC_DATA, cBuf);
        GetValidRoomIDs(bRoomIDs);
        for (n = 0; n < SMB_NUM_ADDRESSDATA; n++)
        {
            _stprintf(cBuf, __T("%.2x"), bRoomIDs[n]);
            SendDlgItemMessage(hDlg, IDC_DATA, CB_ADDSTRING, 0, (LPARAM)cBuf);
        }

        wsprintf(cBuf, __T("%d"), iPage);
        SetDlgItemText(hDlg, IDC_PAGEEDIT2, cBuf);
    }
    break;
    case WM_PAINT:
    UpdateBadguysEditDlgPreview(hDlg, FALSE);
    break;
    case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case IDOK:
    {
        TCHAR cBuf[10];
        BOOL blSuccess;
        UINT uRet;

        GetDlgItemText(hDlg, IDC_DATA, cBuf, 20);
        if (1 != _stscanf(cBuf, __T("%hhx"), &bRoomID)) return TRUE;
        if (!IsRoomIDValid(bRoomID)) return TRUE;
        iPage = GetDlgItemInt(hDlg, IDC_PAGEEDIT2, &blSuccess, FALSE);
        if (!blSuccess) return TRUE;

        if (GetMapEditMode())
        {
            undoPrepare(UNDONAME_SENDOBJ);
            uRet = BadGuysMoveObject(GETADDRESS_CURRENT_EDITTING, GetSelectedIndex(), bRoomID, iPage);
            if (uRet == MOVEOBJ_ERR_SUCCESS)
            {
                SortByPosXBadGuys(bRoomID, NULL, FALSE);
                if (bRoomID != GetRoomID() && GetSelectedIndex() > 0) SetSelectedItem(GetSelectedIndex() - 1, TRUE);
                UpdateObjectViewCursole();
            }
            else
            {
                undoRestore();
                ShowSendDlgError(uRet);
                return TRUE;
            }
        }
        else
        {
            undoPrepare(UNDONAME_SENDOBJ);
            uRet = MapMoveObject(GETADDRESS_CURRENT_EDITTING, GetSelectedIndex(), bRoomID, iPage);
            if (uRet == MOVEOBJ_ERR_SUCCESS)
            {
                SortByPosXMap(bRoomID, NULL, FALSE);
                if (bRoomID != GetRoomID() && GetSelectedIndex() > 0) SetSelectedItem(GetSelectedIndex() - 1, TRUE);
                UpdateObjectViewCursole();
            }
            else
            {
                undoRestore();
                ShowSendDlgError(uRet);
                return TRUE;
            }
        }

        fr_SetDataChanged(TRUE);

        UpdateObjectList(0);
        UpdateObjectView(0);
    }
    case IDCANCEL:
    {
        EndDialog(hDlg, TRUE);
        return TRUE;
    }
    case IDC_DATA:
    if (HIWORD(wParam) == CBN_EDITCHANGE)
        UpdateBadguysEditDlgPreview(hDlg, FALSE);
    else if (HIWORD(wParam) == CBN_SELCHANGE)
        UpdateBadguysEditDlgPreview(hDlg, TRUE);
    return TRUE;
    case IDC_PAGEEDIT2:
    if (HIWORD(wParam) == EN_CHANGE)
    {
        UpdateBadguysEditDlgPreview(hDlg, FALSE);
        return TRUE;
    }
    }
    }

    return FALSE;
}
