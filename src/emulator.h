/************************************************************************************

                                  smb Utility

  File: emulator.h
  Description:
  History:

 ************************************************************************************/
#ifndef EMULATOR_H
#define EMULATOR_H

#include "M6502.h"

typedef struct
{
	BYTE *pbPRGROM;
	BYTE *pbCHRROM;
	BYTE *pbTRAINER;
	BYTE *pbVRAM;
	struct
	{
		BOOL blValid;
		struct
		{
			WORD wAF;
			WORD wPC;
			BYTE wX;
			BYTE wY;
			BYTE wS;
		}Reg;
	}CPUReg;
}EMULATORSETUP;

typedef struct
{
	union 
	{
		struct{
			BYTE bLower;
			BYTE bUpper;
		}byte;
		WORD word;
	}BadGuysAddress;
	union 
	{
		struct{
			BYTE bLower;
			BYTE bUpper;
		}byte;
		WORD word;
	}MapAddress;
	BYTE bRoomID;
	BYTE bPage;
	BYTE bBackView;
	BYTE bBasicBlock;
	BYTE bBackObject1;//$741
	BYTE bBackObject2;//$744
	BYTE bLeftObjOfs[3];
	BYTE bLeftObjNum[3];
	BYTE bLeftObjData1;//�K�i$734
	BYTE bLeftObjData2[3];//�L�m�R�̓�$737
	BYTE bMapOfs;//$072C
	BYTE bMapPage;//$072A
	BYTE bMapPageFlag;//$072B
	BYTE bBadGuysOfs;//$0739
	BYTE bBadGuysPage;//$073A
	BYTE bBadGuysPageFlag;//$073B
	BYTE bBadGuysPage2;//$071B
	BYTE bWorld;
	BYTE bArea; //$075C �ʏ�̂���
	BYTE bArea2;//$0760 �����ʂ��P�̃G���A�ɐ�����
	BYTE bIsCleared;//0 -NO, 1 -YES
	BYTE bIsDifficult;
	BYTE bMarioSize;//$0756 0-large 1-small
	BYTE bMarioCap;//$0754 0-normal 1-super 2-fire
	BOOL blIsStatusDraw;
}TESTPLAYSETUP;

typedef struct
{
	BYTE bRoomID;
	BYTE bPage;
	BYTE bWorld;
	BYTE bArea;//$075C �ʏ�̂���
	BYTE bArea2;//$0760 �����ʂ��P�̃G���A�ɐ�����
	BYTE bIsCleared;//0 -NO, 1 -YES
	BYTE bIsDifficult;
	BYTE bMarioSize;//$0756 0-large 1-small
	BYTE bMarioCap;//$0754 0-normal 1-super 2-fire
	BYTE bBadGuyHack;
	BYTE fPosXHack;
	BYTE bPosX;
	BYTE fPosYHack;
	BYTE bPosY;
	BYTE bInvincible;
}TESTPLAYSETUPEX;

typedef struct _tagNESCONTEXT
{
	//
	CONTEXTM6502 *psM6502;
	//
	BYTE *pb6502CPUMemory;
	//PPU Reg
	BYTE bPPUCtrlReg1;
	BYTE bPPUCtrlReg2;
	BYTE bPPUStaReg;
	//back ground scroll
	BOOL blBGIsVertical;
	BYTE bBGScrlH;
	BYTE bBGScrlV;
	//vram
	WORD wVRAMAddr;
	BYTE *pbVRAM;
	BOOL blVRAMFirstRead;
	BYTE bBGColor;
	//sprite
	BYTE bSPRRAMAddr;
	BYTE *pbSPRRAM;
	//
	BYTE bJoy1Read;
	BYTE bJoy2Read;
	//current scanline
	WORD wScanline;
}NESCONTEXT,FAR *LPNESCONTEXT;

//
BOOL RegisterEmuWndClass(HINSTANCE hInstance);
HWND CreateEmulatorWnd(HINSTANCE hInstance,HWND hWndMDIClient);
BOOL PrepareVROMData(BYTE *pbSource);
//�Эڰ� ����޳�̊�{����
void StartEmulator(void);
void StopEmulator(void);
BOOL SuspendEmulator(BOOL blState);//blState == TRUE ->suspend, blState == FALSE ->start
void ResetEmulator(void);
BOOL SetupEmulator(EMULATORSETUP* psEmuSetup);

void ClearEmuBackBuffer();

void SetEmuBackBufferPallete(LPBYTE lpPalData);

//HPALETTE GetEmulatorPalette();
//BOOL RestoreEmulatorPalette(HDC hdc);


BOOL IsEmulatorRunning();

BOOL IsEmulatorSavePresent();
void FreeEmulatorSaveBuffer();
BOOL SaveEmulatorState();
BOOL LoadEmulatorState(EMULATORSETUP* psEmuSetup);
//
void DrawAllScanline();
void Run6502Ex(int iFrames);

HWND GetEmuWndHandle(void);

void TestPlaySetup(TESTPLAYSETUP *psTPS);
void TestPlaySetupEx(TESTPLAYSETUPEX *psTPS);
void DirectWriteToEmulatorRAM(WORD wAddr,LPBYTE lpBuf,WORD wSize);

LRESULT CALLBACK EmulatorOptionDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

BOOL LoadEmuKeySetting();

// �L�[�{�[�h�̃{�^�� WORD�^�̗v�f��EMULATOR_NUM_BUTTONS�̔z��
BOOL GetEmulatorVKeys(WORD aEmuKeys[]);
BOOL GetDefaultEmulatorKeys(WORD aEmuKeys[]);
// �W���C�X�e�B�b�N�̃{�^�� DWORD�^�̗v�f��EMULATOR_NUM_JOYBUTTONS�̔z��
BOOL GetEmulatorJoyButtons(DWORD aEmuJoyButtons[]);


// �����L�[���܂߂��{�^����
#define EMULATOR_NUM_BUTTONS 8
// �����L�[���������W���C�X�e�B�b�N�Ń{�^���̓��͂Ƃ��Ď󂯎��A, B, Select, Start��4��
#define EMULATOR_NUM_JOYBUTTONS 4
// Win32 API joyGetPosEx()�֐��Ŏ擾�\�ȃ{�^���̍ő吔
#define JOYSTICK_MAX_BUTTONS 32

//gblDemoRecord��TRUE�̏ꍇ�A�W���C�X�e�C�b�N�̃��[�`������Ăяo�����B
//emuutil.c�Ɏ�������Ă���B
void DemoRecorderHandler(BYTE bJoy1Read,BYTE bRet);

#define EMULATOR_NES_COLORS 64

#define NES_SCREENSIZEX 256
#define NES_SCREENSIZEY 240

#define WM_UPDATEFRAME (WM_USER + 1)

#endif /* EMULATOR_H */
