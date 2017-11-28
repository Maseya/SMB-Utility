/************************************************************************************

                                  smb Utility

  File: keyaccel.c
  Description:
  History:

 ************************************************************************************/

#include "smbutil.h"
#include "ini.h"
#include "keyaccel.h"

BOOL GetPresetEditorKeys(WORD aEditKeys[], DWORD dwID)
{
	WORD wKeys[KEYACCEL_NUM_PRESETS][KEYACCEL_NUM_COMMANDS] =
	{
	  //"種類を+1", "種類を-1", "種類を+16", "種類を-16", "位置を上へ", "位置を下へ", "位置を左へ", "位置を右へ", "次のﾍﾟｰｼﾞへ", "前のﾍﾟｰｼﾞへ", "次のｵﾌﾞｼﾞｪｸﾄへ", "前のｵﾌﾞｼﾞｪｸﾄへ",
		VK_ADD, VK_SUBTRACT, VK_MULTIPLY, VK_DIVIDE, VK_NUMPAD8, VK_NUMPAD2, VK_NUMPAD4, VK_NUMPAD6, VK_NUMPAD9, VK_NUMPAD3, VK_NUMPAD7, VK_NUMPAD1, CTRLBIT | 0x53, CTRLBIT | 0x5A, VK_F2, VK_F3, VK_F4, VK_F9, VK_F10, VK_F11, VK_F12, VK_F5, VK_F7, VK_F8, VK_TAB, SHIFTBIT | VK_TAB,
		0x57,   0x51,        0x52,        0x45,      0x53,       0x58,       0x5A,       0x43,       0x44,       0x41,       0x46,       0x56,       CTRLBIT | 0x53, CTRLBIT | 0x5A, VK_F2, VK_F3, VK_F4, VK_F9, VK_F10, VK_F11, VK_F12, VK_F5, VK_F7, VK_F8, VK_TAB, SHIFTBIT | VK_TAB
	};

	if (dwID >= KEYACCEL_NUM_PRESETS)
		return FALSE;

	memcpy(aEditKeys, wKeys[dwID], KEYACCEL_NUM_COMMANDS * sizeof(WORD));

	return TRUE;
}

BOOL GetEditorVKeys(WORD aEditKeys[])
{
	if(!ReadFromRegistry(INI_EDITOR_KEYS,
						 REG_BINARY,
						 aEditKeys,
						 KEYACCEL_NUM_COMMANDS * sizeof(WORD))){
		GetPresetEditorKeys(aEditKeys, KEYACCEL_PRESETID_DESKTOP);
		return FALSE;
	}
	return TRUE;
}

HACCEL CreateEditCommandAccel()
{
	UINT uCmd[KEYACCEL_NUM_COMMANDS] =
	{
		IDM_EDITCOMMAND_ADD1TOTYPE,
		IDM_EDITCOMMAND_DEC1TOTYPE,
		IDM_EDITCOMMAND_ADD16TOTYPE,
		IDM_EDITCOMMAND_DEC16TOTYPE,
		IDM_EDITCOMMAND_UP,
		IDM_EDITCOMMAND_DOWN,
		IDM_EDITCOMMAND_LEFT,
		IDM_EDITCOMMAND_RIGHT,
		IDM_EDITCOMMAND_FWDPAGE,
		IDM_EDITCOMMAND_REWPAGE,
		IDM_EDITCOMMAND_NEXTOBJ,
		IDM_EDITCOMMAND_PREVOBJ,
		IDM_FILE_SAVE,
		IDM_EDIT_UNDO,
		IDM_SETTING_AREA,
		IDM_SETTING_BADGUYS,
		IDM_SETTING_MAP,
		IDM_EMULATOR_NORMALPLAY,
		IDM_EMULATOR_PAGEPLAY,
		IDM_EMULATOR_PAGEPLAYHALF,
		IDM_EMULATOR_STOP,
		IDM_EMULATOR_SAVE,
		IDM_EMULATOR_LOAD,
		IDM_EMULATOR_TESTPLAYSETTING,
		IDM_WINDOW_NEXT,
		IDM_WINDOW_PREV
	};
	ACCEL accel[KEYACCEL_NUM_COMMANDS];
	WORD wVKeys[KEYACCEL_NUM_COMMANDS];
	UINT N;

	GetEditorVKeys(&wVKeys[0]);

	for(N=0;N<KEYACCEL_NUM_COMMANDS;N++)
	{
		WORD wVKey = wVKeys[N];
		LPACCEL lpAccel = &accel[N];
		//
		lpAccel->cmd = uCmd[N];
		lpAccel->fVirt = FNOINVERT | FVIRTKEY;
		if(GETCTRL(wVKey)) lpAccel->fVirt |= FCONTROL; 
		if(GETALT(wVKey)) lpAccel->fVirt |= FALT;
		if(GETSHIFT(wVKey)) lpAccel->fVirt |= FSHIFT;
		lpAccel->key = GETVKEY(wVKey);
	}

	return CreateAcceleratorTable(accel,KEYACCEL_NUM_COMMANDS);
}
