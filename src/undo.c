/************************************************************************************

                                  smb Utility

  File: undo.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "roommng.h"
#include "objview.h"
#include "objlist.h"
#include "emulator.h"

INESHEADER Head_undo;
BYTE bPRGROM_undo[PRG_SIZE];
BYTE bCHRROM_undo[SMB_CHR_SIZE];
int iTrainer_undo=0;

#define UNDONAME_BUFFER_SIZE 50
TCHAR g_tcUndoName[UNDONAME_BUFFER_SIZE]={0};
BOOL g_blUndoEnable=FALSE;

//キー入力に対してUNDOのためのコピーを保存するか
BOOL g_blKeyCommandUndo=TRUE;

void undoPrepare(LPCTSTR lpUndoName)
{
	//PRG
	memcpy(bPRGROM_undo, bPRGROM, PRG_SIZE);
	//CHR
	memcpy(bCHRROM_undo, bCHRROM, SMB_CHR_SIZE);
	//Trainer Present?
	iTrainer_undo=iTrainer;
	// store undo name
	wsprintf(g_tcUndoName,"%s",lpUndoName);
	//
	g_blUndoEnable=TRUE;
}

void undoRestore()
{
	if(g_blUndoEnable)
	{
		//PRG
		memcpy(bPRGROM, bPRGROM_undo, PRG_SIZE);
		//CHR
		memcpy(bCHRROM, bCHRROM_undo, SMB_CHR_SIZE);
		//Trainer Present?
		iTrainer=iTrainer_undo;
		//
		g_tcUndoName[0]='\0';
		//
		g_blUndoEnable=FALSE;
		//
		g_blKeyCommandUndo=TRUE;
		//
		rm_UpdateGlobalRoomData();

		//キャラロムの前処理
		PrepareVROMData(bCHRROM);

		if(!UpdateObjectViewCursole()){
			EnsureMapViewCursoleVisible();
			OpenNewRoomProcess();
		}

		UpdateStatusBarRoomInfoText(NULL);

		UpdateObjectView(0);
		UpdateObjectList(0);

		fr_SetDataChanged(TRUE);
	}
}

void undoReset()
{
	g_blUndoEnable=FALSE;
	g_tcUndoName[0]='\0';
	g_blKeyCommandUndo=TRUE;
}

BOOL undoIsEnabled()
{
	return (gblIsROMLoaded)?g_blUndoEnable:FALSE;
}

LPTSTR undoGetNameBuffer()
{
	return g_tcUndoName;
}