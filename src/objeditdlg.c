/************************************************************************************

                                  smb Utility

  File: objeditdlg.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "roommng.h"
#include "emulator.h"
#include "emuutil.h"
#include "objlist.h"
#include "objlib.h"
#include "objview.h"
#include "objmng.h"
#include "resource.h"

extern SMBMAPOBJECTINFO smbMapObjectInfo0B[];
extern SMBMAPOBJECTINFO smbMapObjectInfoC[];
extern SMBMAPOBJECTINFO smbMapObjectInfoD[];
extern SMBMAPOBJECTINFO smbMapObjectInfoE[];
extern SMBMAPOBJECTINFO smbMapObjectInfoF[];
extern struct {LPSTR Name;}smbMapBasicBlock[];
extern LPSTR smbMapHeadTime[];
extern LPSTR smbMapHeadPosition[];
extern LPSTR smbMapHeadBackColor[];
extern LPSTR smbMapHeadMapType[];
extern LPSTR smbMapHeadView[];
extern SMBBADGUYSINFO smbBudGuysInfo[];
extern int GetNum0BMapObject();
extern int GetNumCMapObject();
extern int GetNumDMapObject();
extern int GetNumEMapObject();
extern int GetNumFMapObject();
/***********************

  敵エデイットダイアログ

************************/
#define BADGUYS_EDITDLG_NOPREVIEWSIZE 420
#define BADGUYS_EDITDLG_HASPREVIEWSIZE 625

#define DISABLE_2BYTES 0
#define DISABLE_3BYTES  1
static void DisableBadguysEditDlgControls(HWND hDlg,int iDisableFlag)
{
	int n;

	if(iDisableFlag==DISABLE_2BYTES)
	{
		int iCtrlID[]={IDC_XPOS,IDC_YPOS,IDC_PAGEFLAG,IDC_BIT6,IDC_TYPE,IDC_ISPAGECOMMAND,IDC_PAGEEDIT,IDC_STATIC_XPOS1,IDC_STATIC_YPOS,IDC_STATIC_TYPE,IDC_STATIC_OBJECT};
		int iEditCtrlID[]={IDC_XPOS,IDC_YPOS,IDC_PAGEEDIT};
		
		for(n=0;n<(sizeof(iEditCtrlID)/sizeof(int));n++)
		SetDlgItemText(hDlg,iEditCtrlID[n],"");
		for(n=0;n<(sizeof(iCtrlID)/sizeof(int));n++)
		EnableWindow(GetDlgItem(hDlg,iCtrlID[n]),FALSE);
	}
	else if(iDisableFlag==DISABLE_3BYTES)
	{
		int iCtrlID[]={IDC_XPOS2,IDC_DATA,IDC_PAGEEDIT2,IDC_PAGEFLAG2,IDC_WORLD,IDC_OPENPREVIEW,IDC_STATIC_ROOMMOVE,IDC_STATIC_XPOS2,IDC_STATIC_ROOM,IDC_STATIC_WORLD,IDC_STATIC_PAGE};
		int iEditCtrlID[]={IDC_XPOS2,IDC_DATA,IDC_PAGEEDIT2,IDC_WORLD};
		
		for(n=0;n<(sizeof(iEditCtrlID)/sizeof(int));n++)
		SetDlgItemText(hDlg,iEditCtrlID[n],"");
		for(n=0;n<(sizeof(iCtrlID)/sizeof(int));n++)
		EnableWindow(GetDlgItem(hDlg,iCtrlID[n]),FALSE);
	}
}

void UpdateBadguysEditDlgPreview(HWND hDlg,BOOL blGetRoomIDFromList)
{
	BYTE bRoomID;
	int iPage;
	BOOL blSuccess;
	char cBuf[10];
	HWND hPVWnd;
	HDC hPVdc;
	RECT rcPV;

	if(!blGetRoomIDFromList)
	{
		GetDlgItemText(hDlg,IDC_DATA,cBuf,10);
		if(1!=sscanf(cBuf,"%x",&bRoomID)) return;
	}
	else
	{
		int iSel;
		BYTE bID[SMB_NUM_ADDRESSDATA];

		GetValidRoomIDs(bID);
		iSel=SendDlgItemMessage(hDlg,IDC_DATA,CB_GETCURSEL,0,0);
		if(iSel==CB_ERR) return;
		bRoomID=bID[iSel];
	}

	if(!IsRoomIDValid(bRoomID)) return;
	
	iPage=GetDlgItemInt(hDlg,IDC_PAGEEDIT2,&blSuccess,FALSE);
	if(!blSuccess) return;

	hPVWnd=GetDlgItem(hDlg,IDC_VIEW);
	GetClientRect(hPVWnd,&rcPV);
	hPVdc=GetDC(hPVWnd);
	if(hPVdc)
	{
		if(RunEmulatorViewPage(bRoomID,iPage))
			TransferFromEmuBackBuffer(hPVdc,0,0,rcPV.right-rcPV.left,rcPV.bottom-rcPV.top,TRUE);
		else
			FillRect(hPVdc,&rcPV,GetSysColorBrush(COLOR_3DFACE));
		ReleaseDC(hPVWnd,hPVdc);
	}
	ClearEmuBackBuffer();
}

LRESULT CALLBACK BadGuysComEditDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	static BOOL blIs3BytesObj;
   switch (message)
   {
       case WM_INITDIALOG:
		   {
			   int n,iPage,iOfs;
			   BYTE bBuf[3];
			   char cOfs[10];
			   RECT rcDlg;

			   iOfs=GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
			   if(iOfs==-1){
				   EndDialog(hDlg, TRUE);
				   return TRUE;
			   }

			   //Resize Window
			   GetWindowRect(hDlg,&rcDlg);
			   MoveWindow(hDlg,rcDlg.left,rcDlg.top,BADGUYS_EDITDLG_NOPREVIEWSIZE,rcDlg.bottom-rcDlg.top,TRUE);

			   for(n=0;n<64;n++)
				   SendDlgItemMessage(hDlg,IDC_TYPE,CB_ADDSTRING,0,(LPARAM)smbBudGuysInfo[n].Name);
			   SendDlgItemMessage(hDlg,IDC_XPOSSPIN,UDM_SETRANGE,0,MAKEWPARAM(15,0));
			   
			   SendDlgItemMessage(hDlg,IDC_PAGEEDITSPIN,UDM_SETRANGE,0,MAKEWPARAM(0x3F,0));

			   sprintf(cOfs,"%.2XH",iOfs);
			   SetDlgItemText(hDlg,IDC_OFFSET,cOfs);
			   
			   //page
			   SetDlgItemInt(hDlg,IDC_PAGE,iPage,FALSE);
			   if((bBuf[0]&0x0F)==0x0E)
			   {
				   BYTE bRoomIDs[SMB_NUM_ADDRESSDATA];
				   char cBuf[20];
				   int n;

				   //ワールドの範囲
				   SendDlgItemMessage(hDlg,IDC_WORLDSPIN,UDM_SETRANGE,0,MAKEWPARAM(8,1));
				   //ページの範囲
				   SendDlgItemMessage(hDlg,IDC_PAGEEDIT2SPIN,UDM_SETRANGE,0,MAKEWPARAM(0x1F,0));
				   //ページの範囲
				   SendDlgItemMessage(hDlg,IDC_XPOS2SPIN,UDM_SETRANGE,0,MAKEWPARAM(0xF,0));

				   sprintf(cBuf,"%.2x %.2x %.2x",bBuf[0],bBuf[1],bBuf[2]);
				   SetDlgItemText(hDlg,IDC_BIN,cBuf);
				   CheckDlgButton(hDlg,IDC_ISBIN,BST_UNCHECKED);
				   
				   //XPos
				   SetDlgItemInt(hDlg,IDC_XPOS2,(bBuf[0]>>4)&0x0F,TRUE);
				   //World
				   SetDlgItemInt(hDlg,IDC_WORLD,((bBuf[2]>>5)&0x07)+1,TRUE);
				   //Page
				   SetDlgItemInt(hDlg,IDC_PAGEEDIT2,bBuf[2]&0x1F,TRUE);
				   //Room ID
				   sprintf(cBuf,"%.2x",bBuf[1]&0x7F);
				   SetDlgItemText(hDlg,IDC_DATA,cBuf);
				   GetValidRoomIDs(&bRoomIDs[0]);
				   for(n=0;n<SMB_NUM_ADDRESSDATA;n++){
					   sprintf(cBuf,"%.2x",bRoomIDs[n]);
					   SendDlgItemMessage(hDlg,IDC_DATA,CB_ADDSTRING,0,(LPARAM)cBuf);
				   }

				   //
				   if(bBuf[1]&0x80)  CheckDlgButton(hDlg,IDC_PAGEFLAG2,BST_CHECKED);
				   blIs3BytesObj=TRUE;
				   DisableBadguysEditDlgControls(hDlg,DISABLE_2BYTES);

				   //
				   return TRUE;
			   }
			   else if((bBuf[0]&0x0F)==0x0F)
			   {
				   char cBuf[10];
				   //改ページフラグ
				   if (bBuf[1] & 0x80)
					   CheckDlgButton(hDlg,IDC_PAGEFLAG,BST_CHECKED);
//				   if (bBuf[1] & 0x40)
//					   CheckDlgButton(hDlg,IDC_BIT6,BST_CHECKED);
				   //XPos
				   SetDlgItemInt(hDlg,IDC_XPOS,(bBuf[0]>>4)&0x0F,TRUE);
				   //
				   //Page data
				   SetDlgItemInt(hDlg,IDC_PAGEEDIT,(int)bBuf[1] & 0x3F,FALSE);

				   CheckDlgButton(hDlg,IDC_ISPAGECOMMAND,BST_CHECKED);
				   //bin
				   sprintf(cBuf,"%.2x %.2x",bBuf[0],bBuf[1]);
				   SetDlgItemText(hDlg,IDC_BIN,cBuf);
				   CheckDlgButton(hDlg,IDC_ISBIN,BST_UNCHECKED);
				   
				   blIs3BytesObj=FALSE;
				   DisableBadguysEditDlgControls(hDlg,DISABLE_3BYTES);
				   // 一応なにか選択しておく
				   SendDlgItemMessage(hDlg,IDC_TYPE,CB_SETCURSEL,0,0);
				   // 関係のない要素を無効化
				   EnableWindow(GetDlgItem(hDlg,IDC_TYPE), FALSE);
				   EnableWindow(GetDlgItem(hDlg,IDC_BIT6), FALSE);

				   return TRUE;
			   }
			   else
			   {
				   char cBuf[10];

				   SetDlgItemInt(hDlg, IDC_XPOS, GetBadGuysXPos(bBuf), TRUE);
				   SetDlgItemInt(hDlg, IDC_YPOS, GetBadGuysYPos(bBuf), TRUE);
				   SendDlgItemMessage(hDlg,IDC_TYPE,CB_SETCURSEL,bBuf[1]&0x3F,0);
				   sprintf(cBuf,"%.2x %.2x",bBuf[0],bBuf[1]);
				   SetDlgItemText(hDlg,IDC_BIN,cBuf);
				   CheckDlgButton(hDlg,IDC_ISBIN,BST_UNCHECKED);
				   if(bBuf[1]&0x40) CheckDlgButton(hDlg,IDC_BIT6,BST_CHECKED);
				   if(bBuf[1]&0x80)  CheckDlgButton(hDlg,IDC_PAGEFLAG,BST_CHECKED);
				   //y位置の範囲の設定
				   SendDlgItemMessage(hDlg,IDC_YPOSSPIN,UDM_SETRANGE,0,MAKEWPARAM(-1,13+smbBudGuysInfo[bBuf[1]&0x3F].YDelta));
				   //x位置の範囲の設定
				   SendDlgItemMessage(hDlg,IDC_XPOSSPIN,UDM_SETRANGE,0,MAKEWPARAM(15+smbBudGuysInfo[bBuf[1]&0x3F].XDelta,smbBudGuysInfo[bBuf[1]&0x3F].XDelta));
				   
				   blIs3BytesObj=FALSE;
				   DisableBadguysEditDlgControls(hDlg,DISABLE_3BYTES);
				   EnableWindow(GetDlgItem(hDlg,IDC_PAGEEDIT), FALSE);
				   return TRUE;
			   }
		   }
	   case WM_PAINT:
		   if(BST_UNCHECKED==IsDlgButtonChecked(hDlg,IDC_OPENPREVIEW)) break;
		   UpdateBadguysEditDlgPreview(hDlg,FALSE);
		   break;
       case WM_COMMAND:
               switch(LOWORD(wParam))
			   {
			   case IDOK:
				   {
					   if(IsDlgButtonChecked(hDlg,IDC_ISBIN)&BST_CHECKED)
					   {
						   char cBuf[20];
						   BYTE bBuf[4];
						   int iSize;
						   //バイナリデータの取得
						   GetDlgItemText(hDlg,IDC_BIN,cBuf,20);
						   iSize=sscanf(cBuf,"%x %x %x %x",&bBuf[0],&bBuf[1],&bBuf[2],&bBuf[3]);
						   if(iSize<1 || iSize>4) return TRUE;

						   undoPrepare(UNDONAME_DLGEDIT);
						   SetBadGuysDataBinary(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,iSize);
					   }
					   else if(!blIs3BytesObj)
					   {
						   BOOL blSuccess;
						   BYTE bBuf[2];
//						   TCHAR cBuf[20];
						   UINT uRet;
						   BYTE bTmp;
						   BYTE bType;

						   memset(bBuf,0,2);
						   if(IsDlgButtonChecked(hDlg,IDC_ISPAGECOMMAND)==BST_CHECKED)
						   {
							   //
							   bBuf[0] |= 0x0F;
							   //
							   uRet = GetDlgItemInt(hDlg,IDC_PAGEEDIT,&blSuccess,FALSE);
							   if(!blSuccess || uRet > 0x3F) return TRUE;
							   bBuf[1] = (BYTE)uRet;
							   //XPOS
							   uRet = GetDlgItemInt(hDlg,IDC_XPOS,&blSuccess,FALSE);
							   if(!blSuccess) return TRUE;
							   bBuf[0]|=((uRet&0x0F)<<4);
							   //
							   if (IsDlgButtonChecked(hDlg,IDC_PAGEFLAG) == BST_CHECKED)
								   bBuf[1] |= 0x80;
							   //if (IsDlgButtonChecked(hDlg,IDC_BIT6) == BST_CHECKED)
							   //  bBuf[1] |= 0x40;
						   }
						   else
						   {
							   //種類の取得
							   bType=(BYTE)SendDlgItemMessage(hDlg,IDC_TYPE,CB_GETCURSEL,0,0);
							   bBuf[1]|=bType;
							   //Xの取得
							   //GetDlgItemText(hDlg,IDC_XPOS,cBuf,20);
							   //if(1!=sscanf(cBuf,"%d",&bTmp)) return TRUE;
							   bTmp = GetDlgItemInt(hDlg, IDC_XPOS, &blSuccess, TRUE);
							   if (!blSuccess) return TRUE;
							   bTmp-=smbBudGuysInfo[bType].XDelta;
							   if(bTmp>0xF) return TRUE;
							   bBuf[0]|=(bTmp<<4);
							   //Yの取得
							   //GetDlgItemText(hDlg,IDC_YPOS,cBuf,20);
							   //if(1!=sscanf(cBuf,"%d",&bTmp)) return TRUE;
							   bTmp = GetDlgItemInt(hDlg, IDC_YPOS, &blSuccess, TRUE);
							   if (!blSuccess) return TRUE;
							   bTmp+=abs(smbBudGuysInfo[bType].YDelta);
							   if(bTmp>0xD) return TRUE;
							   bBuf[0]|=(bTmp&0x0F);
							   //改ページフラグの取得
							   if(BST_CHECKED&IsDlgButtonChecked(hDlg,IDC_PAGEFLAG))
								   bBuf[1]|=0x80;
							   //第2バイトのビット6の取得
							   if(BST_CHECKED&IsDlgButtonChecked(hDlg,IDC_BIT6))
								   bBuf[1]|=0x40;
						   }

						   undoPrepare(UNDONAME_DLGEDIT);
						   SetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf);
					   }
					   else
					   {
						   BYTE bBuf[3];
						   TCHAR cBuf[3];
						   BYTE bTmp;
						   BOOL blSuccess;

						   memset(bBuf,0,3);

						   //XPOS
						   bTmp=(BYTE)GetDlgItemInt(hDlg,IDC_XPOS2,&blSuccess,FALSE);
						   if(!blSuccess || bTmp>0x0F) return TRUE;
						   bBuf[0]|=((bTmp&0x0F)<<4);
						   //YPOS
						   bBuf[0]|=0x0E;
						   //改ページフラグ
						   if(BST_CHECKED==IsDlgButtonChecked(hDlg,IDC_PAGEFLAG2))
							   bBuf[1]|=0x80;
						   //
						   GetDlgItemText(hDlg,IDC_DATA,cBuf,3);
						   if(1!=sscanf(cBuf,"%x",&bTmp)) return TRUE;
						   bBuf[1]|=(bTmp&0x7F);
						   //ワールド
						   bTmp=GetDlgItemInt(hDlg,IDC_WORLD,&blSuccess,FALSE);
						   if(!blSuccess || bTmp>8) return TRUE;
						   bBuf[2]|=(((bTmp-1)&0x07)<<5);
						   //ページ
						   bTmp=GetDlgItemInt(hDlg,IDC_PAGEEDIT2,&blSuccess,FALSE);
						   if(!blSuccess || bTmp>0x1F) return TRUE;
						   bBuf[2]|=(bTmp&0x1F);
					   
						   undoPrepare(UNDONAME_DLGEDIT);
						   
						   SetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf);
					   }
				   }

				   UpdateObjectList(0);
				   UpdateObjectView(0);
				   ObjectListSetCursor(GetSelectedIndex());
			   case IDCANCEL:
				   EndDialog(hDlg, TRUE);
				   return TRUE;
			   case IDC_TYPE:
				   {
					   BYTE bType;
					   bType=(BYTE)SendDlgItemMessage(hDlg,IDC_TYPE,CB_GETCURSEL,0,0);
					   if(bType!=CB_ERR)
					   {
						   //y位置の範囲の設定
						   SendDlgItemMessage(hDlg,IDC_YPOSSPIN,UDM_SETRANGE,0,MAKEWPARAM(-1,13+smbBudGuysInfo[bType].YDelta));
						   //x位置の範囲の設定
						   SendDlgItemMessage(hDlg,IDC_XPOSSPIN,UDM_SETRANGE,0,MAKEWPARAM(15+smbBudGuysInfo[bType].XDelta,smbBudGuysInfo[bType].XDelta));
					   }
					   if(HIWORD(wParam)==CBN_SELCHANGE)
						   CheckDlgButton(hDlg,IDC_ISBIN,BST_UNCHECKED);
					   return TRUE;
				   }
			   case IDC_BIN:
					   if(HIWORD(wParam)==EN_CHANGE)
						   CheckDlgButton(hDlg,IDC_ISBIN,BST_CHECKED);
					   return TRUE;
			   case IDC_XPOS:
			   case IDC_YPOS:
					   if(HIWORD(wParam)==EN_CHANGE)
						   CheckDlgButton(hDlg,IDC_ISBIN,BST_UNCHECKED);
					   return TRUE;
			   case IDC_PAGEFLAG:
			   case IDC_BIT6:
				   if(HIWORD(wParam)==BN_CLICKED)
					   CheckDlgButton(hDlg,IDC_ISBIN,BST_UNCHECKED);
				   return TRUE;
			   //ルーム間移動
			   case IDC_DATA:
				   if(BST_UNCHECKED==IsDlgButtonChecked(hDlg,IDC_OPENPREVIEW)) return TRUE;
				   if(HIWORD(wParam)==CBN_EDITCHANGE)
					   UpdateBadguysEditDlgPreview(hDlg,FALSE);
				   else if(HIWORD(wParam)==CBN_SELCHANGE)
					   UpdateBadguysEditDlgPreview(hDlg,TRUE);
				   return TRUE;
			   case IDC_PAGEEDIT2:
				   if(HIWORD(wParam)==EN_CHANGE){
					   if(BST_UNCHECKED==IsDlgButtonChecked(hDlg,IDC_OPENPREVIEW)) return TRUE;
					   UpdateBadguysEditDlgPreview(hDlg,FALSE);
					   return TRUE;
				   }
			   case IDC_OPENPREVIEW:
				   {
					   if(HIWORD(wParam)==BN_CLICKED)
					   {
						   RECT rcDlg;
						   
						   GetWindowRect(hDlg,&rcDlg);
						   if(BST_UNCHECKED==IsDlgButtonChecked(hDlg,IDC_OPENPREVIEW))
							   MoveWindow(hDlg,rcDlg.left,rcDlg.top,BADGUYS_EDITDLG_NOPREVIEWSIZE,rcDlg.bottom-rcDlg.top,TRUE);
						   else if(BST_CHECKED==IsDlgButtonChecked(hDlg,IDC_OPENPREVIEW))
						   {
							   MoveWindow(hDlg,rcDlg.left,rcDlg.top,BADGUYS_EDITDLG_HASPREVIEWSIZE,rcDlg.bottom-rcDlg.top,TRUE);
							   UpdateBadguysEditDlgPreview(hDlg,FALSE);
						   }
					   }
				   }
				   return TRUE;
			   case IDC_ISPAGECOMMAND:
				   {
					   if(BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_ISPAGECOMMAND)){
						   EnableWindow(GetDlgItem(hDlg,IDC_TYPE), FALSE);
						   EnableWindow(GetDlgItem(hDlg,IDC_BIT6), FALSE);
						   EnableWindow(GetDlgItem(hDlg,IDC_PAGEEDIT), TRUE);
					   }
					   else{
						   EnableWindow(GetDlgItem(hDlg,IDC_TYPE), TRUE);
						   EnableWindow(GetDlgItem(hDlg,IDC_BIT6), TRUE);
						   EnableWindow(GetDlgItem(hDlg,IDC_PAGEEDIT), FALSE);
					   }
				   }
				   return TRUE;
			   }
   }
   return FALSE;
}

/*********************************

  マップエディトダイアログ関数関数

***********************************/

void DisableConrols(HWND hDlg)
{
	SetDlgItemText(hDlg,IDC_LEN,"");
	EnableWindow(GetDlgItem(hDlg,IDC_LEN),FALSE);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_LENGTH),FALSE);
	
	SendDlgItemMessage(hDlg,IDC_VIEW,CB_SETCURSEL,(WPARAM)-1,0);
	EnableWindow(GetDlgItem(hDlg,IDC_VIEW),FALSE);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_VIEW),FALSE);

	SendDlgItemMessage(hDlg,IDC_FIRSTBLOCK,CB_SETCURSEL,(WPARAM)-1,0);
	EnableWindow(GetDlgItem(hDlg,IDC_FIRSTBLOCK),FALSE);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_BLOCK),FALSE);

	SendDlgItemMessage(hDlg,IDC_BACKCOLOR,CB_SETCURSEL,(WPARAM)-1,0);
	EnableWindow(GetDlgItem(hDlg,IDC_BACKCOLOR),FALSE);
	EnableWindow(GetDlgItem(hDlg,IDC_STATIC_BACKCOLOR),FALSE);
}

static void EnableControl(HWND hDlg,int iCtrlID)
{
	EnableWindow(GetDlgItem(hDlg,iCtrlID),TRUE);
}

#define PREPAREDLG_MAPCOMEDIT_ALL 0
#define PREPAREDLG_MAPCOMEDIT_YPOS 1
#define PREPAREDLG_MAPCOMEDIT_BININPUTONLY 2
static void PrepareMapComEditDlg(HWND hDlg,BYTE *bBuf,int iPage,int iPrepareOption)
{
	int n;
	int iMaxNumObjs;
	SMBMAPOBJECTINFO* psObjInfo;

	CheckDlgButton(hDlg,IDC_ISBIN,BST_UNCHECKED);

	//XPOS
	SendDlgItemMessage(hDlg,IDC_XPOSSPIN,UDM_SETRANGE,0,MAKEWPARAM(15,0));
	SetDlgItemInt(hDlg,IDC_XPOS,(bBuf[0]>>4)&0x0F,FALSE);

	//PAGE FLAG
	if(bBuf[1]&0x80)  CheckDlgButton(hDlg,IDC_PAGEFLAG,BST_CHECKED);

	//YPOS
	if(iPrepareOption!=PREPAREDLG_MAPCOMEDIT_YPOS)
	{
		SendDlgItemMessage(hDlg,IDC_YPOSSPIN,UDM_SETRANGE,0,MAKEWPARAM(0,15));
		SetDlgItemInt(hDlg,IDC_YPOS,bBuf[0]&0x0F,FALSE);
	}

	//タイプを並べる
	DisableConrols(hDlg);
	switch(bBuf[0]&0x0F)
	{
	case 0x0C:psObjInfo=smbMapObjectInfoC;iMaxNumObjs=GetNumCMapObject();break;
	case 0x0D:psObjInfo=smbMapObjectInfoD;iMaxNumObjs=GetNumDMapObject();break;
	case 0x0E:
	{
		psObjInfo=smbMapObjectInfoE;
		iMaxNumObjs=GetNumEMapObject();
		if(bBuf[1]&0x40)
		{
			SendDlgItemMessage(hDlg,IDC_BACKCOLOR,CB_SETCURSEL,bBuf[1]&0x07,0);
			EnableControl(hDlg,IDC_BACKCOLOR);
			EnableControl(hDlg,IDC_STATIC_BACKCOLOR);
		}
		else
		{
			SendDlgItemMessage(hDlg,IDC_VIEW,CB_SETCURSEL,(bBuf[1]>>4)&0x03,0);
			EnableControl(hDlg,IDC_VIEW);
			EnableControl(hDlg,IDC_STATIC_VIEW);
			SendDlgItemMessage(hDlg,IDC_FIRSTBLOCK,CB_SETCURSEL,bBuf[1]&0x0F,0);
			EnableControl(hDlg,IDC_FIRSTBLOCK);
			EnableControl(hDlg,IDC_STATIC_BLOCK);
		}
	}
	break;
	case 0x0F:psObjInfo=smbMapObjectInfoF;iMaxNumObjs=GetNumFMapObject();break;
	default:psObjInfo=smbMapObjectInfo0B;iMaxNumObjs=GetNum0BMapObject();break;
	}

	while(SendDlgItemMessage(hDlg,IDC_TYPE,CB_GETCOUNT,0,0) && CB_ERR!=SendDlgItemMessage(hDlg,IDC_TYPE,CB_GETCOUNT,0,0))
	{
		SendDlgItemMessage(hDlg,IDC_TYPE,CB_DELETESTRING,0,0);
	}

	for(n=0;n<iMaxNumObjs;n++)
		SendDlgItemMessage(hDlg,IDC_TYPE,CB_ADDSTRING,0,(LPARAM)psObjInfo[n].Name);

	//
	for(n=0;n<iMaxNumObjs;n++)
	{
		if((bBuf[1]&psObjInfo[n].bBasicDataMask)==psObjInfo[n].bBasicData) break;
	}

	//タイプを選択
	if(n<iMaxNumObjs)
		SendDlgItemMessage(hDlg,IDC_TYPE,CB_SETCURSEL,n,0);
	else//不明なオブジェクトの場合
	{
		SendDlgItemMessage(hDlg,IDC_TYPE,CB_SETCURSEL,-1,0);
		CheckDlgButton(hDlg,IDC_ISBIN,BST_CHECKED);
	}
	//注意書きを表示
	SetDlgItemText(hDlg,IDC_NOTE,psObjInfo[n].Note);

	//YPOSをセット→サイズをセットの順
	if(psObjInfo[n].bIsSizeValid)
	{
		EnableControl(hDlg,IDC_LEN);
		EnableControl(hDlg,IDC_STATIC_LENGTH);
		SendDlgItemMessage(hDlg,IDC_LENSPIN,UDM_SETRANGE,0,MAKEWPARAM(psObjInfo[n].bSizeMask+abs(psObjInfo[n].iSizeDelta),abs(psObjInfo[n].iSizeDelta)));
		SetDlgItemInt(hDlg,IDC_LEN,(bBuf[1]&psObjInfo[n].bSizeMask)+abs(psObjInfo[n].iSizeDelta),FALSE);
	}
}

LRESULT CALLBACK MapComEditDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	static BYTE bBuf[2];
	static int iPage;

	switch (message)
   {
       case WM_INITDIALOG:
		   {
			   int n;
			   char cBuf[10];
			   int iOfs;
			   
			   iOfs=GetMapData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
			   if(iOfs==-1){
				   EndDialog(hDlg, TRUE);
				   return TRUE;
			   }
			   //
			   for(n=0;n<4;n++) SendDlgItemMessage(hDlg,IDC_VIEW,CB_ADDSTRING,0,(LPARAM)smbMapHeadView[n]);
			   for(n=0;n<16;n++) SendDlgItemMessage(hDlg,IDC_FIRSTBLOCK,CB_ADDSTRING,0,(LPARAM)smbMapBasicBlock[n].Name);
			   for(n=0;n<8;n++) SendDlgItemMessage(hDlg,IDC_BACKCOLOR,CB_ADDSTRING,0,(LPARAM)smbMapHeadBackColor[n]);
			   
			   sprintf(cBuf,"%.2XH",iOfs);
			   SetDlgItemText(hDlg,IDC_OFFSET,cBuf);

			   sprintf(cBuf,"%.2x %.2x",bBuf[0],bBuf[1]);
			   SetDlgItemText(hDlg,IDC_BIN,cBuf);
			   //PAGE
			   SetDlgItemInt(hDlg,IDC_PAGE,iPage,FALSE);
			   
			   PrepareMapComEditDlg(hDlg,bBuf,iPage,PREPAREDLG_MAPCOMEDIT_ALL);
               return TRUE;
		   }
		   break;
       case WM_COMMAND:
		   {
               switch(LOWORD(wParam))
			   {
			   case IDOK:
				   {
	
					   BYTE bTmp;
					   int iSel,iSize;
					   BOOL blSuccess;
					   SMBMAPOBJECTINFO* psObjInfo;
					   BYTE bNewData[2];
					   
					   memset(bNewData,0,2);
					   if(IsDlgButtonChecked(hDlg,IDC_ISBIN)&BST_CHECKED)
					   {
						   BYTE bBinBuf[3]={0};
						   char cBinBuf[10]={0};
						   int iValidSize;

						   GetDlgItemText(hDlg,IDC_BIN,cBinBuf,10);
						   iValidSize=sscanf(cBinBuf,"%x %x %x",&bBinBuf[0],&bBinBuf[1],&bBinBuf[2]);
						   if(iValidSize<1 || 3<iValidSize) return TRUE;

						   undoPrepare(UNDONAME_DLGEDIT);
						   memcpy(bBuf,bNewData,2);
						   SetMapDataBinary(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBinBuf,iValidSize);
					   
					   }
					   else
					   {
						   //改ページフラグ
						   if(BST_CHECKED&IsDlgButtonChecked(hDlg,IDC_PAGEFLAG)) bNewData[1]|=0x80;
						   //Xの取得
						   bTmp=(BYTE)GetDlgItemInt(hDlg,IDC_XPOS,&blSuccess,FALSE);
						   if(!blSuccess || bTmp>0x0F) return FALSE;
						   bNewData[0]|=((bTmp&0x0F)<<4);
						   //Yの取得
						   bTmp=(BYTE)GetDlgItemInt(hDlg,IDC_YPOS,&blSuccess,FALSE);
						   if(!blSuccess || bTmp>0x0F) return FALSE;
						   bNewData[0]|=(bTmp&0x0F);

						   iSel=SendDlgItemMessage(hDlg,IDC_TYPE,CB_GETCURSEL,0,0);					   
						   if(iSel==CB_ERR)	return TRUE;

						   switch(bTmp&0x0F)
						   {
						   case 0x0C:psObjInfo=smbMapObjectInfoC;break;
						   case 0x0D:psObjInfo=smbMapObjectInfoD;break;
						   case 0x0E:
						   {
							   BYTE bSel;
							   psObjInfo=smbMapObjectInfoE;
							   if(iSel&0x1)
							   {
								   EnableControl(hDlg,IDC_BACKCOLOR);
								   bSel=(BYTE)SendDlgItemMessage(hDlg,IDC_BACKCOLOR,CB_GETCURSEL,0,0);
								   bSel&=0x07;
								   bNewData[1]|=bSel;
							   }
							   else
							   {
								   EnableControl(hDlg,IDC_FIRSTBLOCK);
								   bSel=(BYTE)SendDlgItemMessage(hDlg,IDC_FIRSTBLOCK,CB_GETCURSEL,0,0);
								   bSel&=0x0F;
								   bNewData[1]|=bSel;

								   EnableControl(hDlg,IDC_VIEW);								   
								   bSel=(BYTE)SendDlgItemMessage(hDlg,IDC_VIEW,CB_GETCURSEL,0,0);
								   bSel&=0x03;
								   bNewData[1]|=(bSel<<4);

							   }
						   }
						   break;
						   case 0x0F:psObjInfo=smbMapObjectInfoF;break;
						   default:psObjInfo=smbMapObjectInfo0B;break;
						   }

					   //
					   bNewData[1]|=psObjInfo[iSel].bBasicData;

					   //
					   if(psObjInfo[iSel].bIsSizeValid)
					   {
						   iSize=GetDlgItemInt(hDlg,IDC_LEN,&blSuccess,FALSE);
						   if(!blSuccess) return FALSE;
						   //増加分を足してから、マスクすることは、重要。例えば、長さ８の土管の入力。
						   iSize+=psObjInfo[iSel].iSizeDelta;

						   if(iSize<0 || iSize>psObjInfo[iSel].bSizeMask) return TRUE;
						   bNewData[1]+=iSize;
					   }
					   //
					   undoPrepare(UNDONAME_DLGEDIT);
					   memcpy(bBuf,bNewData,2);
					   SetMapData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bNewData);
					   }

					   UpdateObjectList(0);
					   UpdateObjectView(0);
					   ObjectListSetCursor(GetSelectedIndex());
				   }
			   case IDCANCEL:
				   EndDialog(hDlg, TRUE);
                   return TRUE;
			   case IDC_YPOS:
				   if(HIWORD(wParam)==EN_CHANGE)
				   {
					   BYTE bTmp;
					   BOOL blSuccess;

					   //Yの取得
					   bTmp=(BYTE)GetDlgItemInt(hDlg,IDC_YPOS,&blSuccess,FALSE);
					   if(!blSuccess) return FALSE;
					   bBuf[0]&=0xF0;
					   bBuf[0]|=(bTmp&0x0F);

					   PrepareMapComEditDlg(hDlg,bBuf,iPage,PREPAREDLG_MAPCOMEDIT_YPOS);
					   return TRUE;
				   }
				   break;
			   case IDC_XPOS:
				   if(HIWORD(wParam)==EN_CHANGE)
				   {
					   BYTE bTmp;
					   BOOL blSuccess;

					   bTmp=(BYTE)GetDlgItemInt(hDlg,IDC_XPOS,&blSuccess,FALSE);
					   if(!blSuccess) return FALSE;
					   bBuf[0]&=0x0F;
					   bBuf[0]|=(bTmp<<4);

					   //PrepareMapComEditDlg(hDlg,bBuf,iPage,PREPAREDLG_MAPCOMEDIT_BININPUTONLY);
					   return TRUE;
				   }
				   break;
			   case IDC_LEN:
				   if(HIWORD(wParam)==EN_CHANGE)
				   {

					   return TRUE;
				   }
				   break;
			   case IDC_TYPE://種類を変更
				   if(HIWORD(wParam)==CBN_SELCHANGE)
				   {
					   BYTE bTmp;
					   int iSel;
					   BOOL blSuccess;
					   SMBMAPOBJECTINFO* psObjInfo;

					   bTmp=(BYTE)GetDlgItemInt(hDlg,IDC_YPOS,&blSuccess,FALSE);
					   if(!blSuccess) return TRUE;

					   iSel=SendDlgItemMessage(hDlg,IDC_TYPE,CB_GETCURSEL,0,0);
					   if(iSel==CB_ERR) return TRUE;

					   switch(bTmp&0x0F)
					   {
						   case 0x0C:psObjInfo=smbMapObjectInfoC;break;
						   case 0x0D:psObjInfo=smbMapObjectInfoD;break;
						   case 0x0E:
						   {
							   psObjInfo=smbMapObjectInfoE;
							   if(iSel&0x1)
								   bBuf[1]|=0x40;
							   else
								   bBuf[1]&=0xBF;
						   }
						   break;
						   case 0x0F:psObjInfo=smbMapObjectInfoF;break;
						   default:psObjInfo=smbMapObjectInfo0B;break;
					   }

					   //
					   if((bTmp&0x0F)!=0x0E)
					   {
						   bBuf[1]&=psObjInfo[iSel].bSizeMask;
						   bBuf[1]|=psObjInfo[iSel].bBasicData;
					   }
					   PrepareMapComEditDlg(hDlg,bBuf,iPage,PREPAREDLG_MAPCOMEDIT_ALL);
					   return TRUE;
				   }
				   break;
			   case IDC_PAGEFLAG:
				   if(HIWORD(wParam)==BN_CLICKED)
				   {				   

					   if(IsDlgButtonChecked(hDlg,IDC_PAGEFLAG)&BST_CHECKED)
					   {
						   if(!(bBuf[1]&0x80)) iPage++;
						   bBuf[1]|=0x80;
					   }
					   else
					   {
						   if(bBuf[1]&0x80) iPage--;
						   bBuf[1]&=0x7F;
					   }
					   //PrepareMapComEditDlg(hDlg,bBuf,iPage,PREPAREDLG_MAPCOMEDIT_BININPUTONLY);
					   return TRUE;
				   }
				   break;
			   case IDC_BIN:
				   {
					   if(HIWORD(wParam)==EN_CHANGE)
						   CheckDlgButton(hDlg,IDC_ISBIN,BST_CHECKED);
					   return TRUE;
				   }
				   break;
			   }
		   }
		   break;

   }
   return FALSE;
}