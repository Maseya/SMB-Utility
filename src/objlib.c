/**********************************************************************

                                  smb Utility

  File: objlib.c
  Description:
  History:

 *********************************************************************/
#include "objlib.h"

#include "ini.h"
#include "objlist.h"
#include "objmng.h"
#include "objview.h"
#include "roommng.h"
#include "smbutil.h"

BOOL g_blIsLengthValid = TRUE;

BOOL ObjectSeekGetIsLengthValid() { return g_blIsLengthValid; }
void ObjectSeekSetIsLengthValid(BOOL blIsValid) { g_blIsLengthValid = blIsValid; }

void LoadEditorOption() {
    UINT uData;

    // HARD
    uData = GetIntegerFromINI(INIFILE_EDITOR, INIFILE_EDITOR_PROTECT, 1);
    if (!uData || uData == 1) g_blIsLengthValid = uData;
}

/*
設定用ダイアログコールバック関数
Setup dialog callback function
*/
LRESULT CALLBACK EditorOptionDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                     LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG: {
            LPPROPSHEETPAGE lpPropPage = (LPPROPSHEETPAGE)lParam;

            if (lpPropPage->lParam) CenterPropatySheet(hDlg);

            CheckDlgButton(hDlg, IDC_ISADDRESS, BST_CHECKED);
            if (!g_blIsLengthValid) CheckDlgButton(hDlg, IDC_ISADDRESS, BST_UNCHECKED);
        } break;
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            switch (pnmh->code) {
                case PSN_APPLY: {
                    g_blIsLengthValid = FALSE;
                    if (BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_ISADDRESS))
                        g_blIsLengthValid = TRUE;
                    return TRUE;
                } break;
            }
        } break;
    }

    return FALSE;
}

/*********************************************

  直接オブジェクトデータを検索するときの補助関数

  Auxiliary function when searching object data directly

**********************************************/
/*********
    敵
    enemy
**********/

// 敵のページについての処理（補助関数）
// Processing for enemy pages (auxiliary function)
static void BadGuysSetPage(OBJECTSEEKINFO* psObjSeek) {
    if (!psObjSeek) return;

    if (((psObjSeek->pbData[1]) & 0x80) && !psObjSeek->blIsPrevPageCom) {
        psObjSeek->dwPage++;
    } else if (((psObjSeek->pbData[0]) & 0x0F) == 0x0F) {
        // nhxx xxxx
        // n:改ページフラグ
        // h:5-3以降出現
        // xxxxxx:新しいページ
        //
        // nhxx xxxx
        // n: page break flag
        // h: Appears after 5-3
        // xxxxxx: New page
        psObjSeek->dwPage = psObjSeek->pbData[1] & 0x3F;
        psObjSeek->blIsPrevPageCom = TRUE;
    }

    if (((psObjSeek->pbData[0]) & 0x0F) != 0x0F) psObjSeek->blIsPrevPageCom = FALSE;
}

// ルーム内のデータならTRUE、ルーム外のデータならFALSEを返す
// Returns TRUE if the data is in the room, FALSE if it is outside the room
static BOOL BadGuysCheckDataLength(OBJECTSEEKINFO* psObjSeek) {
    if (!psObjSeek) return FALSE;

    if ((psObjSeek->dwLength) <
        (psObjSeek->dwOfs) + BadGuysGetDataLength(psObjSeek->pbData))
        return FALSE;

    return TRUE;
}

// 敵のデータのサイズを得る
// Get enemy data size
int BadGuysGetDataLength(BYTE* pbBuf) { return ((pbBuf[0] & 0x0F) == 0x0E) ? 3 : 2; }

// 敵のデータを検索するためのデータを初期化する
// uRoomIDには、取得したいルームのデータを指定。
// GETADDRESS_CURRENT_EDITTINGを指定すると現在エディトしているルーム
// Initialize data for searching enemy's data
// Specify the data of the room you want to obtain for uRoomID.
// If GETADDRESS_CURRENT_EDITTING is specified, the room currently being edited
BOOL BadGuysSeekFirst(OBJECTSEEKINFO* psObjSeek, UINT uRoomID) {
    if (!psObjSeek) return FALSE;

    // 構造体を初期化
    // Initialize structure
    memset(psObjSeek, 0, sizeof(OBJECTSEEKINFO));
    psObjSeek->pbData = bPRGROM + GetBadGuysAddress(uRoomID);

    if (psObjSeek->pbData[0] == 0xFF) return FALSE;

    // ページの情報をセット
    // Set page information
    BadGuysSetPage(psObjSeek);

    // 長さの情報をセット
    // Set length information
    psObjSeek->dwLength = BadGuysGetAllDataLength(uRoomID);

    // オブジェクトのサイズ
    // Size of object
    psObjSeek->dwObjLen = BadGuysGetDataLength(psObjSeek->pbData);

    if (g_blIsLengthValid && !BadGuysCheckDataLength(psObjSeek)) return FALSE;

    return TRUE;
}

// 次のデータを得る
// 正しくデータを取得できた場合は、TRUE,　NULLを渡すか、データの終了の場合は、FALSEを返す。
// Obtain the next data
// If TRUE, NULL is passed if data can be acquired correctly, FALSE is returned in the
// case of data termination.
BOOL BadGuysSeekNext(OBJECTSEEKINFO* psObjSeek) {
    int iCurObjSize;
    if (!psObjSeek) return FALSE;

    iCurObjSize = BadGuysGetDataLength(psObjSeek->pbData);
    psObjSeek->pbData += iCurObjSize;
    psObjSeek->dwOfs += iCurObjSize;

    if (psObjSeek->pbData[0] == 0xFF) return FALSE;

    // ページの情報をセット
    // Set page information
    BadGuysSetPage(psObjSeek);

    // インデックスをインクリメント
    // Increment index
    psObjSeek->dwIndex++;

    // オブジェクトのサイズ
    // Size of object
    psObjSeek->dwObjLen = BadGuysGetDataLength(psObjSeek->pbData);

    if (g_blIsLengthValid && (!BadGuysCheckDataLength(psObjSeek)

                              // 1ルームに存在できる最大オブジェクト数
                              // maximum number of objects that can exist in one room
                              || psObjSeek->dwOfs > 0xFF))
        return FALSE;

    return TRUE;
}

DWORD BadGuysIsPageRelatedObject(LPBYTE lpbBuf) {
    if (lpbBuf[1] & 0x80) return PAGEOBJECT_NEXTPAGEFLAG;
    if ((lpbBuf[0] & 0x0F) == 0x0F) return PAGEOBJECT_SETPAGE;
    return PAGEOBJECT_NO;
}

/**********
   マップ
   map
***********/

// ページの処理（補助関数）
// Processing of pages (auxiliary function)
static void MapSetPage(OBJECTSEEKINFO* psObjSeek) {
    if (!psObjSeek) return;

    if (((psObjSeek->pbData[1]) & 0x80)) {
        psObjSeek->dwPage++;
    } else if ((((psObjSeek->pbData[0]) & 0x0F) == 0x0D) &&
               (!((psObjSeek->pbData[1]) & 0x40))) {
        psObjSeek->dwPage = ((psObjSeek->pbData[1]) & 0x3F);
    }
}

// ルーム内のデータならTRUE、ルーム外のデータならFALSEを返す
// Returns TRUE if the data is in the room, FALSE if it is outside the room
static BOOL MapCheckDataLength(OBJECTSEEKINFO* psObjSeek) {
    if (!psObjSeek) return FALSE;

    if ((psObjSeek->dwLength) < (psObjSeek->dwOfs) + 2) return FALSE;

    return TRUE;
}

// 検索するためのデータを初期化する
// uRoomIDには、取得したいルームのデータを指定。
// GETADDRESS_CURRENT_EDITTINGを指定すると現在エディトしているルーム
// Initialize data for searching
// Specify the data of the room you want to obtain for uRoomID.
// If GETADDRESS_CURRENT_EDITTING is specified, the room currently being edited
BOOL MapSeekFirst(OBJECTSEEKINFO* psObjSeek, UINT uRoomID) {
    if (!psObjSeek) return FALSE;

    // 構造体を初期化
    // Initialize structure
    memset(psObjSeek, 0, sizeof(OBJECTSEEKINFO));

    // +2は、ヘッダ分
    // + 2 is the header part
    psObjSeek->pbData = bPRGROM + GetMapAddress(uRoomID) + 2;

    if (psObjSeek->pbData[0] == 0xFD) return FALSE;

    // ページの情報をセット
    // Set page information
    MapSetPage(psObjSeek);

    // 長さの情報をセット
    // Set length information
    psObjSeek->dwLength = MapGetAllDataLength(uRoomID);

    // オブジェクトのサイズ
    // Size of object
    psObjSeek->dwObjLen = 2;

    if (g_blIsLengthValid && !MapCheckDataLength(psObjSeek)) return FALSE;

    return TRUE;
}

BOOL MapSeekNext(OBJECTSEEKINFO* psObjSeek) {
    if (!psObjSeek) return FALSE;

    psObjSeek->pbData += 2;
    psObjSeek->dwOfs += 2;

    if (psObjSeek->pbData[0] == 0xFD) return FALSE;

    // ページの情報をセット
    // Set page information
    MapSetPage(psObjSeek);

    // インデックスをインクリメント
    // Increment index
    psObjSeek->dwIndex++;

    // オブジェクトのサイズ
    // Size of object
    psObjSeek->dwObjLen = 2;

    if (g_blIsLengthValid && (!MapCheckDataLength(psObjSeek)

                              // 1ルームに存在できる最大オブジェクト数
                              // maximum number of objects that can exist in one room
                              || psObjSeek->dwOfs > 0xFF))
        return FALSE;

    return TRUE;
}

/***********************************

  マップのデータ変更処理関数

  Map data change processing function

************************************/
int GetMapData(UINT uRoomID, int iIndex, BYTE* pbBuf, int* piPage) {
    OBJECTSEEKINFO ObjSeek;

    if (iIndex < 0) return -1;

    if (!MapSeekFirst(&ObjSeek, uRoomID)) return -1;
    for (;;) {
        if (ObjSeek.dwIndex == (DWORD)iIndex) break;
        if (!MapSeekNext(&ObjSeek)) return -1;
    }

    if (pbBuf && sizeof(pbBuf) >= 2) memcpy(pbBuf, ObjSeek.pbData, 2);

    if (piPage) *piPage = ObjSeek.dwPage;

    return ObjSeek.dwOfs;
}

void GetMapHeadData(UINT uRoomID, BYTE* pbBuf) {
    if (!pbBuf || sizeof(pbBuf) < 2) return;

    memcpy(pbBuf, bPRGROM + GetMapAddress(uRoomID), 2);
}

BOOL SortByPosXMap(UINT uRoomID, int* piCurIndex, BOOL IsResort) {
    BYTE* pbData;
    BOOL blRet;
    UINT uPageBaseOfs;

    // 先頭からの基準インデックス
    // Base index from the beginning
    int iIndexBase;

    // 基準からのインデックス
    // Index from reference
    int iIndex;
    BOOL blReSortNeed;
    DWORD dwLength;

    blReSortNeed = FALSE;
    blRet = FALSE;
    iIndexBase = 0;
    iIndex = 0;
    uPageBaseOfs = 0;
    dwLength = MapGetAllDataLength(uRoomID);
    pbData = bPRGROM + GetMapAddress(uRoomID);
    pbData += 2;
    for (;;) {
        BYTE* pbPageBase;
        int iPageBaseNum = 0;

        pbPageBase = pbData;

        // １ページ内の検索
        // Search within one page
        for (;;) {
            int n = 0;
            for (;;) {
                if ((pbPageBase[iPageBaseNum] & 0xF0) >
                    (pbPageBase[iPageBaseNum + n] & 0xF0)) {
                    BYTE bTmp[2];

                    if ((pbPageBase[iPageBaseNum] & 0x0F) == 0x0D &&
                        (pbPageBase[iPageBaseNum + 1] & 0x70) == 0x00)
                        blReSortNeed = TRUE;

                    // 改ページフラグ
                    // Page break flag
                    if (pbPageBase[iPageBaseNum + 1] & 0x80) {
                        pbPageBase[iPageBaseNum + n + 1] |= 0x80;
                        pbPageBase[iPageBaseNum + 1] &= 0x7F;
                    }
                    memcpy(bTmp, pbPageBase + iPageBaseNum, 2);
                    memcpy(pbPageBase + iPageBaseNum, pbPageBase + iPageBaseNum + n, 2);
                    memcpy(pbPageBase + iPageBaseNum + n, bTmp, 2);

                    // ﾃﾞｰﾀが変わった
                    // Data changed
                    blRet = TRUE;

                    // インデックスの処理
                    // Processing indexes
                    if (piCurIndex) {
                        if ((iIndexBase + iIndex) == *piCurIndex) {
                            *piCurIndex = iIndexBase;
                        } else if (iIndexBase == *piCurIndex) {
                            *piCurIndex = iIndexBase + iIndex;
                        }
                    }
                }

                n += 2;
                iIndex++;
                if ((pbPageBase[iPageBaseNum + n + 1] & 0x80) ||
                    ((pbPageBase[iPageBaseNum + n] & 0x0F) == 0x0D &&
                     (pbPageBase[iPageBaseNum + n + 1] & 0x70) == 0x00) ||
                    (pbPageBase[iPageBaseNum + n] == 0xFD) ||
                    (g_blIsLengthValid && ((DWORD)(uPageBaseOfs + n + 2) >= dwLength)))
                    break;
            }

            iPageBaseNum += 2;
            uPageBaseOfs += 2;
            iIndexBase++;
            iIndex = 0;

            if ((pbPageBase[iPageBaseNum + 1] & 0x80) ||
                ((pbPageBase[iPageBaseNum] & 0x0F) == 0x0D &&
                 (pbPageBase[iPageBaseNum + 1] & 0x70) == 0x00) ||
                (pbPageBase[iPageBaseNum] == 0xFD) ||
                (g_blIsLengthValid && ((DWORD)uPageBaseOfs + 2 >= dwLength)))
                break;
        }

        pbData += iPageBaseNum;

        if (pbData[0] == 0xFD) break;
    }

    if (!IsResort && blReSortNeed) {
        blRet = SortByPosXMap(uRoomID, piCurIndex, TRUE);
    }

    return blRet;
}

BOOL SetMapData(UINT uRoomID, int iIndex, BYTE* bBuf) {
    OBJECTSEEKINFO ObjSeek;

    if (sizeof(bBuf) < 2 || iIndex < 0) return FALSE;

    if (bBuf[0] == 0xFD) return FALSE;

    if (!MapSeekFirst(&ObjSeek, uRoomID)) return FALSE;
    for (;;) {
        if (ObjSeek.dwIndex == (DWORD)iIndex) break;
        if (!MapSeekNext(&ObjSeek)) return FALSE;
    }

    memcpy(ObjSeek.pbData, bBuf, 2);

    fr_SetDataChanged(TRUE);

    {
        int iPage;
        BYTE bTmp[2];

        SortByPosXMap(uRoomID, &iIndex, FALSE);

        SetSelectedItem(iIndex, FALSE);
        GetMapData(uRoomID, iIndex, bTmp, &iPage);

        // カーソルの更新
        // Update cursor
        SetMapViewCursoleMap(bTmp, iPage);
    }

    return TRUE;
}

BOOL SetMapDataBinary(UINT uRoomID, int iIndex, BYTE* bBuf, int iValidSize) {
    OBJECTSEEKINFO ObjSeek;

    if (sizeof(bBuf) < 2 || iIndex < 0) return FALSE;

    if (!MapSeekFirst(&ObjSeek, uRoomID)) return FALSE;

    for (;;) {
        if (ObjSeek.dwIndex == (DWORD)iIndex) break;
        if (!MapSeekNext(&ObjSeek)) return FALSE;
    }

    if (g_blIsLengthValid && (ObjSeek.dwLength < ObjSeek.dwOfs + iValidSize))
        return FALSE;

    memcpy(ObjSeek.pbData, bBuf, iValidSize);

    fr_SetDataChanged(TRUE);

    {
        int iPage;
        BYTE bTmp[2];

        SortByPosXMap(GETADDRESS_CURRENT_EDITTING, &iIndex, FALSE);

        SetSelectedItem(iIndex, FALSE);
        GetMapData(GETADDRESS_CURRENT_EDITTING, iIndex, bTmp, &iPage);

        // カーソルの更新
        // Update cursor
        SetMapViewCursoleMap(bTmp, iPage);
    }

    return TRUE;
}

/******

  blPageOverDecは、指定したﾍﾟｰｼﾞを超えてしまった場合に、インデックスを１つ減らすか
  TRUE…減らす FALSE …　減らさない

  If blPageOverDec exceeds the specified page, reduce the index by one
  TRUE ... decrease FALSE ... do not decrease

*******/
int GetMapDataIndex(UINT uRoomID, GETINDEXINFO* psGetIndex, int iPage,
                    BOOL blPageOverDec) {
    int iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
    OBJECTSEEKINFO ObjSeek;
    BOOL fFound = FALSE;

    if (iPage < 0) iPage = 0;

    if (psGetIndex) {
        psGetIndex->nNumSamePos = 0;
        psGetIndex->iIndex = 0;
    }

    if (MapSeekFirst(&ObjSeek, uRoomID)) {
        for (;;) {
            if (!fFound) iCurIndex++;
            if (ObjSeek.dwPage >= (DWORD)iPage) {
                if (!psGetIndex) {
                    if (ObjSeek.dwPage > (DWORD)iPage && iCurIndex > 0 && blPageOverDec)
                        iCurIndex--;
                    break;
                } else if (ObjSeek.dwPage == (DWORD)iPage) {
                    if ((psGetIndex->dwFlag & GETINDEX_FLAG_XY) &&
                        psGetIndex->x == GetMapXPos(ObjSeek.pbData) &&
                        psGetIndex->y == GetMapYPos(ObjSeek.pbData)) {
                        fFound = TRUE;
                        if (psGetIndex && (++psGetIndex->nNumSamePos > 1)) {
                            if (iCurIndex != GETDATAINDEX_ERROR_PL)
                                psGetIndex->iIndex = iCurIndex;
                            iCurIndex = GETDATAINDEX_ERROR_PL;
                        }
                    }
                }
            }

            if (!MapSeekNext(&ObjSeek)) {
                if (!fFound) {
                    if (psGetIndex && (psGetIndex->dwFlag & GETINDEX_FLAG_END))
                        iCurIndex = iCurIndex;
                    else
                        iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
                }
                break;
            }
        }
    }
    return iCurIndex;
}

/***********************************

  敵のデータ変更処理関数

  Enemy data change processing function

************************************/
int GetBadGuysData(UINT uRoomID, int iIndex, BYTE* bBuf, int* piPage) {
    OBJECTSEEKINFO ObjSeek;

    if (iIndex < 0) return -1;

    if (!BadGuysSeekFirst(&ObjSeek, uRoomID)) return -1;

    for (;;) {
        if (ObjSeek.dwIndex == (DWORD)iIndex) break;
        if (!BadGuysSeekNext(&ObjSeek)) return -1;
    }

    if (sizeof(bBuf) >= 3 && bBuf)
        memcpy(bBuf, ObjSeek.pbData, BadGuysGetDataLength(ObjSeek.pbData));
    if (piPage) *piPage = ObjSeek.dwPage;

    return ObjSeek.dwOfs;
}

static BOOL MemorySwap(BYTE* pbBuf1, int iBuf1Size, BYTE* pbBuf2, int iBuf2Size) {
    // アドレスの小さい方
    // The smaller one
    BYTE* pbBufSmall;
    BYTE* pbTmpSmall;

    // アドレスの大きい方
    // The one with the larger address
    BYTE* pbBufLarge;
    BYTE* pbTmpLarge;

    // アドレスの小さい方のデータのサイズ
    // Size of data with the smaller address
    int iSizeSmall;

    // アドレスの大きい方のデータのサイズ
    // Size of data with larger address
    int iSizeLarge;

    if (pbBuf1 == pbBuf2) return FALSE;

    if (pbBuf1 > pbBuf2) {
        pbBufSmall = pbBuf2;
        iSizeSmall = iBuf2Size;
        pbBufLarge = pbBuf1;
        iSizeLarge = iBuf1Size;
    } else {
        pbBufSmall = pbBuf1;
        iSizeSmall = iBuf1Size;
        pbBufLarge = pbBuf2;
        iSizeLarge = iBuf2Size;
    }

    pbTmpSmall = Malloc(iSizeSmall);
    if (!pbTmpSmall) return FALSE;
    memcpy(pbTmpSmall, pbBufSmall, iSizeSmall);

    pbTmpLarge = Malloc(iSizeLarge);
    if (!pbTmpSmall) {
        Mfree(pbTmpSmall);
        return FALSE;
    }
    memcpy(pbTmpLarge, pbBufLarge, iSizeLarge);

    memmove(pbBufSmall + iSizeLarge, pbBufSmall + iSizeSmall,
            pbBufLarge - (pbBufSmall + iSizeSmall));

    memcpy(pbBufSmall, pbTmpLarge, iSizeLarge);
    memcpy(pbBufLarge + iSizeLarge - iSizeSmall, pbTmpSmall, iSizeSmall);

    Mfree(pbTmpSmall);
    Mfree(pbTmpLarge);

    return TRUE;
}

/***************************************************
並べ替えが起こったらTRUE、そうでなければFALSEを返す。
Returns TRUE if a sort occurs, FALSE otherwise.
****************************************************/
BOOL SortByPosXBadGuys(UINT uRoomID, int* piCurIndex, BOOL blIsResort) {
    BYTE* pbData;
    BOOL blRet;
    UINT uPageBaseOfs;

    // 先頭からの基準インデックス
    // Base index from the beginning
    int iIndexBase;

    // 基準からのインデックス
    // Index from reference
    int iIndex;
    BOOL blReSortNeed;
    DWORD dwLength;

    blReSortNeed = FALSE;
    blRet = FALSE;
    pbData = bPRGROM + GetBadGuysAddress(uRoomID);
    dwLength = BadGuysGetAllDataLength(uRoomID);
    iIndexBase = 0;
    iIndex = 0;
    uPageBaseOfs = 0;
    for (;;) {
        BYTE* pbPageBase;
        int iPageBaseNum = 0;

        pbPageBase = pbData;

        // １ページ内のソート
        // Sort within one page
        for (;;) {
            int n = 0;
            for (;;) {
                if ((pbPageBase[iPageBaseNum + n] & 0x0F) != 0x0E)
                    n += 2;
                else
                    n += 3;

                iIndex++;

                if ((pbPageBase[iPageBaseNum + n + 1] & 0x80) ||
                    (pbPageBase[iPageBaseNum + n] == 0xFF) ||
                    ((pbPageBase[iPageBaseNum + n] & 0x0F) == 0x0F) ||
                    (g_blIsLengthValid &&
                     ((DWORD)(uPageBaseOfs + n +
                              BadGuysGetDataLength(&pbPageBase[iPageBaseNum])) >
                      dwLength)))
                    break;

                // スワップが必要なら行う
                // Do swapping if necessary
                if ((pbPageBase[iPageBaseNum] & 0xF0) >
                    (pbPageBase[iPageBaseNum + n] & 0xF0)) {
                    BYTE bTmp[3];
                    int iData1Size;
                    int iData2Size;

                    // ページのベースが送りコマンドで、それと何かをスワップした場合、再ソートが必要
                    // If the base of the page is a forward command and you swap
                    // something with it, you will need resort
                    if ((pbPageBase[iPageBaseNum] & 0x0F) == 0x0F) blReSortNeed = TRUE;

                    iData1Size = ((pbPageBase[iPageBaseNum] & 0x0F) == 0x0E) ? 3 : 2;
                    iData2Size =
                            ((pbPageBase[iPageBaseNum + n] & 0x0F) == 0x0E) ? 3 : 2;
                    if (pbPageBase[iPageBaseNum + 1] & 0x80) {
                        pbPageBase[iPageBaseNum + 1] &= 0x7F;
                        pbPageBase[iPageBaseNum + n + 1] |= 0x80;
                    }

                    if (iData1Size == iData2Size) {
                        memcpy(bTmp, pbPageBase + iPageBaseNum, iData1Size);
                        memcpy(pbPageBase + iPageBaseNum, pbPageBase + iPageBaseNum + n,
                               iData1Size);
                        memcpy(pbPageBase + iPageBaseNum + n, bTmp, iData1Size);
                    } else {
                        MemorySwap(pbPageBase + iPageBaseNum, iData1Size,
                                   pbPageBase + iPageBaseNum + n, iData2Size);
                        n += (iData2Size - iData1Size);
                    }

                    // ﾃﾞｰﾀが変わった
                    // Data changed
                    blRet = TRUE;

                    // インデックスの処理
                    // Processing indexes
                    if (piCurIndex) {
                        if ((iIndexBase + iIndex) == *piCurIndex) {
                            *piCurIndex = iIndexBase;
                        } else if (iIndexBase == *piCurIndex) {
                            *piCurIndex = iIndexBase + iIndex;
                        }
                    }
                }
            }

            if ((pbPageBase[iPageBaseNum] & 0x0F) != 0x0E) {
                iPageBaseNum += 2;
                uPageBaseOfs += 2;
            } else {
                iPageBaseNum += 3;
                uPageBaseOfs += 3;
            }
            iIndexBase++;
            iIndex = 0;

            if ((pbPageBase[iPageBaseNum + 1] & 0x80) ||
                (pbPageBase[iPageBaseNum] == 0xFF) ||
                ((pbPageBase[iPageBaseNum] & 0x0F) == 0x0F) ||
                (g_blIsLengthValid &&
                 ((DWORD)(uPageBaseOfs +
                          BadGuysGetDataLength(&pbPageBase[iPageBaseNum])) > dwLength)))
                break;
        }

        pbData += iPageBaseNum;

        if (pbData[0] == 0xFF) break;
    }

    if (!blIsResort && blReSortNeed) {
        blRet = SortByPosXBadGuys(uRoomID, piCurIndex, TRUE);
    }

    return blRet;
}

/*
    注意：　データのサイズの違うオブジェクト（ルーム間移動の命令にクリボーなど）を設定しようとすると失敗する
    Attention: Attempting to set an object with different data size (such as a command
   to move between rooms like Kuribo) fails
*/
BOOL SetBadGuysData(UINT uRoomID, int iIndex, BYTE* bBuf) {
    int iValidSize1, iValidSize2;
    OBJECTSEEKINFO ObjSeek;

    if (sizeof(bBuf) < 3 || !bBuf) return FALSE;

    if (bBuf[0] == 0xFF) return FALSE;

    if (!BadGuysSeekFirst(&ObjSeek, uRoomID)) return FALSE;
    for (;;) {
        if (ObjSeek.dwIndex == (DWORD)iIndex) break;
        if (!BadGuysSeekNext(&ObjSeek)) return FALSE;
    }

    // 入力データのサイズを取得
    // Get size of input data
    iValidSize1 = BadGuysGetDataLength(bBuf);

    // 元のデータのサイズを取得
    // Get original data size
    iValidSize2 = BadGuysGetDataLength(ObjSeek.pbData);
    if (iValidSize1 != iValidSize2) return FALSE;
    memcpy(ObjSeek.pbData, bBuf, iValidSize1);

    fr_SetDataChanged(TRUE);

    {
        int iPage;
        BYTE bTmp[3];

        SortByPosXBadGuys(GETADDRESS_CURRENT_EDITTING, &iIndex, FALSE);

        SetSelectedItem(iIndex, FALSE);
        GetBadGuysData(GETADDRESS_CURRENT_EDITTING, iIndex, bTmp, &iPage);

        // マップビューのカーソルの更新
        // Update cursor in map view
        SetMapViewCursoleBadGuys(bTmp, iPage);
    }

    return TRUE;
}

BOOL SetBadGuysDataBinary(UINT uRoomID, int iIndex, BYTE* bBuf, int iValidSize) {
    OBJECTSEEKINFO ObjSeek;

    if (sizeof(bBuf) < 3 || !bBuf) return FALSE;

    if (!BadGuysSeekFirst(&ObjSeek, uRoomID)) return FALSE;

    for (;;) {
        if (ObjSeek.dwIndex == (DWORD)iIndex) break;
        if (!BadGuysSeekNext(&ObjSeek)) return FALSE;
    }

    if (g_blIsLengthValid && (ObjSeek.dwLength < ObjSeek.dwOfs + iValidSize))
        return FALSE;

    memcpy(ObjSeek.pbData, bBuf, iValidSize);

    fr_SetDataChanged(TRUE);

    // マップビュー、カレントセレクトの更新
    // Map view, update current selection
    {
        int iPage;
        BYTE bTmp[3];

        SortByPosXBadGuys(GETADDRESS_CURRENT_EDITTING, &iIndex, FALSE);

        SetSelectedItem(iIndex, FALSE);
        GetBadGuysData(GETADDRESS_CURRENT_EDITTING, iIndex, bTmp, &iPage);

        SetMapViewCursoleBadGuys(bTmp, iPage);
    }

    return TRUE;
}

int GetBadGuysDataIndex(UINT uRoomID, GETINDEXINFO* psGetIndex, int iPage,
                        BOOL blPageOverDec) {
    int iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
    OBJECTSEEKINFO ObjSeek;
    int x, y;
    BOOL fFound = FALSE;

    if (iPage < 0) iPage = 0;

    if (psGetIndex) {
        psGetIndex->nNumSamePos = 0;
        psGetIndex->iIndex = 0;
    }

    if (BadGuysSeekFirst(&ObjSeek, uRoomID)) {
        for (;;) {
            if (!fFound) iCurIndex++;
            if (!psGetIndex) {
                if (ObjSeek.dwPage >= (DWORD)iPage) {
                    if (ObjSeek.dwPage > (DWORD)iPage && iCurIndex > 0 && blPageOverDec)
                        iCurIndex--;
                    break;
                }
            } else {
                if (psGetIndex->dwFlag & GETINDEX_FLAG_XY) {
                    int iTmpPage;
                    iTmpPage = ObjSeek.dwPage;
                    x = GetBadGuysXPos(ObjSeek.pbData);
                    y = GetBadGuysYPos(ObjSeek.pbData);
                    if (x < 0) {
                        iTmpPage--;
                        x += 16;
                    } else if (x > 15) {
                        iTmpPage++;
                        x -= 16;
                    }
                    if (x == psGetIndex->x && y == psGetIndex->y && iTmpPage == iPage) {
                        fFound = TRUE;
                        if (psGetIndex && (++psGetIndex->nNumSamePos > 1)) {
                            if (iCurIndex != GETDATAINDEX_ERROR_PL)
                                psGetIndex->iIndex = iCurIndex;
                            iCurIndex = GETDATAINDEX_ERROR_PL;
                        }
                    }
                }
            }

            if (!BadGuysSeekNext(&ObjSeek)) {
                if (!fFound) {
                    if (psGetIndex && (psGetIndex->dwFlag & GETINDEX_FLAG_END))
                        iCurIndex = iCurIndex;
                    else
                        iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
                }
                break;
            }
        }
    }
    return iCurIndex;
}

extern SMBBADGUYSINFO* smbBadGuysInfo;
int GetBadGuysYPos(BYTE* pbBuf) {
    int iRet;
    if ((pbBuf[0] & 0x0F) != 0x0E) {
        if ((pbBuf[0] & 0x0F) != 0x0F) {
            if (smbBadGuysInfo[pbBuf[1] & 0x3F].bFixedYPos)
                iRet = smbBadGuysInfo[pbBuf[1] & 0x3F].bFixedYPos;
            else
                iRet = (pbBuf[0] & 0x0F) + smbBadGuysInfo[pbBuf[1] & 0x3F].YDelta;
        } else
            iRet = 0x0F;
    } else {
        iRet = 0x0E;
    }
    return iRet;
}

int GetBadGuysXPos(BYTE* pbBuf) {
    if ((pbBuf[0] & 0x0F) != 0x0E)
        return ((pbBuf[0] >> 4) & 0x0F) + smbBadGuysInfo[pbBuf[1] & 0x3F].XDelta;
    else
        return ((pbBuf[0] >> 4) & 0x0F);
}

int GetMapXPos(BYTE* pbBuf) { return ((pbBuf[0] >> 4) & 0x0F); }
int GetMapYPos(BYTE* pbBuf) { return (pbBuf[0] & 0x0F); }

BOOL BadGuysIsHardObject(BYTE* pbBuf) {
    BYTE bBuf = pbBuf[0] & 0x0F;
    return ((bBuf != 0x0E) && (bBuf != 0x0F) && (pbBuf[1] & 0x40)) ? TRUE : FALSE;
}
