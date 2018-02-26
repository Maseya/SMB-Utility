/************************************************************************************

                                  smb Utility

  File: emuutil.h
  Description:
  History:

 ************************************************************************************/
#ifndef EMUUTIL_H
#define EMUUTIL_H

#define TESTPLAY_SETTINGS_MAXYPOS 12

void LoadTestPlaySettings();

/*********************************

  テストプレイの設定ダイアログ関数

  Test play setting dialog function

**********************************/
LRESULT CALLBACK TestPlaySettingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

/*****************************************************************

BOOL RunEmulatorViewPage(UINT uRoomID,int iPage);

  uRoomIDで指定されたルームのiPageで指定されたページのマップの絵を
  エミュレータウインドウのバックバッファに準備する。uRoomIDに
  GETADDRESS_CURRENT_EDITTING定数を指定すると、現在エディトしている
  ルームを指定したことになる。

  Picture of the map of the page specified by iPage in the room designated by uRoomID
  Prepare in the back buffer of the emulator window. to uRoomID
  If you specify the GETADDRESS_CURRENT_EDITTING constant, you are currently editing
  You have designated a room.

*******************************************************************/
BOOL RunEmulatorViewPage(UINT uRoomID, int iPage);

/***********************************************************************************************************

void TransferFromEmuBackBuffer(HDC hDCDest,int iDestX,int iDestY,int iWidth,int iHeight,BOOL blIsStretch)

  blIsStretch TRUE  iDestX,iDestY,iWidth,iHeightへ１画面分、StretchBlt()行う
              FALSE iDestX,iDestYへiWidth,iHeight分、BitBlt()行う

  エミュレータウインドウのバックバッファーからhDCDestで指定されたデバイスコンテキストへBitBltを行う

  blIsStretch TRUE Perform one StretchBlt() for iDestX, iDestY, iWidth, iHeight
  FALSE iDestX, to iDestY iWidth, iHeight minutes, BitBlt() done

  BitBlt from the back buffer of the emulator window to the device context specified by hDCDest

************************************************************************************************************/
void TransferFromEmuBackBuffer(HDC hDCDest, int iDestX, int iDestY, int iWidth, int iHeight, BOOL blIsStretch);

void InstallEmulatorPalette(HDC hdc);
/********************************************************************

  void RunEmulatorTestPlay();

  現在エディトしているルームのテストプレイをルームのはじめから行う。

  Perform a test play of the room you are currently editing from the beginning of the room.

*********************************************************************/
void RunEmulatorTestPlay();

/********************************************************************

  void RunEmulatorTestPlayEx(int iPage);

  現在エディトしているルームのテストプレイを指定されたページから行う

  Perform the test play of the room currently edited from the specified page

*********************************************************************/
void RunEmulatorTestPlayEx(int iPage, BOOL blHalfPoint, LPPOINT lpPt);

/*******************************

  void RunEmulatorNormal();

  エミュレータの通常起動を行う

  Perform normal startup of the emulator

********************************/
void RunEmulatorNormal();

void DemoRecord();

#endif /* EMUUTIL_H */
