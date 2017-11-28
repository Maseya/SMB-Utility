/************************************************************************************

                                  smb Utility

  File: objeditcom.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "roommng.h"
#include "objmng.h"
#include "objlib.h"
#include "objview.h"
#include "objlist.h"
#include "objeditcom.h"
#include "ini.h"

/***********************

  キーボード入力ハンドラ

************************/
static int SetDataNextPageBadGuys(int iItem)
{
	//移動するデータ
	BYTE bBuf[3];
	int  iBufSize;
	BOOL blIsPage;
	//
	BYTE *pbBuf;
	//
	BYTE *pbPrev;
	//
	OBJECTSEEKINFO ObjSeek,ObjSeekTmp;
	BYTE bNextPageXPos;
	int iMaxIndex;
	DWORD dwPrevPageRelated = PAGEOBJECT_NO;

	if(!BadGuysSeekFirst(&ObjSeek,GETADDRESS_CURRENT_EDITTING)) return iItem;
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iItem) break;
		dwPrevPageRelated = BadGuysIsPageRelatedObject(ObjSeek.pbData);
		if(!BadGuysSeekNext(&ObjSeek)) return INVALID_OBJECT_INDEX;
	}
	//
	pbPrev=pbBuf=ObjSeek.pbData;
	iBufSize = ObjSeek.dwObjLen;
	memcpy(bBuf,ObjSeek.pbData,iBufSize);
	blIsPage=(bBuf[1]&0x80)?TRUE:FALSE;

	//Check if this page is last one.if so return.
	memcpy(&ObjSeekTmp,&ObjSeek,sizeof(OBJECTSEEKINFO));
	for(iMaxIndex=iItem;;iMaxIndex++){
		if(ObjSeekTmp.dwPage!=ObjSeek.dwPage) break;
		if(!BadGuysSeekNext(&ObjSeekTmp)) return INVALID_OBJECT_INDEX;
	}

	if(!BadGuysSeekNext(&ObjSeek)) return INVALID_OBJECT_INDEX;
	iItem++;

	//移動するﾃﾞｰﾀが改ページフラグを持っていて、かつ次のデータがページを構成する
	//オブジェクトである場合か、以下のような並びの（＊）のオブジェクト
	// ページ送りコマンド
	// (改ページフラグあり・なしの両方の)横位置15のオブジェクト…（＊）
	//は、移動させない。
	{
		BOOL blSetPage = (dwPrevPageRelated == PAGEOBJECT_SETPAGE);
		BOOL blNextPageRelated = ((ObjSeek.pbData[1]&0x80)||((ObjSeek.pbData[0]&0x0f)==0x0F));
		if(    (blIsPage  && blNextPageRelated)
			|| (blIsPage  && blSetPage)
			|| (blSetPage && blNextPageRelated))
		return INVALID_OBJECT_INDEX;
	}
	// 同じ横位置15にあるオブジェクトをとばす。
	// データが変更されるため、BadGuysSeek()関数は、使えないことに注意
	for(;iItem<=iMaxIndex;iItem++)
	{
		BYTE bTmp[3];
		int iTmpSize;
		// コピーを保存しておく
		iTmpSize=((ObjSeek.pbData[0]&0x0F)==0x0E)?3:2;
		memcpy(bTmp,ObjSeek.pbData,iTmpSize);
		// データを移動
		memcpy(pbPrev,bTmp,iTmpSize);
		pbPrev+=iTmpSize;
		//
		if(bTmp[1]&0x80){
			bNextPageXPos=0;
			break;
		}
		else if((bTmp[0]&0x0f)==0x0F){
			bNextPageXPos=(bTmp[0]&0xF0);
			break;
		}
		//
		ObjSeek.pbData+=iTmpSize;
	}

	//移動したデータにページフラッグがある場合
	if(blIsPage)
		pbBuf[1]|=0x80;

	//
	bBuf[0]&=0x0F;
	bBuf[0]|=bNextPageXPos;
	bBuf[1]&=0x7F;
	memcpy(pbPrev,bBuf,iBufSize);

	return iItem;
}

static int SetDataPrevPageBadGuys(int iItem)
{
	BYTE bBuf[3];
	int iBufSize;
	BYTE *pbBuf;
	BYTE *pbPageTop;
	int n=0;
	BOOL blIsPage;//移動するデータ
	OBJECTSEEKINFO ObjSeek;
	DWORD dwPrevPageRelated = PAGEOBJECT_NO;

	if(!BadGuysSeekFirst(&ObjSeek,GETADDRESS_CURRENT_EDITTING))
		return INVALID_OBJECT_INDEX;
	for(;;){
		
		if(ObjSeek.dwIndex==(DWORD)iItem){
			if(!ObjSeek.dwPage)
				return INVALID_OBJECT_INDEX;
			break;
		}
		
		if( ((ObjSeek.pbData[0]&0x0F)==0x0F)
			|| (ObjSeek.pbData[1]&0x80)  )
		{
			pbPageTop=ObjSeek.pbData;
			n=0;
		}

		dwPrevPageRelated = BadGuysIsPageRelatedObject(ObjSeek.pbData);

		if(!BadGuysSeekNext(&ObjSeek))
			return INVALID_OBJECT_INDEX;

		n--;

	}

	pbBuf = ObjSeek.pbData;
	iBufSize = ObjSeek.dwObjLen;
	memcpy(bBuf,ObjSeek.pbData,iBufSize);
	blIsPage=(bBuf[1]&0x80)?TRUE:FALSE;

//	if(!(bBuf[1]&0x80) && iItem==0) return INVALID_OBJECT_INDEX;
	if(!BadGuysSeekNext(&ObjSeek)) return INVALID_OBJECT_INDEX;

	// 移動するﾃﾞｰﾀが改ページフラグを持っていて、かつ次のデータが改ページフラグを持っ
	// ている場合すなわち、そのデータが1ページに1つしかない
	//以下のような並びの（＊）のオブジェクト
	// ページ送りコマンド
	// 改ページフラグなしの横位置15のオブジェクト…（＊）
	// 改ページフラグを持ったオブジェクト
	//は、移動させない。
	if(blIsPage || dwPrevPageRelated == PAGEOBJECT_SETPAGE){
		if( (ObjSeek.pbData[1]&0x80)
			|| ((ObjSeek.pbData[0]&0x0f)==0x0F) )
			return INVALID_OBJECT_INDEX;
	}

	// 移動するデータ自身が改ページフラグを持っている場合、
	// 次のデータに改ページフラグを移動する
	if(blIsPage){
		ObjSeek.pbData[1]|=0x80;

		bBuf[0]|=0xF0;
		bBuf[1]&=0x7F;
		memcpy(pbBuf,bBuf,iBufSize);
		//インデックスの変更なし
		n=0;
	}
	else{
		memmove(pbPageTop+iBufSize,pbPageTop,pbBuf-pbPageTop);
		bBuf[0]|=0xF0;
		memcpy(pbPageTop,bBuf,iBufSize);
	}

	return (iItem+n);
}

static int SetObjIndexBadGuys(int iItem,BOOL fPlus)
{
	OBJECTSEEKINFO ObjSeek;

	if(!fPlus)
		iItem--;
	else
		iItem++;

	if(iItem<0)
		return INVALID_OBJECT_INDEX;

	if(!BadGuysSeekFirst(&ObjSeek,GETADDRESS_CURRENT_EDITTING))
		return INVALID_OBJECT_INDEX;
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iItem){
			break;
		}
		if(!BadGuysSeekNext(&ObjSeek))
			return INVALID_OBJECT_INDEX;
	}

	SetSelectedItem(iItem,FALSE);

	return iItem;
}


//戻り値　リストボックスの更新が必要な場合、TRUE, さもなければ、FALSE
static BOOL BadGuysKeyInput(int iItem,int iVKey)
{
	BYTE bBuf[3];
	BOOL bRet=FALSE;

	if(-1==GetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf,NULL)) return FALSE;
	switch(iVKey)
	{
	case IDM_EDITCOMMAND_DOWN:
		if(bBuf[0]==0xFF) break;
		if((bBuf[0]&0x0F)!=0x0F){
			bBuf[0]++;
			if(SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf))
				bRet=TRUE;
		}
		break;
	case IDM_EDITCOMMAND_UP:
		if(bBuf[0]==0xFF) break;
		if((bBuf[0]&0x0F)!=0x00){
			bBuf[0]--;
			if(SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf))
				bRet=TRUE;
		}
		break;
	case IDM_EDITCOMMAND_RIGHT:
		if((bBuf[0]&0xF0)!=0xF0) 
		{
			bBuf[0]+=0x10;
			if(bBuf[0]==0xFF) break;
			SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
			bRet=TRUE;
		}
		else if((bBuf[0]&0xF0)==0xF0)//次のページへ
		{
			int iCurIndex;
			int iPage;
			BYTE bTmp[3];
			
			iCurIndex=SetDataNextPageBadGuys(iItem);
			if(iCurIndex!=INVALID_OBJECT_INDEX){
				SortByPosXBadGuys(GETADDRESS_CURRENT_EDITTING,&iCurIndex,FALSE);
				SetSelectedItem(iCurIndex,TRUE);//giSelectedItem=iCurIndex;
				//ページ、カーソルの更新
				GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bTmp,&iPage);
				SetMapViewCursoleBadGuys(bTmp,iPage);
				bRet=TRUE;
			}
		}
		break;
	case IDM_EDITCOMMAND_LEFT:
		if((bBuf[0]&0xF0)!=0x00)
		{
			bBuf[0]-=0x10;
			if(bBuf[0]==0xFF) break;
			SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
			bRet=TRUE;
		}
		else if((bBuf[0]&0xF0)==0x00)//前のページへ
		{
			int iCurIndex;
			int iPage;
			BYTE bTmp[3];
			
			if((bBuf[0]|0xF0)==0xFF) break;
			iCurIndex=SetDataPrevPageBadGuys(iItem);
			if(iCurIndex!=INVALID_OBJECT_INDEX){
				SortByPosXBadGuys(GETADDRESS_CURRENT_EDITTING,&iCurIndex,FALSE);
				SetSelectedItem(iCurIndex,TRUE);//giSelectedItem=iCurIndex;

				//ページ、カーソルの更新
				GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bTmp,&iPage);
				SetMapViewCursoleBadGuys(bTmp,iPage);
				bRet=TRUE;
			}
		}
		break;
	case IDM_EDITCOMMAND_ADD1TOTYPE:
		if((bBuf[1]&0x3F)!=0x3F) bBuf[1]++;
		SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
	case IDM_EDITCOMMAND_DEC1TOTYPE:
		if((bBuf[1]&0x3F)!=0x00) bBuf[1]--;
		SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
	case IDM_EDITCOMMAND_ADD16TOTYPE:
		if((bBuf[1]&0x3F)<=0x2F)
			bBuf[1]+=0x10;
		else
			bBuf[1]|=0x3F;
		SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
	case IDM_EDITCOMMAND_DEC16TOTYPE:
		if((bBuf[1]&0x3F)>=0x10)
			bBuf[1]-=0x10;
		else
			bBuf[1]&=0xC0;
		SetBadGuysData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
	case IDM_EDITCOMMAND_REWPAGE:
		{
			int iPage;
			BYTE bBuf[3];
			int iNewIndex;

			GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),NULL,&iPage);
			iNewIndex=GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING,NULL,iPage-1,TRUE);
			if(iNewIndex!=-1) SetSelectedItem(iNewIndex,TRUE);//giSelectedItem=iNewIndex;
			
			bRet=TRUE;

			//カーソルの更新
			GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
			SetMapViewCursoleBadGuys(bBuf,iPage);

		}
		break;
	case IDM_EDITCOMMAND_FWDPAGE:
		{
			int iPage;
			BYTE bBuf[3];
			int iNewIndex;

			GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),NULL,&iPage);
			iNewIndex=GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING,NULL,iPage+1,FALSE);
			if(iNewIndex!=-1) SetSelectedItem(iNewIndex,TRUE);//giSelectedItem=iNewIndex;
			bRet=TRUE;

			//カーソルの更新
			GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
			SetMapViewCursoleBadGuys(bBuf,iPage);
		}
		break;
	case IDM_EDITCOMMAND_NEXTOBJ:
	case IDM_EDITCOMMAND_PREVOBJ:
		{
			int iNewIndex;
			BOOL blPlus;

			blPlus = (iVKey == IDM_EDITCOMMAND_NEXTOBJ) ? TRUE : FALSE;

			if( (iNewIndex = SetObjIndexBadGuys(iItem,blPlus))
				!= INVALID_OBJECT_INDEX)
			{
				int iPage;
				BYTE bBuf[3];
				// 
				bRet = TRUE;
				// カーソルの更新
				GetBadGuysData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
				SetMapViewCursoleBadGuys(bBuf,iPage);
			}
		}
		break;
	}

	return bRet;
}

static int SetObjIndexMap(int iItem,BOOL fPlus)
{
	OBJECTSEEKINFO ObjSeek;

	if(!fPlus)
		iItem--;
	else
		iItem++;

	if(iItem<0)
		return INVALID_OBJECT_INDEX;

	if(!MapSeekFirst(&ObjSeek,GETADDRESS_CURRENT_EDITTING))
		return INVALID_OBJECT_INDEX;
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iItem){
			break;
		}
		if(!MapSeekNext(&ObjSeek))
			return INVALID_OBJECT_INDEX;
	}

	SetSelectedItem(iItem,FALSE);

	return iItem;
}

BOOL MapKeyInput(int iItem,int iVKey)
{
	BYTE bBuf[2];
	BOOL bRet=FALSE;

	if(-1==GetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf,NULL)) return FALSE;
	switch(iVKey)
	{
	case IDM_EDITCOMMAND_DOWN:
		if(bBuf[0]==0xFD) break;
		if((bBuf[0]&0x0F)!=0x0F){
			bBuf[0]++;
			if(SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf))
				bRet=TRUE;
		}
		break;
	case IDM_EDITCOMMAND_UP:
		if(bBuf[0]==0xFD) break;
		if((bBuf[0]&0x0F)!=0x00){
			bBuf[0]--;
			if(SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf))
				bRet=TRUE;
		}
		break;
	case IDM_EDITCOMMAND_RIGHT:
		if((bBuf[0]&0xF0)!=0xF0) 
		{
			bBuf[0]+=0x10;
			if(bBuf[0]==0xFD) break;
			if(SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf))
				bRet=TRUE;
		}
		else if((bBuf[0]&0xF0)==0xF0)//次のページへ
		{
			/*
			bBuf …　移動するデータ
			*/
			BYTE bTmp[3]={0};
			BYTE bNextPageXPos;
			int n=0;
			int i;
			BOOL blIsPage;//移動するデータ

			blIsPage=(bBuf[1]&0x80)?TRUE:FALSE;

			if(blIsPage)//移動するﾃﾞｰﾀが改ページフラグを持っていて、かつ次のデータが改ページフラグを持っている場合すなわち、1ページに1つしかデータがない場合移動させない
			{
				if(-1==GetMapData(GETADDRESS_CURRENT_EDITTING,iItem+1,bTmp,NULL)) break;
				if((bTmp[1]&0x80)||((bTmp[0]&0x0f)==0x0D) && ((bTmp[1]&0x40)==0x00)) goto CANCELNEXT;
			}

			for(i=1;;i++)
			{
				BYTE bData[2];

				if(-1==GetMapData(GETADDRESS_CURRENT_EDITTING,iItem+i,bData,NULL))
					goto CANCELNEXT;
				if(bData[0]==0xFD)
					goto CANCELNEXT;
				if((bData[1]&0x80)||((bData[0]&0x0f)==0x0D) && ((bData[1]&0x40)==0x00))
					break;
			}
			
			for(n=1;;n++)
			{
				if(-1==GetMapData(GETADDRESS_CURRENT_EDITTING,iItem+n,bTmp,NULL)) break;
				if(bTmp[0]==0xFD) goto CANCELNEXT;
				SetMapData(GETADDRESS_CURRENT_EDITTING,iItem+n-1,bTmp);
				if((bTmp[1]&0x80)){
					bNextPageXPos=0;
					break;
				}
				else if(((bTmp[0]&0x0f)==0x0D) && ((bTmp[1]&0x40)==0x00)){
					bNextPageXPos=(bTmp[0]&0xF0);
					break;
				}
			}

			if(blIsPage)//移動するﾃﾞｰﾀが改ページフラグを持ってい場合
			{
				GetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bTmp,NULL);
				bTmp[1]|=0x80;
				SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bTmp);
			}

			bBuf[0]&=0x0F;
			bBuf[0]|=bNextPageXPos;
			bBuf[1]&=0x7F;

			SetMapData(GETADDRESS_CURRENT_EDITTING,iItem+n,bBuf);

			bRet=TRUE;
		}
CANCELNEXT:
		break;
	case IDM_EDITCOMMAND_LEFT:
		if((bBuf[0]&0xF0)!=0x00) 
		{
			bBuf[0]-=0x10;
			if(bBuf[0]==0xFD) break;
			if(SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf))
				bRet=TRUE;
		}
		else if((bBuf[0]&0xF0)==0x00)//前のページへ
		{
			BYTE bTmp[2]={0};
			int n=0;
			int iPage;
			
			if((bBuf[0]|0xF0)==0xFD) break;

			//もし、0ﾍﾟｰｼﾞののデータならキャンセル
			GetMapData(GETADDRESS_CURRENT_EDITTING,iItem,NULL,&iPage);
			if(iPage==0) break;
			
			if(bBuf[1]&0x80)//移動するﾃﾞｰﾀが改ページフラグを持っていて、かつ次のデータが改ページフラグを持っている場合すなわち、1ページに1つしかデータがない場合移動させない
			{
				if(-1!=GetMapData(GETADDRESS_CURRENT_EDITTING,iItem+1,bTmp,NULL)){
					if((bTmp[1]&0x80)||((bTmp[0]&0x0f)==0x0D) && ((bTmp[1]&0x40)==0x00))
						goto CANCELPREV;
				}
			}

			if(bBuf[1]&0x80)
			{
				GetMapData(GETADDRESS_CURRENT_EDITTING,iItem+1,bTmp,NULL);
				bTmp[1]|=0x80;
				SetMapData(GETADDRESS_CURRENT_EDITTING,iItem+1,bTmp);

				bBuf[0]|=0xF0;
				bBuf[1]&=0x7F;
				SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
			}
			else
			{
				for(;;)
				{
					n--;
					if(iItem+n<0) goto CANCELPREV;
					GetMapData(GETADDRESS_CURRENT_EDITTING,iItem+n,bTmp,NULL);
					if(bTmp[0]==0xFD) goto CANCELPREV;
					SetMapData(GETADDRESS_CURRENT_EDITTING,iItem+n+1,bTmp);
					if((bTmp[1]&0x80)||((bTmp[0]&0x0f)==0x0D) && ((bTmp[1]&0x40)==0x00)) break;
				}
				bBuf[0]|=0xF0;
				SetMapData(GETADDRESS_CURRENT_EDITTING,iItem+n,bBuf);
			}
			bRet=TRUE;
		}
		
CANCELPREV:
		break;
	case IDM_EDITCOMMAND_ADD1TOTYPE:
		if((bBuf[1]&0x7F)!=0x7F) bBuf[1]++;
		SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
//	case VK_SUBTRACT:
//	case 'Q':
	case IDM_EDITCOMMAND_DEC1TOTYPE:
		if((bBuf[1]&0x7F)!=0x00) bBuf[1]--;
		SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
	case IDM_EDITCOMMAND_ADD16TOTYPE:
		if((bBuf[1]&0x7F)<=0x6F)
			bBuf[1]+=0x10;
		else
			bBuf[1]|=0x7F;
		SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
	case IDM_EDITCOMMAND_DEC16TOTYPE:
		if((bBuf[1]&0x7F)>=0x10)
			bBuf[1]-=0x10;
		else
			bBuf[1]&=0x80;
		SetMapData(GETADDRESS_CURRENT_EDITTING,iItem,bBuf);
		bRet=TRUE;
		break;
		/*
	case VK_NUMPAD0:
		if(bBuf[1]&0x80) bBuf[1]&=0x7F;
		else bBuf[1]|=0x80;
		SetMapData(iItem,bBuf);
		bRet=TRUE;
		break;
		*/
	case IDM_EDITCOMMAND_REWPAGE:
		{
			int iPage;
			BYTE bBuf[2];
			int iNewIndex;
			GetMapData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),NULL,&iPage);
			iNewIndex=GetMapDataIndex(GETADDRESS_CURRENT_EDITTING,NULL,iPage-1,TRUE);
			if(iNewIndex!=-1) SetSelectedItem(iNewIndex,TRUE);//giSelectedItem=iNewIndex;
			bRet=TRUE;

			//カーソルの更新
			GetMapData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
			SetMapViewCursoleMap(bBuf,iPage);

		}
		break;
	case IDM_EDITCOMMAND_FWDPAGE:
		{
			int iPage;
			BYTE bBuf[2];
			int iNewIndex;

			GetMapData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),NULL,&iPage);
			iNewIndex=GetMapDataIndex(GETADDRESS_CURRENT_EDITTING,NULL,iPage+1,FALSE);
			if(iNewIndex!=-1) SetSelectedItem(iNewIndex,TRUE);//giSelectedItem=iNewIndex;
			bRet=TRUE;

			//カーソルの更新
			GetMapData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
			SetMapViewCursoleMap(bBuf,iPage);
		}
		break;
	case IDM_EDITCOMMAND_NEXTOBJ:
	case IDM_EDITCOMMAND_PREVOBJ:
		{
			int iNewIndex;
			BOOL blPlus;

			blPlus = (iVKey == IDM_EDITCOMMAND_NEXTOBJ) ? TRUE : FALSE;

			if( (iNewIndex = SetObjIndexMap(iItem,blPlus))
				!= INVALID_OBJECT_INDEX)
			{
				int iPage;
				BYTE bBuf[2];
				// 
				bRet = TRUE;
				// カーソルの更新
				GetMapData(GETADDRESS_CURRENT_EDITTING,GetSelectedIndex(),bBuf,&iPage);
				SetMapViewCursoleMap(bBuf,iPage);
			}
		}
		break;
	}

	return bRet;
}

static BOOL IsUndoNeeded(int iID)
{
	if(iID==IDM_EDITCOMMAND_ADD1TOTYPE ||
		iID==IDM_EDITCOMMAND_DEC1TOTYPE ||
		iID==IDM_EDITCOMMAND_ADD16TOTYPE ||
		iID==IDM_EDITCOMMAND_DEC16TOTYPE ||
		iID==IDM_EDITCOMMAND_RIGHT ||
		iID==IDM_EDITCOMMAND_LEFT ||
		iID==IDM_EDITCOMMAND_UP ||
		iID==IDM_EDITCOMMAND_DOWN)
		return TRUE;
	return FALSE;
}

BOOL MapEditCommand(WORD wCommand)
{
	static int iPrevEditMode=-2;
	BOOL fReturn;

	if(GetMapEditMode()!=iPrevEditMode)
		g_blKeyCommandUndo=TRUE;
	//
	if(IsUndoNeeded(wCommand) && g_blKeyCommandUndo){
		undoPrepare(UNDONAME_KEYEDIT);
		g_blKeyCommandUndo=FALSE;
	}
	//
	iPrevEditMode=GetMapEditMode();
	//
	if(GetMapEditMode()){
		//敵
		if(fReturn=BadGuysKeyInput(GetSelectedIndex(),wCommand)){
			UpdateObjectList(1);
			UpdateObjectView(0);
		}
	}
	else{
		//マップ
		if(fReturn=MapKeyInput(GetSelectedIndex(),wCommand)){
			UpdateObjectList(1);
			UpdateObjectView(0);
		}
	}

	return fReturn;
}

