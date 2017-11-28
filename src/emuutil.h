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

**********************************/
LRESULT CALLBACK TestPlaySettingDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

/*****************************************************************

BOOL RunEmulatorViewPage(UINT uRoomID,int iPage);

  uRoomIDで指定されたルームのiPageで指定されたページのマップの絵を
  エミュレータウインドウのバックバッファに準備する。uRoomIDに
  GETADDRESS_CURRENT_EDITTING定数を指定すると、現在エディトしている
  ルームを指定したことになる。

*******************************************************************/
BOOL RunEmulatorViewPage(UINT uRoomID,int iPage);

/***********************************************************************************************************

void TransferFromEmuBackBuffer(HDC hDCDest,int iDestX,int iDestY,int iWidth,int iHeight,BOOL blIsStretch)

  blIsStretch TRUE  iDestX,iDestY,iWidth,iHeightへ１画面分、StretchBlt()行う
              FALSE iDestX,iDestYへiWidth,iHeight分、BitBlt()行う

  エミュレータウインドウのバックバッファーからhDCDestで指定されたデバイスコンテキストへBitBltを行う

************************************************************************************************************/
void TransferFromEmuBackBuffer(HDC hDCDest,int iDestX,int iDestY,int iWidth,int iHeight,BOOL blIsStretch);


void InstallEmulatorPalette(HDC hdc);
/********************************************************************

  void RunEmulatorTestPlay();

  現在エディトしているルームのテストプレイをルームのはじめから行う。

*********************************************************************/
void RunEmulatorTestPlay();

/********************************************************************

  void RunEmulatorTestPlayEx(int iPage);

  現在エディトしているルームのテストプレイを指定されたページから行う

*********************************************************************/
void RunEmulatorTestPlayEx(int iPage,BOOL blHalfPoint,LPPOINT lpPt);

/*******************************

  void RunEmulatorNormal();

  エミュレータの通常起動を行う

********************************/
void RunEmulatorNormal();

void DemoRecord();

#endif /* EMUUTIL_H */