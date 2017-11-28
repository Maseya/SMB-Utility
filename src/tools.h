/************************************************************************************

                                  smb Utility

  File: tools.h
  Description:
  History:

 ************************************************************************************/
#ifndef TOOLS_H
#define TOOLS_H

#define SMBADDRESS_LOOP_WORLD            0xC06B
#define SMBADDRESS_LOOP_PAGE             0xC076
#define SMBADDRESS_LOOP_YPOS             0xC081
#define SMBADDRESS_LOOP_RETURNPOS        0x9BF8
#define SMBADDRESS_LOOP_W7DATA1          0xC0FC
#define SMBADDRESS_LOOP_W7DATA2          0xC119
//
#define SMB_MARIO_LEFT                   0x906A
#define SMB_FLOWER                       0x98F5 //5バイト
#define SMB_POLEGFX                      0x8C60 //4バイト
//
#define SMB_WARPZONE_WORLD_ADDRESS       0x87F2
//
#define SMB_TIME                         0x912E
//
#define SMB_COINSFOR1UP_ADDRESS          0xB30E
//
#define SMB_KOOPAREALCHARCTER_ADDRESS    0xD771
#define SMB_KOOPAHAMMER                  0xD11B
//
#define SMB_DIFFICULTYWORLD              0x903B
#define SMB_DIFFICULTYAREA               0x9044
#define SMB_SEABLOCKWORLD                0x9476

LRESULT CALLBACK StringEditDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK LoopEditDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
//LRESULT CALLBACK LoopWizardDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK GameSettingDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
void GameSettingPropertySheet(HWND hwndOwner);

#endif /* TOOLS_H */