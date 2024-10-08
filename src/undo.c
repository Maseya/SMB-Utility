﻿/**********************************************************************

                                  smb Utility

  File: undo.c
  Description:
  History:

 *********************************************************************/
#include "emulator.h"
#include "objlist.h"
#include "objview.h"
#include "roommng.h"
#include "smbutil.h"

INESHEADER Head_undo;
BYTE bPRGROM_undo[PRG_SIZE];
BYTE bCHRROM_undo[SMB_CHR_SIZE];
int iTrainer_undo = 0;

#define UNDONAME_BUFFER_SIZE 50
TCHAR g_tcUndoName[UNDONAME_BUFFER_SIZE] = {0};
BOOL g_blUndoEnable = FALSE;

// キー入力に対してUNDOのためのコピーを保存するか
// Whether to save a copy for UNDO for key input
BOOL g_blKeyCommandUndo = TRUE;

extern BYTE bPRGROM[INES_PRGROMBANKSIZE * SMB_NUM_PRGS + 0x8000];
extern BYTE bCHRROM[INES_CHRROMBANKSIZE * SMB_NUM_CHARS];
extern int iTrainer;
extern BOOL gblIsROMLoaded;

void undoPrepare(LPCTSTR lpUndoName) {
    // PRG
    memcpy(bPRGROM_undo, bPRGROM, PRG_SIZE);

    // CHR
    memcpy(bCHRROM_undo, bCHRROM, SMB_CHR_SIZE);

    // Trainer Present?
    iTrainer_undo = iTrainer;

    // store undo name
    wsprintf(g_tcUndoName, __T("%s"), lpUndoName);

    g_blUndoEnable = TRUE;
}

void undoRestore() {
    if (g_blUndoEnable) {
        // PRG
        memcpy(bPRGROM, bPRGROM_undo, PRG_SIZE);

        // CHR
        memcpy(bCHRROM, bCHRROM_undo, SMB_CHR_SIZE);

        // Trainer Present?
        iTrainer = iTrainer_undo;

        g_tcUndoName[0] = __T('\0');
        g_blUndoEnable = FALSE;
        g_blKeyCommandUndo = TRUE;
        rm_UpdateGlobalRoomData();

        // キャラロムの前処理
        // Pre-processing of CHR ROM
        PrepareVROMData(bCHRROM);

        if (!UpdateObjectViewCursole()) {
            EnsureMapViewCursoleVisible();
            OpenNewRoomProcess();
        }

        UpdateStatusBarRoomInfoText(NULL);

        UpdateObjectView(0);
        UpdateObjectList(0);

        fr_SetDataChanged(TRUE);
    }
}

void undoReset() {
    g_blUndoEnable = FALSE;
    g_tcUndoName[0] = __T('\0');
    g_blKeyCommandUndo = TRUE;
}

BOOL undoIsEnabled() { return (gblIsROMLoaded) ? g_blUndoEnable : FALSE; }

LPTSTR undoGetNameBuffer() { return g_tcUndoName; }
