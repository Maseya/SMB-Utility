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

  �e�X�g�v���C�̐ݒ�_�C�A���O�֐�

**********************************/
LRESULT CALLBACK TestPlaySettingDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

/*****************************************************************

BOOL RunEmulatorViewPage(UINT uRoomID,int iPage);

  uRoomID�Ŏw�肳�ꂽ���[����iPage�Ŏw�肳�ꂽ�y�[�W�̃}�b�v�̊G��
  �G�~�����[�^�E�C���h�E�̃o�b�N�o�b�t�@�ɏ�������BuRoomID��
  GETADDRESS_CURRENT_EDITTING�萔���w�肷��ƁA���݃G�f�B�g���Ă���
  ���[�����w�肵�����ƂɂȂ�B

*******************************************************************/
BOOL RunEmulatorViewPage(UINT uRoomID,int iPage);

/***********************************************************************************************************

void TransferFromEmuBackBuffer(HDC hDCDest,int iDestX,int iDestY,int iWidth,int iHeight,BOOL blIsStretch)

  blIsStretch TRUE  iDestX,iDestY,iWidth,iHeight�ւP��ʕ��AStretchBlt()�s��
              FALSE iDestX,iDestY��iWidth,iHeight���ABitBlt()�s��

  �G�~�����[�^�E�C���h�E�̃o�b�N�o�b�t�@�[����hDCDest�Ŏw�肳�ꂽ�f�o�C�X�R���e�L�X�g��BitBlt���s��

************************************************************************************************************/
void TransferFromEmuBackBuffer(HDC hDCDest,int iDestX,int iDestY,int iWidth,int iHeight,BOOL blIsStretch);


void InstallEmulatorPalette(HDC hdc);
/********************************************************************

  void RunEmulatorTestPlay();

  ���݃G�f�B�g���Ă��郋�[���̃e�X�g�v���C�����[���̂͂��߂���s���B

*********************************************************************/
void RunEmulatorTestPlay();

/********************************************************************

  void RunEmulatorTestPlayEx(int iPage);

  ���݃G�f�B�g���Ă��郋�[���̃e�X�g�v���C���w�肳�ꂽ�y�[�W����s��

*********************************************************************/
void RunEmulatorTestPlayEx(int iPage,BOOL blHalfPoint,LPPOINT lpPt);

/*******************************

  void RunEmulatorNormal();

  �G�~�����[�^�̒ʏ�N�����s��

********************************/
void RunEmulatorNormal();

void DemoRecord();

#endif /* EMUUTIL_H */