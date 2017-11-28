/************************************************************************************

                                  smb Utility

  File: emuutil.c
  Description: ビューの表示・テストプレイのためのルーチン
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "M6502.h"

#include "emuutil.h"
#include "emulator.h"
#include "objlist.h"
#include "objlib.h"
#include "roommng.h"
#include "emubgset.h"
#include "ini.h"
#include "logview.h"


BOOL g_blUseTestPlayPage=FALSE;
BYTE g_bTestPlayPage=0;
BOOL g_bl0PageBadGuyHack=FALSE;
BYTE gbIsCleared=0;
BOOL g_bInvincible = FALSE;
int giMarioState=0;//0 - normal 1- super mario 2 - fire mario

#define TESTPLAY_STARTPOS_MAXDOTX 240

typedef enum _STARTPOSHACK
{
	NONE = 0,
	POS = 1,
	DOT = 2
}STARTPOSHACK;
STARTPOSHACK g_fUseStartPosXHack = NONE;
int g_bStartPosX = 0;
STARTPOSHACK g_fUseStartPosYHack = NONE;
int g_bStartPosY = 0;

void LoadTestPlaySettings()
{
	UINT uData;

	// HARD
	uData = GetIntegerFromINI(INIFILE_TESTPLAY,INIFILE_TESTPLAY_HARD,0);
	if( !uData || uData==1 )
		gbIsCleared = uData;

	// STATE
	uData = GetIntegerFromINI(INIFILE_TESTPLAY,INIFILE_TESTPLAY_STATE,0);
	if( uData >=0 && uData<=2 )
		giMarioState = uData;

	// BADGUYS
	uData = GetIntegerFromINI(INIFILE_TESTPLAY,INIFILE_TESTPLAY_BADGUYS,0);
	if( !uData || uData==1 )
		g_bl0PageBadGuyHack = uData;

	// INVINCIBLE
	uData = GetIntegerFromINI(INIFILE_TESTPLAY,INIFILE_TESTPLAY_INVINCIBLE,0);
	if( !uData || uData==1 )
		g_bInvincible = uData;

    // PAGE
	uData = GetIntegerFromINI(INIFILE_TESTPLAY,INIFILE_TESTPLAY_PAGE,256);
	if( uData == 256 ){		
		g_blUseTestPlayPage=FALSE;
		g_bTestPlayPage=0;
	}
	else if(uData < 256){
		g_blUseTestPlayPage=TRUE;
		g_bTestPlayPage=uData;
	}

	return;
}

/***********************************



************************************/
static void TestPlay(int iPage, LPPOINT lpPt)
{
	TESTPLAYSETUPEX sTPS;
	EMULATORSETUP es;
//	BYTE bBuf[2];

	//
	StopEmulator();
	ResetEmulator();

	//
	memset(&es,0,sizeof(EMULATORSETUP));
	es.pbPRGROM=bPRGROM+0x8000;
	es.pbCHRROM=bCHRROM;
	if(iTrainer) es.pbTRAINER=bPRGROM+0x7000;

	SetupEmulator(&es);

	//
	memset(&sTPS,0,sizeof(TESTPLAYSETUPEX));

	sTPS.bRoomID=GetRoomID();
	sTPS.bPage=iPage;
	
	sTPS.bWorld=(BYTE)g_iWorld;
	sTPS.bArea=(BYTE)g_iArea;
	sTPS.bArea2=(BYTE)g_iArea2;

	//
	if(gbIsCleared) 
	{
		sTPS.bIsCleared=0x01;//クリボー→メット
		sTPS.bIsDifficult=0x01;//5-3以降
	}

	switch(giMarioState)
	{
	case 1:
		sTPS.bMarioSize=0x00;
		sTPS.bMarioCap=0x01;
		break;
	case 2:
		sTPS.bMarioSize=0x00;
		sTPS.bMarioCap=0x02;
		break;
	default:
		sTPS.bMarioSize=0x01;
		sTPS.bMarioCap=0x00;
		break;
	}

	if(g_bl0PageBadGuyHack)
		sTPS.bBadGuyHack = 1;

	if(g_bInvincible)
		sTPS.bInvincible = 1;

	//
	if (lpPt)
		sTPS.fPosXHack = TRUE;
	else
		sTPS.fPosXHack = g_fUseStartPosXHack;
	if (lpPt)
		sTPS.bPosX = (BYTE)lpPt->x;
	else if (g_fUseStartPosXHack)
		sTPS.bPosX = g_bStartPosX;
	//
	if (lpPt)
		sTPS.fPosYHack = TRUE;
	else
		sTPS.fPosYHack = g_fUseStartPosYHack;

	if (lpPt)
		sTPS.bPosY = (BYTE)lpPt->y;
	else if (g_fUseStartPosYHack)
		sTPS.bPosY = g_bStartPosY;

	TestPlaySetupEx(&sTPS);

	StartEmulator();
}

void RunEmulatorTestPlay()
{
	TestPlay(g_iPage, NULL);
}

void RunEmulatorTestPlayEx(int iPage, BOOL blHalfPoint, LPPOINT lpPt)
{
	if(blHalfPoint)
		iPage = GetHalfPointPage(rm_GetWorld(), rm_GetArea());
	else if(g_blUseTestPlayPage)
		iPage = g_bTestPlayPage;

	TestPlay(iPage, lpPt);
}


/********************************




*********************************/
extern BOOL gblDemoRecord;
//extern BOOL gblQuit;
#define DEMO_BUFFER_SIZE 21
#define DEMO_STATE_ADDRESS 0x8340
#define DEMO_TIMER_ADDRESS 0x8355
static int g_iDemoIndex;
static BYTE g_bDemoJoyState[DEMO_BUFFER_SIZE];
static BYTE g_bDemoJoyTimer[DEMO_BUFFER_SIZE];
static BYTE g_bDemoJoy;

void InitDemoRecorder()
{
	//
	memset(g_bDemoJoyState,0,DEMO_BUFFER_SIZE);
	memset(g_bDemoJoyTimer,0,DEMO_BUFFER_SIZE);
	g_bDemoJoy=0x00;
	g_iDemoIndex=-1;
}

void DemoRecorderHandler(BYTE bJoy1Read, BYTE bRet)
{
	if(bRet&1) g_bDemoJoy |= (0x80>>bJoy1Read);

	if(bJoy1Read != 0x07) return;

	if(g_iDemoIndex >= (int)(DEMO_BUFFER_SIZE - 1) /*|| ((g_iDemoIndex!=-1) && !g_bDemoJoy)*/){
		//
		gblDemoRecord = FALSE;
		//
		SetStatusBarText(STRING_CONFIRM_DEMORECORD2);
		MessageBeep(MB_OK);
		//
//		gblDataChanged = TRUE;
		fr_SetDataChanged(TRUE);
		undoPrepare(UNDONAME_TOOLDEMORECORDER);
		//
		g_bDemoJoyState[g_iDemoIndex] = 0x00;
		g_bDemoJoyTimer[g_iDemoIndex] = 0xFF;
		//
		memcpy(bPRGROM+DEMO_STATE_ADDRESS, g_bDemoJoyState, DEMO_BUFFER_SIZE);
		memcpy(bPRGROM+DEMO_TIMER_ADDRESS, g_bDemoJoyTimer, DEMO_BUFFER_SIZE);
		//
		return;
	}

	if(!g_bDemoJoy) g_bDemoJoy=0x10;

	if(g_iDemoIndex!=-1){
		if(g_bDemoJoyState[g_iDemoIndex]==g_bDemoJoy && g_bDemoJoyTimer[g_iDemoIndex]!=0xFF)
			g_bDemoJoyTimer[g_iDemoIndex]++;
		else{
			g_bDemoJoyState[++g_iDemoIndex]=g_bDemoJoy;
			g_bDemoJoyTimer[g_iDemoIndex]=0x01;
		}
	}
	else if(g_bDemoJoy){
		g_bDemoJoyState[++g_iDemoIndex]=g_bDemoJoy;
		g_bDemoJoyTimer[g_iDemoIndex]=0x01;
	}
	g_bDemoJoy=0x00;
	
}

void DemoRecord()
{
	TESTPLAYSETUP sTPS;
	EMULATORSETUP es;
	BYTE bMapHead[2];
	BYTE bRoomID;
	
	if(!gblIsROMLoaded) return;

	StopEmulator();
	ResetEmulator();

	memset(&es,0,sizeof(EMULATORSETUP));
	es.pbPRGROM=bPRGROM+0x8000;
	es.pbCHRROM=bCHRROM;
	if(iTrainer) es.pbTRAINER=bPRGROM+0x7000;

	SetupEmulator(&es);

	memset(&sTPS,0,sizeof(TESTPLAYSETUP));

	bRoomID=rm_GetMainRoomID(0);
	
	sTPS.BadGuysAddress.word=GetBadGuysAddress(bRoomID);
	sTPS.MapAddress.word=GetMapAddress(bRoomID);
	sTPS.bRoomID=bRoomID;

	GetMapHeadData(bRoomID,bMapHead);
	if((bMapHead[0]&0x07)&0x04){
		sTPS.bBackObject1=0x00;
		sTPS.bBackObject2=(bMapHead[0]&0x07);	
	}
	else{
		sTPS.bBackObject1=(bMapHead[0]&0x07);
		sTPS.bBackObject2=0x00;
	}//初期背景色

	sTPS.bBasicBlock =(bMapHead[1]&0x0F);//初期基本背景ブロック
	sTPS.bBackView =((bMapHead[1]>>4)&0x03);//初期景色

	sTPS.bMarioSize=0x01;
	sTPS.bMarioCap=0x00;

	memset(sTPS.bLeftObjNum,0xFF,3);
	memset(sTPS.bLeftObjOfs,0x00,3);

	TestPlaySetup(&sTPS);

	gblDemoRecord=TRUE;
	InitDemoRecorder();

	StartEmulator();
}
/*************************************************************

  BOOL RunEmulatorViewPage(UINT uRoomID,int iPage)

  uRoomIDで指定されたルームのiPageで指定されたページのマップの絵を
  エミュレータウインドウのバックバッファに準備する。uRoomIDに
  GETADDRESS_CURRENT_EDITTING定数を指定すると、現在エディトしている
  ルームを指定したことになる。

  ●補助関数●
	static void PrepareMapRelatedMemory(int iPage,int iMapType,MAPRELATEDMEMORY *psMRM)

	マップに関連したメモリーのセットアップを行う。

**************************************************************/
//エミュレータウインドウを操作するためのグローバル変数
extern HDC ghMemdcOffScreen;
extern BOOL gblShowSprite;

typedef struct 
{
	BYTE bBasicDataMask;
	BYTE bBasicData;
	BYTE bIsSizeFixed;
	BYTE bFixedSize;
	BYTE bSizeMask;
	BYTE bExInfo;
}MAPOBJLEFTINFO;

//bExInfoメンバで有効な値
#define EXINFO_STEPS    0x01//階段のための特殊処理
#define EXINFO_ATHLETIC 0x02//アスレチック台とキラーのための特殊処理

MAPOBJLEFTINFO MapObjLeftInfo0B[]={0x7F,0x0C,0x01,0x03,0x00,0x00,//逆L字型土管「固定」
                                   0x70,0x10,0x00,0x00,0x0F,0x02,//アスレチック台（要注意）
								   0x70,0x20,0x00,0x00,0x0F,0x00,//横に並んだレンガブロック
								   0x70,0x30,0x00,0x00,0x0F,0x00,//横に並んだ壊せないブロック
								   0x70,0x40,0x00,0x00,0x0F,0x00,//横に並んだコインブロック
								   0x70,0x70,0x01,0x01,0x00,0x00};//土管「固定」
MAPOBJLEFTINFO MapObjLeftInfoC[]={0x70,0x00,0x00,0x00,0x0F,0x00,//谷
                                  0x70,0x10,0x00,0x00,0x0F,0x00,//天秤リフトの横ロープ
								  0x70,0x20,0x00,0x00,0x0F,0x00,//つり橋
								  0x70,0x30,0x00,0x00,0x0F,0x00,//つり橋
								  0x70,0x40,0x00,0x00,0x0F,0x00,//つり橋
								  0x70,0x50,0x00,0x00,0x0F,0x00,//川
								  0x70,0x60,0x00,0x00,0x0F,0x00,//横に並んだ?ブロック（コイン、高さ3）
								  0x70,0x70,0x00,0x00,0x0F,0x00};//横に並んだ?ブロック（コイン、高さ7）
MAPOBJLEFTINFO MapObjLeftInfoD[]={0x7F,0x40,0x01,0x03,0x00,0x00,
	                              0x7F,0x44,0x01,0x0C,0x00,0x00};//クッパの橋「固定」
MAPOBJLEFTINFO MapObjLeftInfoF[]={0x70,0x20,0x01,0x04,0x00,0x00,//城「固定」
                                  0x78,0x30,0x00,0x00,0x07,0x01,//階段（可変）
								  0x78,0x38,0x01,0x08,0x00,0x01,//階段「固定」
								  0x70,0x40,0x01,0x03,0x00,0x00};//逆L字型

int GetNumMapObjLeftInfoC(){return sizeof(MapObjLeftInfoC)/sizeof(MAPOBJLEFTINFO);};
int GetNumMapObjLeftInfoD(){return sizeof(MapObjLeftInfoD)/sizeof(MAPOBJLEFTINFO);};
int GetNumMapObjLeftInfo0B(){return sizeof(MapObjLeftInfo0B)/sizeof(MAPOBJLEFTINFO);};
int GetNumMapObjLeftInfoF(){return sizeof(MapObjLeftInfoF)/sizeof(MAPOBJLEFTINFO);};

typedef struct 
{
	BYTE bBackView;
	BYTE bBasicBlock;
	BYTE bBackObject1;
	BYTE bBackObject2;
	BYTE bLeftObjNum[3];
	BYTE bLeftObjOfs[3];
	BYTE bLeftObjData1;//階段
	BYTE bLeftObjData2[3];//きのこの島の茎
	BYTE bMapOfs;
	BYTE bMapPageFlag;
}MAPRELATEDMEMORY;


static BOOL PrepareMapRelatedMemory(UINT uRoomID,int iPage,int iMapType,MAPRELATEDMEMORY *psMRM)
{
	MAPOBJLEFTINFO *psMapObjLeftInfo;
	int iLeftDataNum;
	OBJECTSEEKINFO ObjSeek;
	BOOL blRet=FALSE;

	if(!psMRM || iPage<0) return FALSE;

	iLeftDataNum=2;
	memset(psMRM->bLeftObjNum,0xFF,3);
	memset(psMRM->bLeftObjOfs,0x00,3);

	if(!MapSeekFirst(&ObjSeek,uRoomID)) return TRUE;
	{
		//0ページのマップデータがない場合に0ページの表示を要求された場合
		if(iPage==0 && (ObjSeek.pbData[1]&0x80)) return TRUE;
		
		for(;;)
		{
			if(ObjSeek.dwPage>=(DWORD)iPage){
				blRet=TRUE;
				//ページ送りコマンド
				if(((ObjSeek.pbData[0]&0x0F)==0x0D) && ((ObjSeek.pbData[1]&0x40)==0x00)  && ObjSeek.dwPage==(DWORD)iPage)
					psMRM->bMapPageFlag=0x00;
				else
					psMRM->bMapPageFlag=0x01;
				break;
			}

			//
			if((ObjSeek.pbData[0]&0x0F)==0x0E){
				if(ObjSeek.pbData[1]&0x40){
					if((ObjSeek.pbData[1]&0x07)&0x04){
						psMRM->bBackObject1=0x00;
						psMRM->bBackObject2=(ObjSeek.pbData[1]&0x07);
					}
					else{
						psMRM->bBackObject1=(ObjSeek.pbData[1]&0x07);
						//					psMRM->bBackObject2=0x00;
					}
				}
				else{
					psMRM->bBasicBlock =(ObjSeek.pbData[1]&0x0F);//初期基本背景ブロック
					psMRM->bBackView =((ObjSeek.pbData[1]>>4)&0x03);//初期景色
				}
			}
			else if((ObjSeek.dwPage+1)==(DWORD)iPage){
				int iMaxNumObjs;
				int n;
				
				if(iLeftDataNum>=0)
				{
					switch(ObjSeek.pbData[0]&0x0F)
					{
					case 0x0C:psMapObjLeftInfo=MapObjLeftInfoC;iMaxNumObjs=GetNumMapObjLeftInfoC();break;
					case 0x0D:psMapObjLeftInfo=MapObjLeftInfoD;iMaxNumObjs=GetNumMapObjLeftInfoD();break;
					case 0x0F:psMapObjLeftInfo=MapObjLeftInfoF;iMaxNumObjs=GetNumMapObjLeftInfoF();break;
					default:psMapObjLeftInfo=MapObjLeftInfo0B;iMaxNumObjs=GetNumMapObjLeftInfo0B();break;
					}
					for(n=0;n<iMaxNumObjs;n++){
						if((ObjSeek.pbData[1]&psMapObjLeftInfo[n].bBasicDataMask)==psMapObjLeftInfo[n].bBasicData) break;
					}
					
					if(n!=iMaxNumObjs){
						int iBasicSize;
						int iLeftObjNum;
						
						iBasicSize=(psMapObjLeftInfo[n].bIsSizeFixed)?psMapObjLeftInfo[n].bFixedSize:(psMapObjLeftInfo[n].bSizeMask & ObjSeek.pbData[1]);
						iLeftObjNum=(int)(((ObjSeek.pbData[0]>>4)&0x0F)+iBasicSize-0x10);
						
						if(iLeftObjNum>=0 && !((psMapObjLeftInfo[n].bExInfo&EXINFO_ATHLETIC) && iMapType==2)){//アスレチックオブジェクトで、大砲面ではない
							psMRM->bLeftObjNum[iLeftDataNum]=(BYTE)iLeftObjNum;
							psMRM->bLeftObjOfs[iLeftDataNum]=(BYTE)(ObjSeek.dwOfs);
							if(psMapObjLeftInfo[n].bExInfo&EXINFO_STEPS)
								psMRM->bLeftObjData1=0x08-(0x0F-(ObjSeek.pbData[0]>>4));
							if((psMapObjLeftInfo[n].bExInfo&EXINFO_ATHLETIC) && iMapType==1)
								psMRM->bLeftObjData2[iLeftDataNum]=((ObjSeek.pbData[1]&psMapObjLeftInfo[n].bSizeMask)/2);
							iLeftDataNum--;
						}
					}/* if */
				}/* if */
			}/* else if */

			if(!MapSeekNext(&ObjSeek)){

				break;
			}
		}/* for */
	}/* if */

	psMRM->bMapOfs=(BYTE)ObjSeek.dwOfs;

	return blRet;
}

BOOL RunEmulatorViewPage(UINT uRoomID, int iPage)
{
	TESTPLAYSETUP sTPS;
	EMULATORSETUP es;
	BYTE bMapHead[2];
	MAPRELATEDMEMORY sMRM;
	PREPAREBGSPRINFO sPreBgSprInfo;
	
	if(!gblIsROMLoaded) return FALSE;

	StopEmulator();
	ResetEmulator();

	memset(&es,0,sizeof(EMULATORSETUP));
	es.pbPRGROM=bPRGROM+0x8000;
	es.pbCHRROM=bCHRROM;
	if(iTrainer) es.pbTRAINER=bPRGROM+0x7000;

	SetupEmulator(&es);

	memset(&sTPS,0,sizeof(TESTPLAYSETUP));

	sTPS.BadGuysAddress.word=GetBadGuysAddress(uRoomID);
	sTPS.MapAddress.word=GetMapAddress(uRoomID);
	if(uRoomID==GETADDRESS_CURRENT_EDITTING)
		sTPS.bRoomID=GetRoomID();
	else
		sTPS.bRoomID=uRoomID;

	sTPS.bWorld=g_iWorld;
	sTPS.bArea=g_iArea;
	sTPS.bArea2=g_iArea2;

	sTPS.bPage=iPage;

	memset(&sMRM,0x00,sizeof(MAPRELATEDMEMORY));
	GetMapHeadData(uRoomID,bMapHead);
	if((bMapHead[0]&0x07)&0x04)
	{
		sMRM.bBackObject1=0x00;
		sMRM.bBackObject2=(bMapHead[0]&0x07);	
	}
	else
	{
		sMRM.bBackObject1=(bMapHead[0]&0x07);
		sMRM.bBackObject2=0x00;
	}//初期背景色

	sMRM.bBasicBlock =(bMapHead[1]&0x0F);//初期基本背景ブロック
	sMRM.bBackView =((bMapHead[1]>>4)&0x03);//初期景色

	PrepareMapRelatedMemory(uRoomID,iPage,(bMapHead[1]>>6)&0x03,&sMRM);
	
	sTPS.bBackObject1=sMRM.bBackObject1;
	sTPS.bBackObject2=sMRM.bBackObject2;
	sTPS.bBasicBlock=sMRM.bBasicBlock;
	sTPS.bBackView=sMRM.bBackView;
	memcpy(sTPS.bLeftObjNum,sMRM.bLeftObjNum,3);
	memcpy(sTPS.bLeftObjOfs,sMRM.bLeftObjOfs,3);
	sTPS.bLeftObjData1=sMRM.bLeftObjData1;//階段
	memcpy(sTPS.bLeftObjData2,sMRM.bLeftObjData2,3);//きのこの島の茎

	sTPS.bMapOfs=sMRM.bMapOfs;
	sTPS.bMapPageFlag=sMRM.bMapPageFlag;
	sTPS.bMapPage=iPage;

	sTPS.bMarioSize=0x01;
	sTPS.bMarioCap=0x00;

	sTPS.blIsStatusDraw=FALSE;

	TestPlaySetup(&sTPS);

	Run6502Ex(0x1280);//橙色の土管、夜に注意,0x1280

	SetPrepareBadGuysSpriteInfoStruct(sTPS.bRoomID,sTPS.bWorld,sTPS.bArea,&sPreBgSprInfo);
	PrepareBadGuysSpriteInfo(&sPreBgSprInfo);
	PrepareSpriteRAM(uRoomID,iPage);

	DrawAllScanline();

	return TRUE;
}



/*************************************************************

void TransferFromEmuBackBuffer(HDC hDCDest,int iDestX,int iDestY,int iWidth,int iHeight,BOOL blIsStretch)

  blIsStretch TRUE…iWidth,iHeightが有効。FALSE…iWidth,iHeightを無視

  エミュレータウインドウのバックバッファーからhDCDestで指定され
  たデバイスコンテキストへBitBltを行う

**************************************************************/
void InstallEmulatorPalette(HDC hdc)
{
	/*
	HPALETTE hPal=GetEmulatorPalette();
	if(!hPal) return;

	SelectPalette(hdc,hPal,FALSE);
	RealizePalette(hdc);
	*/
}

void TransferFromEmuBackBuffer(HDC hDCDest,int iDestX,int iDestY,int iWidth,int iHeight,BOOL blIsStretch)
{
	if(!gblIsROMLoaded) return;
	
	if(hDCDest && ghMemdcOffScreen)
	{
		SetEmuBackBufferPallete(NULL);

		InstallEmulatorPalette(hDCDest);

		if(blIsStretch){
			StretchBlt(hDCDest,iDestX,iDestY,iWidth,iHeight,ghMemdcOffScreen,0,16,NES_SCREENSIZEX,NES_SCREENSIZEY-16,SRCCOPY);
		}
		else{
			BitBlt(hDCDest,iDestX,iDestY,iWidth,iHeight,ghMemdcOffScreen,0,16,SRCCOPY);
		}
	}
}

/**************************************************************

  void RunEmulatorNormal()

  エミュレータの通常起動を行う。

***************************************************************/
void RunEmulatorNormal()
{
	EMULATORSETUP es;

	StopEmulator();

	memset(&es,0,sizeof(EMULATORSETUP));
	es.pbPRGROM=bPRGROM+0x8000;
	es.pbCHRROM=bCHRROM;
	if(iTrainer) es.pbTRAINER=bPRGROM+0x7000;

	SetupEmulator(&es);
				
	ResetEmulator();
	StartEmulator();
}


LRESULT CALLBACK TestPlaySettingDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	static STARTPOSHACK sphPrevX, sphPrevY;
   switch (message)
   {
       case WM_INITDIALOG:
		   {
			   int n;
			   LPTSTR StateName[] = {STRING_TESTPLAYSETTING_MARIO, STRING_TESTPLAYSETTING_SUPERMARIO, STRING_TESTPLAYSETTING_FIREMARIO};
			   LPTSTR StartUnitName[] = {STRING_TESTPLAYSETTING_NONE, STRING_TESTPLAYSETTING_POS, STRING_TESTPLAYSETTING_DOT};

			   if (rm_IsSubRoom())
				   EnableWindow(GetDlgItem(hDlg, IDC_HALFPAGE), FALSE);
			   if(gbIsCleared)
				   CheckDlgButton(hDlg, IDC_ISCLEARED, BST_CHECKED);
			   if(g_bl0PageBadGuyHack)
				   CheckDlgButton(hDlg, IDC_0PAGEBADGUYS, BST_CHECKED);
			   if(g_bInvincible)
				   CheckDlgButton(hDlg, IDC_INVINCIBLE, BST_CHECKED);

			   sphPrevX = g_fUseStartPosXHack;
			   switch(g_fUseStartPosXHack)
			   {
			   case NONE:
				   n = 0;
				   break;
			   case POS:
				   n = g_bStartPosX / 16;
				   break;
			   case DOT:
				   n = g_bStartPosX;
			   }
			   SetDlgItemInt(hDlg, IDC_XPOS, n, TRUE);
			   for(n = 0;n < 3;n++)
			   {
				   SendDlgItemMessage(hDlg, IDC_XPOSHACK, CB_ADDSTRING, 0, (LPARAM)StartUnitName[n]);
			   }
			   SendDlgItemMessage(hDlg, IDC_XPOSHACK, CB_SETCURSEL, (WPARAM)g_fUseStartPosXHack, 0);
			   PostMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_XPOSHACK, CBN_SELCHANGE), 0); 

			   sphPrevY = g_fUseStartPosYHack;
			   switch(g_fUseStartPosYHack)
			   {
			   case NONE:
				   n = -1;
				   break;
			   case POS:
				   n = (g_bStartPosY / 16) - 1;
				   break;
			   case DOT:
				   n = g_bStartPosY;
			   }
			   SetDlgItemInt(hDlg, IDC_YPOS, n, TRUE);
			   for(n = 0;n < 3;n++)
			   {
				   SendDlgItemMessage(hDlg, IDC_YPOSHACK, CB_ADDSTRING, 0, (LPARAM)StartUnitName[n]);
			   }
			   SendDlgItemMessage(hDlg, IDC_YPOSHACK, CB_SETCURSEL, (WPARAM)g_fUseStartPosYHack, 0);
			   PostMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_YPOSHACK, CBN_SELCHANGE), 0); 


			   for(n = 0;n < 3;n++)
			   {
				   SendDlgItemMessage(hDlg, IDC_MARIOSTATE, CB_ADDSTRING, 0, (LPARAM)StateName[n]);
			   }
			   SendDlgItemMessage(hDlg, IDC_MARIOSTATE, CB_SETCURSEL, (WPARAM)giMarioState, 0);

			   SetDlgItemInt(hDlg, IDC_PAGE, g_bTestPlayPage, FALSE);
			   if(g_blUseTestPlayPage)
				   CheckRadioButton(hDlg, IDC_FROMCURPAGE, IDC_FROMDESTPAGE, IDC_FROMDESTPAGE);
			   else
				   CheckRadioButton(hDlg, IDC_FROMCURPAGE, IDC_FROMDESTPAGE, IDC_FROMCURPAGE);
			   SendDlgItemMessage(hDlg, IDC_PAGESPIN, UDM_SETRANGE, 0, MAKEWPARAM(SMB_MAX_PAGE,0));

			   return TRUE;
		   }
       case WM_COMMAND:
               switch(LOWORD(wParam))
			   {
			   case IDC_PAGE:
				   {
					   switch(HIWORD(wParam))
					   {
					   case EN_CHANGE:
						   CheckRadioButton(hDlg, IDC_FROMCURPAGE, IDC_FROMDESTPAGE, IDC_FROMDESTPAGE);
						   break;
					   }
				   }
				   break;
			   case IDC_XPOSHACK:
				   {
					   switch(HIWORD(wParam))
					   {
					   case CBN_SELCHANGE:
						   {
							   BOOL fEditEnable, fTraslated;
							   WORD wSpinMin, wSpinMax;
							   int iNewValue;
							   STARTPOSHACK iRet = SendDlgItemMessage(hDlg, IDC_XPOSHACK, CB_GETCURSEL, 0, 0);

							   if(iRet == CB_ERR) break;
							   iNewValue = GetDlgItemInt(hDlg, IDC_XPOS, &fTraslated, TRUE);
							   switch(iRet)
							   {
							   case NONE:
								   fEditEnable = FALSE;
								   break;
							   case POS:
								   if (fTraslated
									   && sphPrevX == DOT)
									   iNewValue /= 16;
								   if (iNewValue < 0)
									   iNewValue = 0;
								   else if(iNewValue > 15)
									   iNewValue = 15;
								   fEditEnable = TRUE;
								   wSpinMin = 0;
								   wSpinMax = 15;
								   break;
							   case DOT:
								   if (fTraslated
									   && sphPrevX == POS)
									   iNewValue *= 16;
								   if (iNewValue < 0)
									   iNewValue = 0;
								   else if(iNewValue > TESTPLAY_STARTPOS_MAXDOTX)
									   iNewValue = TESTPLAY_STARTPOS_MAXDOTX;
								   fEditEnable = TRUE;
								   wSpinMin = 0;
								   wSpinMax = TESTPLAY_STARTPOS_MAXDOTX;
								   break;
							   }
							   EnableWindow(GetDlgItem(hDlg,IDC_XPOS), fEditEnable);
							   SetDlgItemInt(hDlg, IDC_XPOS, iNewValue, TRUE);
							   SendDlgItemMessage(hDlg, IDC_XPOSSPIN, UDM_SETRANGE, 0, MAKEWPARAM(wSpinMax, wSpinMin));

							   sphPrevX = iRet;
							   return TRUE;
						   }
					   }
				   }
				   break;
			   case IDC_YPOSHACK:
				   {
					   switch(HIWORD(wParam))
					   {
					   case CBN_SELCHANGE:
						   {
							   BOOL fEditEnable, fTraslated;
							   WORD wSpinMin, wSpinMax;
							   int iNewValue;
							   STARTPOSHACK iRet = SendDlgItemMessage(hDlg, IDC_YPOSHACK, CB_GETCURSEL, 0, 0);

							   if(iRet == CB_ERR) break;
							   iNewValue = GetDlgItemInt(hDlg, IDC_YPOS, &fTraslated, TRUE);
							   switch(iRet)
							   {
							   case NONE:
								   fEditEnable = FALSE;
								   break;
							   case POS:
								   if (fTraslated
									   && sphPrevY == DOT){
									   iNewValue /= 16;
									   iNewValue--;
								   }
								   if (iNewValue < -1)
									   iNewValue = -1;
								   else if(iNewValue > TESTPLAY_SETTINGS_MAXYPOS)
									   iNewValue = TESTPLAY_SETTINGS_MAXYPOS;
								   fEditEnable = TRUE;
								   wSpinMin = -1;
								   wSpinMax = TESTPLAY_SETTINGS_MAXYPOS;
								   break;
							   case DOT:
								   if (fTraslated
									   && sphPrevY == POS){
									   iNewValue++;
									   iNewValue *= 16;
								   }
								   if (iNewValue < 0)
									   iNewValue = 0;
								   else if(iNewValue > (TESTPLAY_SETTINGS_MAXYPOS + 1) * 16)
									   iNewValue = (TESTPLAY_SETTINGS_MAXYPOS + 1) * 16;
								   fEditEnable = TRUE;
								   wSpinMin = 0;
								   wSpinMax = (TESTPLAY_SETTINGS_MAXYPOS + 1) * 16;
								   break;
							   }
							   EnableWindow(GetDlgItem(hDlg,IDC_YPOS), fEditEnable);
							   SetDlgItemInt(hDlg, IDC_YPOS, iNewValue, TRUE);
							   SendDlgItemMessage(hDlg, IDC_YPOSSPIN, UDM_SETRANGE, 0, MAKEWPARAM(wSpinMin, wSpinMax));

							   sphPrevY = iRet;
							   return TRUE;
						   }
					   }

				   }
				   break;
			   case IDC_HALFPAGE:
				   {
					   switch(HIWORD(wParam))
					   {
					   case BN_CLICKED:
						   SetDlgItemInt(hDlg, IDC_PAGE, GetHalfPointPage(rm_GetWorld(),rm_GetArea()), FALSE);
						   break;
					   }
				   }
				   break;
			   case IDOK:
				   {
					   int iRet;
					   BOOL fTranslated;
					   STARTPOSHACK iSPH;
					   
					   gbIsCleared=0x00;
					   if(IsDlgButtonChecked(hDlg,IDC_ISCLEARED)==BST_CHECKED) 
						   gbIsCleared=0x01;

					   //
					   iSPH = SendDlgItemMessage(hDlg, IDC_XPOSHACK, CB_GETCURSEL, 0, 0);
					   if (iSPH == CB_ERR)
						   return FALSE;
					   if (iSPH) {
						   iRet = GetDlgItemInt(hDlg, IDC_XPOS, &fTranslated, TRUE);
						   switch(iSPH)
						   {
						   case POS:
							   if (!fTranslated
								   || iRet < 0
								   || iRet > 15)
								   return FALSE;
							   g_bStartPosX = iRet * 16;
							   break;
						   case DOT:
							   if (!fTranslated
								   || iRet < 0
								   || iRet > TESTPLAY_STARTPOS_MAXDOTX)
								   return FALSE;
							   g_bStartPosX = iRet;
							   break;
						   }
					   }
					   g_fUseStartPosXHack = iSPH;

					   //
					   iSPH = SendDlgItemMessage(hDlg, IDC_YPOSHACK, CB_GETCURSEL, 0, 0);
					   if (iSPH == CB_ERR)
						   return FALSE;
					   if (iSPH) {
						   iRet = GetDlgItemInt(hDlg, IDC_YPOS, &fTranslated, TRUE);
						   switch(iSPH)
						   {
						   case POS:
						   if (!fTranslated
							   || iRet < -1
							   || iRet > TESTPLAY_SETTINGS_MAXYPOS)
							   return FALSE;
							   g_bStartPosY = (iRet + 1) * 16;
							   break;
						   case DOT:
						   if (!fTranslated
							   || iRet < 0
							   || iRet > (TESTPLAY_SETTINGS_MAXYPOS + 1) * 16)
							   return FALSE;
							   g_bStartPosY = iRet;
							   break;
						   }
					   }
					   g_fUseStartPosYHack = iSPH;

					   iRet = SendDlgItemMessage(hDlg,IDC_MARIOSTATE,CB_GETCURSEL,0,0);
					   if(iRet == CB_ERR) return TRUE;
					   giMarioState = iRet;

					   g_blUseTestPlayPage = FALSE;
					   if(BST_CHECKED == IsDlgButtonChecked(hDlg,IDC_FROMDESTPAGE)){
						   iRet = GetDlgItemInt(hDlg, IDC_PAGE, &fTranslated, FALSE);
						   if(!fTranslated) return TRUE;
						   g_bTestPlayPage = iRet;
						   g_blUseTestPlayPage = TRUE;
					   }

					   g_bl0PageBadGuyHack = FALSE;
					   if(BST_CHECKED == IsDlgButtonChecked(hDlg,IDC_0PAGEBADGUYS))
						   g_bl0PageBadGuyHack = TRUE;

					   g_bInvincible = FALSE;
					   if(BST_CHECKED == IsDlgButtonChecked(hDlg,IDC_INVINCIBLE))
						   g_bInvincible = TRUE;


				   }
			   case IDCANCEL: 
				   EndDialog(hDlg, TRUE);
				   return TRUE;
			   }
   }
   return FALSE;
}