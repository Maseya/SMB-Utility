/************************************************************************************

                                  smb Utility

  File: keyaccel.h
  Description:
  History:

 ************************************************************************************/
#ifndef KEYACCEL_H
#define KEYACCEL_H

#define CTRLBIT   0x0100
#define ALTBIT    0x0200
#define SHIFTBIT  0x0400

#define GETVKEY(W)      ((BYTE)( (W) & 0xFF ))
#define GETCTRL(W)      ( (W) & CTRLBIT )
#define GETALT(W)       ( (W) & ALTBIT )
#define GETSHIFT(W)     ( (W) & SHIFTBIT )
#define GETASSISTKEY(W) ( (W) & 0xFF00 )

#define SETVKEY(W,K)  (W) = ((W) & 0xFF00) | ( (K) & 0xFF )
#define SETCTRL(W)   ( (W) | CTRLBIT )
#define SETALT(W)    ( (W) | ALTBIT )
#define SETSHIFT(W)  ( (W) | SHIFTBIT )

#define KEYACCEL_NUM_COMMANDS 26
#define KEYACCEL_NUM_PRESETS  2
#define KEYACCEL_PRESETID_DESKTOP 0
#define KEYACCEL_PRESETID_NOTE    1

BOOL GetPresetEditorKeys(WORD aEditKeys[], DWORD dwID);
BOOL GetEditorVKeys(WORD aEditKeys[]);
HACCEL CreateEditCommandAccel();

#endif