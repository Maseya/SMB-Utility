/************************************************************************************

                                  smb Utility

  File: objlist.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "roommng.h"
#include "objlist.h"
#include "objlib.h"
#include "objmng.h"
#include "objview.h"
#include "ini.h"
#include "objwndcmn.h"
/************************

  ユーザー定義メッセージ

*************************/
#define WM_LVSELCHANGE (WM_USER + 100) 

/******************

  グローバル変数

*******************/

//リストビューのハンドル
HWND g_hWndListView;





/***************

補助関数

***************/
//リストボックス
void ObjectListClear()
{
	ListView_DeleteAllItems(g_hWndListView);
}

void ObjectListSetCursor(int iIndex)
{
	// clear cursor
	ListView_SetItemState(g_hWndListView,-1,~(LVIS_SELECTED|LVIS_FOCUSED),LVIS_SELECTED|LVIS_FOCUSED);
	// set cursor
	ListView_SetItemState(g_hWndListView,iIndex,LVIS_SELECTED|LVIS_FOCUSED ,LVIS_SELECTED|LVIS_FOCUSED );
}

void ObjectListShowCursor()
{
	ListView_EnsureVisible(g_hWndListView,GetSelectedIndex(),FALSE);
}

//グローバルな変数の初期化
void InitMapEditGlobalValue()
{
	SetSelectedItem(0,TRUE);
}

/*************************************************

  マップの情報の表示

***************************************************/
#include "objdata.h"


void FormatMapString(LPBYTE lpbBuf,LPTSTR lpszBuf)
{
	switch((lpbBuf[0]&0x0f))
	{
	case 0x0C:
		wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfoC[(lpbBuf[1] >> 4) & 0x07].Name);
		break;
	case 0x0D:
		if(!(lpbBuf[1]&0x40))
			wsprintf(lpszBuf, "%s:%.2d", smbMapObjectInfoD[0].Name, lpbBuf[1] & 0x3F);
		else{
			if((lpbBuf[1]&0x70)==0x40)
				wsprintf(lpszBuf, "%s", smbMapObjectInfoD[(lpbBuf[1] & 0x0F) + 1].Name);
			else
				wsprintf(lpszBuf, STRING_OBJLIST_UNKNOWN);
		}
		break;
	case 0x0E:
		if(lpbBuf[1]&0x40)
			wsprintf(lpszBuf, STRING_OBJLIST_BACK, smbMapHeadBackColor[lpbBuf[1] & 0x07]);
		else
			wsprintf(lpszBuf, STRING_OBJLIST_VIEWBLOCK, smbMapHeadView[(lpbBuf[1] >> 4) & 0x03], smbMapBasicBlock[lpbBuf[1] & 0x0F].Name);
		break;
	case 0x0F:
		{
			switch((lpbBuf[1]>>4)&0x07)
			{
			case 0:
				wsprintf(lpszBuf, STRING_OBJLIST_ROPE);
				break;
			// オブジェクトのデータベースの都合(1と4,5との間には、追加の要素が1つ入っているため)により、
			// 1と4,5は違う処理
			case 1:
				wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfoF[((lpbBuf[1] >> 4) & 0x07)].Name);
				break;
			case 4:
			case 5:
				wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfoF[((lpbBuf[1] >> 4) & 0x07) + 1].Name);
				break;
			case 2:
				{
					BYTE bHeight;
					bHeight = lpbBuf[1] & 0x0F;
					if(0x00 <= bHeight && bHeight <= 0x0B)
						wsprintf(lpszBuf, STRING_OBJLIST_LENCASTLE, 0x0B - bHeight + 1);
					else
						wsprintf(lpszBuf, STRING_OBJLIST_CRASH);
				}
				break;
			case 3:
				if(!((lpbBuf[1]>>3)&0x01))
					wsprintf(lpszBuf,STRING_OBJLIST_STEP,(lpbBuf[1]&0x07)+1,(lpbBuf[1]&0x07)+1);
				else
				{
					BYTE bLower;
					bLower=(lpbBuf[1]&0x07);
					if(0<= bLower && bLower <=0x03)
						wsprintf(lpszBuf, STRING_OBJLIST_STEP98);
					else if(0x04<= bLower && bLower <=0x06)
						wsprintf(lpszBuf, STRING_OBJLIST_STEP98_2);
					else if(0x07==bLower)
						wsprintf(lpszBuf, STRING_OBJLIST_STEP98_2);
				}
				break;
			case 6:
			case 7:
				wsprintf(lpszBuf, STRING_OBJLIST_NONE);
				break;
			}
		}
		break;
	default:
		if(!(lpbBuf[1]&0x70)){
			wsprintf(lpszBuf,"%s",smbMapObjectInfo0B[lpbBuf[1] & 0x0F].Name);
		}
		else if((lpbBuf[1]&0x70)!=0x70){
			wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfo0B[0x0F + ((lpbBuf[1] >> 4) & 0x07)].Name);
		}
		else{
			if(lpbBuf[1]&0x08)//土管
				wsprintf(lpszBuf, STRING_OBJLIST_LENNAME,(lpbBuf[1] & 0x07) + 1, smbMapObjectInfo0B[0x17].Name);
			else
				wsprintf(lpszBuf, STRING_OBJLIST_LENNAME,(lpbBuf[1] & 0x0F) + 1, smbMapObjectInfo0B[0x16].Name);
		}
		break;
	}/* switch */
}

static BOOL smbMapCommand(BOOL blQuietUpdate)
{
	OBJECTSEEKINFO ObjSeek;
	int n;

	if(MapSeekFirst(&ObjSeek,GETADDRESS_CURRENT_EDITTING))
	{
		for(n=0;;n++)
		{
			LPTSTR cBuf = GetTempStringBuffer();
			LVITEM lvItem;

			//バイナリ
			wsprintf(cBuf,"%.2x %.2x",ObjSeek.pbData[0],ObjSeek.pbData[1]);
			
			if(blQuietUpdate){
				BYTE bBuf[2];
				LPTSTR cCurText = GetTempStringBuffer2();
				lvItem.iItem=n;
				lvItem.iSubItem=0;
				lvItem.mask=LVIF_TEXT;
				lvItem.pszText=cCurText;
				lvItem.cchTextMax=TMPSTRBUFSIZ;
				if(ListView_GetItem(g_hWndListView,&lvItem)){
					sscanf(cCurText,"%x %x",&bBuf[0],&bBuf[1]);
					if(!memcmp(bBuf,ObjSeek.pbData,2)) 
						goto CANCEL_SET_ITEM_TEXT;
				}
				ListView_SetItemText(g_hWndListView,n,0,cBuf);
			}
			else{
				memset(&lvItem,0,sizeof(LVITEM));
				lvItem.mask=LVIF_TEXT;
				lvItem.iItem=n;
				lvItem.iSubItem=0;
				lvItem.pszText=cBuf;		
				ListView_InsertItem(g_hWndListView,&lvItem);
			}

			//ページ
			wsprintf(cBuf,"%d",ObjSeek.dwPage);
			ListView_SetItemText(g_hWndListView,n,1,cBuf);
			//位置
			wsprintf(cBuf,"(%d,%d)",GetMapXPos(ObjSeek.pbData),GetMapYPos(ObjSeek.pbData));
			ListView_SetItemText(g_hWndListView,n,2,cBuf);
			//種類
			FormatMapString(ObjSeek.pbData,cBuf);
			ListView_SetItemText(g_hWndListView,n,3,cBuf);

CANCEL_SET_ITEM_TEXT:
		if(!MapSeekNext(&ObjSeek)) break;
		}/* for */
	}/* if */

	return TRUE;
}

/*************************************************

  敵の情報の表示


***************************************************/
void FormatBadGuysString(LPBYTE lpbBuf,LPTSTR lpszBuf)
{
	LPSTR bit6[]={"", STRING_OBJLIST_HARD};

	switch(lpbBuf[0]&0x0F)
	{
	case 0x0E://ルーム間移動の命令（３バイト）
		{
			LPTSTR lpAttr[] = {STRING_SEA, STRING_SKY, STRING_UNDERGROUND, STRING_CASTLE}; 
			wsprintf(lpszBuf, STRING_OBJLIST_ROOM, lpbBuf[1] & 0x7F, lpAttr[(lpbBuf[1] >> 5) & 0x03], ((lpbBuf[2] >> 5) & 0x07) + 1,lpbBuf[2] & 0x1F);
		}
		break;
	case 0x0F://送りコマンド（２バイト）
		wsprintf(lpszBuf, STRING_OBJLIST_PAGECOMMAND, lpbBuf[1] & 0x3F);
		break;
	default://（敵キャラコマンド）
		{
			wsprintf(lpszBuf, "%s%s", smbBudGuysInfo[lpbBuf[1] & 0x3f].Name, bit6[(lpbBuf[1] >> 6) & 0x01]);
		}
	}
}
static BOOL smbBadGuysCommand(BOOL blQuietUpdate)
{
	OBJECTSEEKINFO ObjSeek;
	int n;

	if(BadGuysSeekFirst(&ObjSeek,GETADDRESS_CURRENT_EDITTING))
	{
		for(n=0;;n++)
		{
			LPTSTR cBuf = GetTempStringBuffer();
			LVITEM lvItem;

			//バイナリ
			if(ObjSeek.dwObjLen==2)
				wsprintf(cBuf,"%.2x %.2x",ObjSeek.pbData[0],ObjSeek.pbData[1]);
			else
				wsprintf(cBuf,"%.2x %.2x %.2x",ObjSeek.pbData[0],ObjSeek.pbData[1],ObjSeek.pbData[2]);

			if(blQuietUpdate){
				BYTE bBuf[3];
				LPTSTR cCurText = GetTempStringBuffer2();
				int iSizeLV,iSizeSrc;
				lvItem.iItem=n;
				lvItem.iSubItem=0;
				lvItem.mask=LVIF_TEXT;
				lvItem.pszText=cCurText;
				lvItem.cchTextMax=TMPSTRBUFSIZ;
				if(ListView_GetItem(g_hWndListView,&lvItem)){
					iSizeLV=sscanf(cCurText,"%x %x %x",&bBuf[0],&bBuf[1],&bBuf[2]);
					iSizeSrc=BadGuysGetDataLength(ObjSeek.pbData);
					if(iSizeLV==iSizeSrc && !memcmp(bBuf,ObjSeek.pbData,iSizeSrc)) 
						goto CANCEL_SET_ITEM_TEXT;
					ListView_SetItemText(g_hWndListView,n,0,cBuf);
				}
			}
			else{
				memset(&lvItem,0,sizeof(LVITEM));
				lvItem.mask=LVIF_TEXT;
				lvItem.iItem=n;
				lvItem.iSubItem=0;
				lvItem.pszText=cBuf;		
				ListView_InsertItem(g_hWndListView,&lvItem);
			}
			//ページ
			wsprintf(cBuf,"%d",ObjSeek.dwPage);
			ListView_SetItemText(g_hWndListView,n,1,cBuf);
			//位置
			wsprintf(cBuf,"(%d,%d)",GetBadGuysXPos(ObjSeek.pbData),GetBadGuysYPos(ObjSeek.pbData));
			ListView_SetItemText(g_hWndListView,n,2,cBuf);
			//種類
			FormatBadGuysString(ObjSeek.pbData,cBuf);
			ListView_SetItemText(g_hWndListView,n,3,cBuf);			
CANCEL_SET_ITEM_TEXT:
			if(!BadGuysSeekNext(&ObjSeek)) break;
		}/* for */
	}/* if */
	return TRUE;
}

/*************************************************

  リストボックスの更新


***************************************************/
void UpdateObjectList(DWORD dwUpdateFlag)
{
	if(!gblIsROMLoaded) return;
	
	//Set redraw mode and delete all items
	SendMessage(g_hWndListView,WM_SETREDRAW,(WPARAM)FALSE,0);
	if(!(dwUpdateFlag&1))
		ListView_DeleteAllItems(g_hWndListView);

	if(GetMapEditMode())
		smbBadGuysCommand(dwUpdateFlag&1);
	else
		smbMapCommand(dwUpdateFlag&1);
	
	ObjectListSetCursor(GetSelectedIndex());

	SendMessage(g_hWndListView,WM_SETREDRAW,(WPARAM)TRUE,0);
}


/**************************

  マップの設定の変更

***************************/

#define RESTART_PAGE_ADDRESS 0x91F7

int GetHalfPointPage(int w,int a)
{
	BYTE bTmp;
	ADDRESSDATA adRestartPageAddress;

	ADDRESSDATA_LOAD(adRestartPageAddress,RESTART_PAGE_ADDRESS);

	bTmp=*(bPRGROM+ADDRESSDATA_GET(adRestartPageAddress)+w*2+((a>>1)&0x01));
	bTmp>>=(!(a&0x01))*4;
	bTmp&=0x0F;

	return bTmp;
}

LRESULT CALLBACK MapComHeadEditDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
   switch (message)
   {
       case WM_INITDIALOG:
		   {
			   register int n;
			   BYTE bRoomID;
			   BYTE bBuf[2];
			   LPTSTR AttrName[]={STRING_SEA,STRING_SKY,STRING_UNDERGROUND,STRING_CASTLE};

			   //ルームの属性
			   for(n=0;n<4;n++)
				   SendDlgItemMessage(hDlg,IDC_MAPATTR,CB_ADDSTRING,0,(LPARAM)AttrName[n]);
			   bRoomID=GetRoomID();
			   SendDlgItemMessage(hDlg,IDC_MAPATTR,CB_SETCURSEL,(bRoomID>>5)&0x03,0);

			   //途中からのページ
			   for(n=0;n<16;n++){
				   TCHAR cTmp[3];
				   wsprintf(cTmp,"%d",n);
				   SendDlgItemMessage(hDlg,IDC_PAGE,CB_ADDSTRING,0,(LPARAM)cTmp);
			   }

			   if(!rm_IsSubRoom())
			   {
				   SendDlgItemMessage(hDlg,IDC_PAGE,CB_SETCURSEL,GetHalfPointPage(g_iWorld,g_iArea),0);
			   }
			   else{
				   	EnableWindow(GetDlgItem(hDlg,IDC_PAGE),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_STATIC_PAGE),FALSE);
			   }

			   GetMapHeadData(GETADDRESS_CURRENT_EDITTING,bBuf); 
			   for(n=0;n<4;n++)
				   SendDlgItemMessage(hDlg,IDC_TIME,CB_ADDSTRING,0,(LPARAM)smbMapHeadTime[n]);
			   SendDlgItemMessage(hDlg,IDC_TIME,CB_SETCURSEL,(WPARAM)((bBuf[0]>>6)&0x03),0);
			   for(n=0;n<8;n++)
				   SendDlgItemMessage(hDlg,IDC_POSITION,CB_ADDSTRING,0,(LPARAM)smbMapHeadPosition[n]);
			   SendDlgItemMessage(hDlg,IDC_POSITION,CB_SETCURSEL,(WPARAM)((bBuf[0]>>3)&0x07),0);
			   for(n=0;n<8;n++)
				   SendDlgItemMessage(hDlg,IDC_BACKCOLOR,CB_ADDSTRING,0,(LPARAM)smbMapHeadBackColor[n]);
			   SendDlgItemMessage(hDlg,IDC_BACKCOLOR,CB_SETCURSEL,bBuf[0]&0x07,0);
			   for(n=0;n<4;n++)
				   SendDlgItemMessage(hDlg,IDC_MAPTYPE,CB_ADDSTRING,0,(LPARAM)smbMapHeadMapType[n]);
			   SendDlgItemMessage(hDlg,IDC_MAPTYPE,CB_SETCURSEL,(bBuf[1]>>6)&0x03,0);
			   for(n=0;n<4;n++)
				   SendDlgItemMessage(hDlg,IDC_VIEW,CB_ADDSTRING,0,(LPARAM)smbMapHeadView[n]);
			   SendDlgItemMessage(hDlg,IDC_VIEW,CB_SETCURSEL,(bBuf[1]>>4)&0x03,0);
			   for(n=0;n<16;n++)
				   SendDlgItemMessage(hDlg,IDC_FIRSTBLOCK,CB_ADDSTRING,0,(LPARAM)smbMapBasicBlock[n].Name);
			   SendDlgItemMessage(hDlg,IDC_FIRSTBLOCK,CB_SETCURSEL,bBuf[1]&0x0F,0);
			   
			   return TRUE;
		   }
       case WM_COMMAND:
               switch(LOWORD(wParam))
			   {
			   case IDOK:
				   {
					   BYTE bBuf[2]={0};
					   BYTE bTmp;
					   int iNewAttr;

					   undoPrepare(UNDONAME_HEADDLG);

					   bTmp=(BYTE)SendDlgItemMessage(hDlg,IDC_TIME,CB_GETCURSEL,0,0);
					   bBuf[0]|=((bTmp&0x3)<<6);
					   bTmp=(BYTE)SendDlgItemMessage(hDlg,IDC_POSITION,CB_GETCURSEL,0,0);
					   bBuf[0]|=((bTmp&0x7)<<3);
					   bTmp=(BYTE)SendDlgItemMessage(hDlg,IDC_BACKCOLOR,CB_GETCURSEL,0,0);
					   bBuf[0]|=(bTmp&0x07);
					   bTmp=(BYTE)SendDlgItemMessage(hDlg,IDC_MAPTYPE,CB_GETCURSEL,0,0);
					   bBuf[1]|=((bTmp&0x3)<<6);
					   bTmp=(BYTE)SendDlgItemMessage(hDlg,IDC_VIEW,CB_GETCURSEL,0,0);
					   bBuf[1]|=((bTmp&0x3)<<4);
					   bTmp=(BYTE)SendDlgItemMessage(hDlg,IDC_FIRSTBLOCK,CB_GETCURSEL,0,0);
					   bBuf[1]|=(bTmp&0x0F);

					   memcpy(bPRGROM+GetMapAddress(GETADDRESS_CURRENT_EDITTING),bBuf,2);

					   iNewAttr=SendDlgItemMessage(hDlg,IDC_MAPATTR,CB_GETCURSEL,0,0);
					   
					   ChangeRoomAttribute(GetRoomID(),iNewAttr&0x03);

					   //途中から
					   if(!rm_IsSubRoom())
					   {
						   BYTE bTmp;
						   BYTE bMask=0xF0;
						   BYTE bSel;
						   ADDRESSDATA adRestartPageAddress;

						   //memcpy(&adRestartPageAddress.byte.bLower,bPRGROM+RESTART_PAGE_ADDRESS,2);
						   ADDRESSDATA_LOAD(adRestartPageAddress,RESTART_PAGE_ADDRESS);
						   
						   bSel=(BYTE)SendDlgItemMessage(hDlg,IDC_PAGE,CB_GETCURSEL,0,0);
						   bSel<<=(!(g_iArea&0x01))*4;
						   bMask>>=(!(g_iArea&0x01))*4;
						   bTmp=bPRGROM[ADDRESSDATA_GET(adRestartPageAddress)+g_iWorld*2+((g_iArea>>1)&0x01)];

						   bTmp&=bMask;
						   bTmp|=bSel;

						   if(g_iWorld>=0 && g_iWorld<GetNumWorlds())//ROMの保護のために絶対に必要
							   bPRGROM[ADDRESSDATA_GET(adRestartPageAddress)+g_iWorld*2+((g_iArea>>1)&0x01)]=bTmp;
					   }

//					   gblDataChanged = TRUE;
					   fr_SetDataChanged(TRUE);
					   
					   UpdateObjectList(0);
					   //RefreshWindowTitle();
					   UpdateObjectView(0);
					   UpdateStatusBarRoomInfoText(NULL);
				   }
			   case IDCANCEL: 
				   EndDialog(hDlg, TRUE);
                       return TRUE;
			   }
   }
   return FALSE;
}



/**********************


***********************/

static void ResizeListView(HWND hwndListView, HWND hwndParent)
{
	RECT  rc;

	GetClientRect(hwndParent, &rc);

	MoveWindow(hwndListView,rc.left,rc.top,rc.right - rc.left,rc.bottom - rc.top,TRUE);
}


static HWND CreateListView(HWND hwndParent)
{
	HWND        hwndListView;
	BOOL        bSuccess = TRUE;

	hwndListView = CreateWindowEx(0,          // ex style
                                 WC_LISTVIEW,               // class name - defined in commctrl.h
                                 "",                        // dummy text
                                 WS_TABSTOP | WS_CHILD | WS_VISIBLE |
								 LVS_AUTOARRANGE | LVS_REPORT |
								 LVS_SHOWSELALWAYS | LVS_SINGLESEL,                   // style
                                 0,                         // x position
                                 0,                         // y position
                                 0,                         // width
                                 0,                         // height
                                 hwndParent,                // parent
                                 (HMENU)IDW_LISTVIEW,        // ID
                                 GetModuleHandle(NULL),                   // instance
                                 NULL);                     // no extra data

	if(!hwndListView) return NULL;


//	if(IsCommonControlSupported(COMCTRL_V470))
//		ListView_SetExtendedListViewStyle(hwndListView,LVS_EX_FULLROWSELECT);


	ResizeListView(hwndListView, hwndParent);

	
	return hwndListView;
}

#define LISTVIEW_COLUMNS 4

static void LoadListViewColumnWidth(int *piWidth)
{
	int n;
	DWORD dwWidth;
	LPTSTR lpRegKey[LISTVIEW_COLUMNS]={INI_OBJECTLIST_COLUMNWIDTH0,
	                                   INI_OBJECTLIST_COLUMNWIDTH1,
									   INI_OBJECTLIST_COLUMNWIDTH2,
									   INI_OBJECTLIST_COLUMNWIDTH3};
	for(n=0;n<4;n++){
		if(ReadFromRegistry(lpRegKey[n],REG_DWORD,&dwWidth,sizeof(DWORD))){
			piWidth[n]=dwWidth;
		}
	}

}

static BOOL InitListView(HWND hwndListView)
{
	LV_COLUMN lvColumn;
	int i;
	TCHAR szString[LISTVIEW_COLUMNS][20] = {STRING_OBJLIST_COLUMN_BIN,
											STRING_OBJLIST_COLUMN_PAGE,
											STRING_OBJLIST_COLUMN_POS,
											STRING_OBJLIST_COLUMN_TYPE};
	int iWidth[LISTVIEW_COLUMNS]={50,45,55,200};

	//empty the list
	ListView_DeleteAllItems(hwndListView);

	//
	LoadListViewColumnWidth(iWidth);

	//initialize the columns
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	for(i=0;i<LISTVIEW_COLUMNS;i++)
	{
		lvColumn.cx = iWidth[i];
		lvColumn.pszText = szString[i];
		ListView_InsertColumn(hwndListView, i, &lvColumn);
	}
	return TRUE;
}

static void SaveListViewColumnWidth()
{
	int n;
	DWORD dwWidth;
	LPTSTR lpRegKey[LISTVIEW_COLUMNS]={INI_OBJECTLIST_COLUMNWIDTH0,
	                                    INI_OBJECTLIST_COLUMNWIDTH1,
										INI_OBJECTLIST_COLUMNWIDTH2,
										INI_OBJECTLIST_COLUMNWIDTH3};

	for(n=0;n<4;n++){
		dwWidth=(DWORD)ListView_GetColumnWidth(g_hWndListView,n);
		WriteToRegistry(lpRegKey[n],REG_DWORD,&dwWidth,sizeof(DWORD));
	}
}


/********************

  メインウインドウ

*********************/
long FAR PASCAL MapEditWndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )
{
    switch( message )
    {
	case WM_SIZE:
		{
			ResizeListView(g_hWndListView,hWnd);
		}
		break;
	case WM_LVSELCHANGE:
		{
			int iRet;
			
			iRet=ListView_GetNextItem(g_hWndListView,-1,LVNI_SELECTED);
			if(iRet == -1) break;
			SetSelectedItem(iRet,TRUE);
			UpdateObjectViewCursole();
			UpdateObjectView(0);
		}
		break;
	case WM_NOTIFY:
		{
			LPNMHDR  lpnmh = (LPNMHDR) lParam;
			switch(lpnmh->code)
			{
			case NM_RDBLCLK:
			case NM_DBLCLK:
			case NM_RETURN:
				{
					int iRet;
					iRet = ListView_GetNextItem(g_hWndListView,-1,LVNI_SELECTED);
					if(iRet == -1) break;
					iRet = GetSelectedIndex();
					if(GetMapEditMode())
						DialogBox(GetModuleHandle(NULL),"BADGUYSCOMEDITDLG",hWnd,BadGuysComEditDlgProc);
					else
						DialogBox(GetModuleHandle(NULL),"MAPCOMEDITDLG",hWnd,MapComEditDlgProc);

				}
				break;
			case NM_RCLICK:
				if(!gblIsROMLoaded) break;
				DialogBox(GetModuleHandle(NULL),"SENDOBJECTDLG",hWnd,SendObjectDlgProc);
				break;
			case NM_CLICK:
			case LVN_KEYDOWN:
				{
					PostMessage(hWnd,WM_LVSELCHANGE,0,0);
				}
				break;
			case NM_SETFOCUS:
//			case NM_KILLFOCUS:
				{
					if(gblIsROMLoaded){
						// set a scroll bar ensureing cursole is visible 
						ListView_EnsureVisible(g_hWndListView,GetSelectedIndex(),FALSE);
					}
				}
				break;
			}
		}
		break;
	case WM_SETFOCUS:
		//キーボードフォーカスをリストボックスへ
		SetFocus(g_hWndListView);
		break;
	case WM_CREATE:
		{
			g_hWndListView=CreateListView(hWnd);
			InitListView(g_hWndListView);
			//DisableIME
			ImmAssociateContext(g_hWndListView, (HIMC)NULL);
			UpdateObjectList(0);
			return 0;
		}
	case WM_DESTROY:
		{
			SaveListViewColumnWidth();
			DestroyWindow(g_hWndListView);
			g_hWndListView=NULL;
		}
		break;
	case WM_SYSCOMMAND:
		{
			if(wParam==SC_CLOSE) return 0;
		}
		break;
	default:
		return DefMDIChildProc (hWnd, message, wParam, lParam);
	}
	return DefMDIChildProc (hWnd, message, wParam, lParam);
} /* MainWndProc */

#define MAPEDITWNDCLASSNAME  "MAPEDITWND"

BOOL RegisterMapEditWndClass(HINSTANCE hInstance)
{
    WNDCLASS            wc;
    #define CBWNDEXTRA      12
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
    wc.lpfnWndProc = MapEditWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra    = CBWNDEXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, "MAPLISTICON" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = MAPEDITWNDCLASSNAME;

    if(!RegisterClass(&wc))return FALSE;
    
	return TRUE;   
}

HWND CreateMapEditWnd(HINSTANCE hInstance,HWND hWndMDIClient)
{
	HWND hWnd;

	//WS_VISIBLEを指定して作成しないと、Windowﾒﾆｭｰにｳｲﾝﾄﾞｳが追加されない。
	hWnd=CreateMDIWindow(MAPEDITWNDCLASSNAME,
		            STRING_WINDOW_OBJLIST,
					WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_VISIBLE,
					90,//CW_USEDEFAULT,
					60,//CW_USEDEFAULT,
					GetSystemMetrics(SM_CXSCREEN)/2,
					GetSystemMetrics(SM_CYSCREEN)/2,
					hWndMDIClient,//
					hInstance,
					0);
	return hWnd;
}