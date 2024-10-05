/**********************************************************************

                                  smb Utility

  File: objmng.c
  Description:
  History:

 *********************************************************************/
#include "objmng.h"

#include "objlib.h"
#include "roommng.h"
#include "smbutil.h"

// 現在エディトしているオブジェクトの種類を保存している
// 0はマップ、 1は敵を示す。
// I am saving the type of object I am currently editing
// 0 indicates a map, 1 indicates an enemy.
int giEditMode = 0;

// 現在エディトしているオブジェクトのインデックスを保存
// Save the index of the currently edited object
int giSelectedItem = 0;

void SetSelectedItem(int iItem, BOOL blInitKeyUndo) {
    if (giSelectedItem == iItem || iItem < 0) return;

    giSelectedItem = iItem;

    if (blInitKeyUndo) g_blKeyCommandUndo = TRUE;
}

/********************************************

  エデイットの対象（マップ<->敵）を切り替える

  iMode CHANGEEDITMODE_SWITCHING
…　現在のモードとは逆のモードにする（敵→マップ、マップ→敵）
        CHANGEEDITMODE_MAP　…　マップ
        CHANGEEDITMODE_BADGUYS　…　敵
        CHANGEEDITMODE_BADGUYS …　モードの変更無し

  Switch the target of Edited (map <-> enemy)

  iMode CHANGEEDITMODE_SWITCHING ... Make the mode opposite to the current mode (enemy →
map, map → enemy) CHANGEEDITMODE_MAP ... map CHANGEEDITMODE_BADGUYS ... Enemy
        CHANGEEDITMODE_BADGUYS ... No change in mode

**********************************************/
void ChangeMapEditMode(int iMode, BOOL blInitSelect) {
    int iPage;
    BYTE bBuf[3] = {0};
    GETINDEXINFO gii = {0};

    if (iMode == CHANGEEDITMODE_SWITCHING)
        giEditMode = (giEditMode) ? EDITMODE_MAP : EDITMODE_BADGUYS;
    else if (iMode == CHANGEEDITMODE_MAP || CHANGEEDITMODE_BADGUYS == 1)
        giEditMode = iMode;

    if (!gblIsROMLoaded) return;

    // 新しいインデックスの設定
    // Setting new index
    if (blInitSelect) {
        // 初期化
        // Initialization
        SetSelectedItem(0, TRUE);
    } else {
        if (giEditMode == CHANGEEDITMODE_BADGUYS) {
            // 敵への切り替えーﾏｯﾌﾟで選択されていたﾍﾟｰｼﾞを選択
            // Switch to enemy Select page selected by mime
            int iNewIndex;
            if (GETDATAINDEX_ERROR_NOTFOUND !=
                GetMapData(GETADDRESS_CURRENT_EDITTING, giSelectedItem, NULL, &iPage)) {
                iNewIndex = GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING, NULL,
                                                iPage, TRUE);
                if (iNewIndex == GETDATAINDEX_ERROR_NOTFOUND) {
                    gii.dwFlag = GETINDEX_FLAG_END;
                    iNewIndex = GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING, &gii,
                                                    iPage, TRUE);
                }

                SetSelectedItem(iNewIndex, TRUE);
            }
        } else {
            int iNewIndex;
            if (GETDATAINDEX_ERROR_NOTFOUND !=
                GetBadGuysData(GETADDRESS_CURRENT_EDITTING, giSelectedItem, NULL,
                               &iPage)) {
                iNewIndex =
                        GetMapDataIndex(GETADDRESS_CURRENT_EDITTING, NULL, iPage, TRUE);
                if (iNewIndex == GETDATAINDEX_ERROR_NOTFOUND) {
                    gii.dwFlag = GETINDEX_FLAG_END;
                    iNewIndex = GetMapDataIndex(GETADDRESS_CURRENT_EDITTING, &gii,
                                                iPage, TRUE);
                }

                SetSelectedItem(iNewIndex, TRUE);
            }
        }
    }
}

/***************************************
EDITMODE_MAP=Map EDITMODE_BADGUYS=Bad Guys
****************************************/
int GetMapEditMode() { return giEditMode; }

int GetSelectedIndex() { return giSelectedItem; }
