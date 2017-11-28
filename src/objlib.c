/************************************************************************************

                                  smb Utility

  File: objlib.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "roommng.h"
#include "objlist.h"
#include "objlib.h"
#include "objview.h"
#include "objmng.h"
#include "ini.h"

BOOL g_blIsLengthValid=TRUE;

BOOL ObjectSeekGetIsLengthValid(){return g_blIsLengthValid;}
void ObjectSeekSetIsLengthValid(BOOL blIsValid){g_blIsLengthValid=blIsValid;}

void LoadEditorOption()
{
	UINT uData;
	// HARD
	uData = GetIntegerFromINI(INIFILE_EDITOR,INIFILE_EDITOR_PROTECT,1);
	if( !uData || uData==1 )
		g_blIsLengthValid = uData;
}

/*
�ݒ�p�_�C�A���O�R�[���o�b�N�֐�
*/
LRESULT CALLBACK EditorOptionDlgProc( HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
   switch (message)
   {
       case WM_INITDIALOG:
		   {
			   LPPROPSHEETPAGE lpPropPage=(LPPROPSHEETPAGE)lParam;

			   if(lpPropPage->lParam)
				   CenterPropatySheet(hDlg);

			   CheckDlgButton(hDlg,IDC_ISADDRESS,BST_CHECKED);
			   if(!g_blIsLengthValid) CheckDlgButton(hDlg,IDC_ISADDRESS,BST_UNCHECKED);
		   }
		   break;
	   case WM_NOTIFY:
	   {
		   LPNMHDR pnmh = (LPNMHDR) lParam; 
		   switch (pnmh->code) { 
		   case PSN_APPLY:
			   {
				   g_blIsLengthValid=FALSE;
				   if(BST_CHECKED==IsDlgButtonChecked(hDlg,IDC_ISADDRESS)) g_blIsLengthValid=TRUE;
				   return TRUE;
			   }
           break;
		   }
	   }
	   break;
   }

   return FALSE;
}

/*********************************************

  ���ڃI�u�W�F�N�g�f�[�^����������Ƃ��̕⏕�֐�

**********************************************/
/*********
    �G
**********/


//�G�̃y�[�W�ɂ��Ă̏����i�⏕�֐��j
static void BadGuysSetPage(OBJECTSEEKINFO *psObjSeek)
{
	if(!psObjSeek) return;

	if(((psObjSeek->pbData[1])&0x80) && !psObjSeek->blIsPrevPageCom){
		psObjSeek->dwPage++;
	}
	else if(((psObjSeek->pbData[0])&0x0F)==0x0F){
		// nhxx xxxx
		// n:���y�[�W�t���O
		// h:5-3�ȍ~�o��
		// xxxxxx:�V�����y�[�W
		psObjSeek->dwPage = psObjSeek->pbData[1] & 0x3F;
		psObjSeek->blIsPrevPageCom=TRUE;
	}
	
	if(((psObjSeek->pbData[0])&0x0F)!=0x0F) psObjSeek->blIsPrevPageCom=FALSE;
}

//���[�����̃f�[�^�Ȃ�TRUE�A���[���O�̃f�[�^�Ȃ�FALSE��Ԃ�
static BOOL BadGuysCheckDataLength(OBJECTSEEKINFO *psObjSeek)
{
	if(!psObjSeek) return FALSE;

	if((psObjSeek->dwLength)<(psObjSeek->dwOfs)+BadGuysGetDataLength(psObjSeek->pbData))
		return FALSE;

	return TRUE;
}

//�G�̃f�[�^�̃T�C�Y�𓾂�
int BadGuysGetDataLength(BYTE *pbBuf){
	return ((pbBuf[0]&0x0F)==0x0E)?3:2;
}

//�G�̃f�[�^���������邽�߂̃f�[�^������������
//uRoomID�ɂ́A�擾���������[���̃f�[�^���w��B
//GETADDRESS_CURRENT_EDITTING���w�肷��ƌ��݃G�f�B�g���Ă��郋�[��
BOOL BadGuysSeekFirst(OBJECTSEEKINFO *psObjSeek,UINT uRoomID){
	if(!psObjSeek) return FALSE;
	//�\���̂�������
	memset(psObjSeek,0,sizeof(OBJECTSEEKINFO));
	psObjSeek->pbData=bPRGROM+GetBadGuysAddress(uRoomID);
	
	if(psObjSeek->pbData[0]==0xFF) return FALSE;
	
	//�y�[�W�̏����Z�b�g
	BadGuysSetPage(psObjSeek);
	//�����̏����Z�b�g
	psObjSeek->dwLength=BadGuysGetAllDataLength(uRoomID);
	//�I�u�W�F�N�g�̃T�C�Y
	psObjSeek->dwObjLen=BadGuysGetDataLength(psObjSeek->pbData);

	if(g_blIsLengthValid && !BadGuysCheckDataLength(psObjSeek)) return FALSE;

	return TRUE;
}

//���̃f�[�^�𓾂�
//�������f�[�^���擾�ł����ꍇ�́ATRUE,�@NULL��n�����A�f�[�^�̏I���̏ꍇ�́AFALSE��Ԃ��B
BOOL BadGuysSeekNext(OBJECTSEEKINFO *psObjSeek){
	int iCurObjSize;
	if(!psObjSeek) return FALSE;
	//
	iCurObjSize=BadGuysGetDataLength(psObjSeek->pbData);
	psObjSeek->pbData+=iCurObjSize;
	psObjSeek->dwOfs+=iCurObjSize;

	if(psObjSeek->pbData[0]==0xFF) return FALSE;

	//�y�[�W�̏����Z�b�g
	BadGuysSetPage(psObjSeek);
	//�C���f�b�N�X���C���N�������g
	psObjSeek->dwIndex++;
	//�I�u�W�F�N�g�̃T�C�Y
	psObjSeek->dwObjLen=BadGuysGetDataLength(psObjSeek->pbData);

	if(g_blIsLengthValid
		&& (!BadGuysCheckDataLength(psObjSeek)
			|| psObjSeek->dwOfs > 0xFF)) // 1���[���ɑ��݂ł���ő�I�u�W�F�N�g��
			return FALSE;

	return TRUE;
}

DWORD BadGuysIsPageRelatedObject(LPBYTE lpbBuf)
{
	if( lpbBuf[1]&0x80 )
		return PAGEOBJECT_NEXTPAGEFLAG;
	if ( (lpbBuf[0]&0x0F)==0x0F )
		return PAGEOBJECT_SETPAGE;
	return PAGEOBJECT_NO;
}


/**********
   �}�b�v
***********/
//�y�[�W�̏����i�⏕�֐��j
static void MapSetPage(OBJECTSEEKINFO *psObjSeek)
{
	if(!psObjSeek) return;

	if(((psObjSeek->pbData[1])&0x80) /*&& !psObjSeek->blIsPrevPageCom*/){
		psObjSeek->dwPage++;
	}
	else if((((psObjSeek->pbData[0])&0x0F)==0x0D) && (!((psObjSeek->pbData[1])&0x40))){
		psObjSeek->dwPage=((psObjSeek->pbData[1])&0x3F);
		//psObjSeek->blIsPrevPageCom=TRUE;
	}

	//if(!((((psObjSeek->pbData[0])&0x0F)==0x0D) && (!((psObjSeek->pbData[1])&0x40)))) psObjSeek->blIsPrevPageCom=FALSE;
}

//���[�����̃f�[�^�Ȃ�TRUE�A���[���O�̃f�[�^�Ȃ�FALSE��Ԃ�
static BOOL MapCheckDataLength(OBJECTSEEKINFO *psObjSeek)
{
	if(!psObjSeek) return FALSE;

	if((psObjSeek->dwLength)<(psObjSeek->dwOfs)+2)
		return FALSE;

	return TRUE;
}

//�������邽�߂̃f�[�^������������
//uRoomID�ɂ́A�擾���������[���̃f�[�^���w��B
//GETADDRESS_CURRENT_EDITTING���w�肷��ƌ��݃G�f�B�g���Ă��郋�[��
BOOL MapSeekFirst(OBJECTSEEKINFO *psObjSeek,UINT uRoomID){
	if(!psObjSeek) return FALSE;
	//�\���̂�������
	memset(psObjSeek,0,sizeof(OBJECTSEEKINFO));
	psObjSeek->pbData=bPRGROM+GetMapAddress(uRoomID)+2;//+2�́A�w�b�_��

	if(psObjSeek->pbData[0]==0xFD) return FALSE;
	
	//�y�[�W�̏����Z�b�g
	MapSetPage(psObjSeek);
	//�����̏����Z�b�g
	psObjSeek->dwLength=MapGetAllDataLength(uRoomID);
	//�I�u�W�F�N�g�̃T�C�Y
	psObjSeek->dwObjLen=2;

	if(g_blIsLengthValid && !MapCheckDataLength(psObjSeek)) return FALSE;

	return TRUE;
}

BOOL MapSeekNext(OBJECTSEEKINFO *psObjSeek){
	if(!psObjSeek) return FALSE;
	//
	psObjSeek->pbData+=2;
	psObjSeek->dwOfs+=2;

	if(psObjSeek->pbData[0]==0xFD) return FALSE;

	//�y�[�W�̏����Z�b�g
	MapSetPage(psObjSeek);
	//�C���f�b�N�X���C���N�������g
	psObjSeek->dwIndex++;
	//�I�u�W�F�N�g�̃T�C�Y
	psObjSeek->dwObjLen=2;

	if(g_blIsLengthValid 
		&& (!MapCheckDataLength(psObjSeek)
		     || psObjSeek->dwOfs > 0xFF)) // 1���[���ɑ��݂ł���ő�I�u�W�F�N�g��
			 return FALSE;

	return TRUE;
}

/***********************************

  �}�b�v�̃f�[�^�ύX�����֐�

************************************/
int GetMapData(UINT uRoomID, int iIndex, BYTE *pbBuf, int *piPage)
{
	OBJECTSEEKINFO ObjSeek;

	if(iIndex<0) return -1;

	if(!MapSeekFirst(&ObjSeek,uRoomID)) return -1;
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iIndex) break;
		if(!MapSeekNext(&ObjSeek)) return -1;
	}

	if(pbBuf && sizeof(pbBuf)>=2) memcpy(pbBuf,ObjSeek.pbData,2);
	
	if(piPage) *piPage=ObjSeek.dwPage;

	return ObjSeek.dwOfs;
}


void GetMapHeadData(UINT uRoomID, BYTE *pbBuf)
{
	if(!pbBuf || sizeof(pbBuf)<2) return;

	memcpy(pbBuf,bPRGROM+GetMapAddress(uRoomID),2);
}

BOOL SortByPosXMap(UINT uRoomID,int *piCurIndex,BOOL IsResort)
{
	BYTE *pbData;
	BOOL blRet;
	UINT uPageBaseOfs;
	int iIndexBase;//�擪����̊�C���f�b�N�X
	int iIndex;//�����̃C���f�b�N�X
	BOOL blReSortNeed;
	DWORD dwLength;

	blReSortNeed=FALSE;
	blRet=FALSE;
	iIndexBase=0;
	iIndex=0;
	uPageBaseOfs=0;
	dwLength=MapGetAllDataLength(uRoomID);
	pbData=bPRGROM+GetMapAddress(uRoomID);
	pbData+=2;
	for(;;)
	{
		BYTE *pbPageBase;
		int iPageBaseNum=0;

		pbPageBase=pbData;
		//�P�y�[�W���̌���
		for(;;)
		{
			int n=0;
			for(;;)
			{	
				if((pbPageBase[iPageBaseNum]&0xF0)>(pbPageBase[iPageBaseNum+n]&0xF0))
				{
					BYTE bTmp[2];

					//
					if((pbPageBase[iPageBaseNum]&0x0F)==0x0D && (pbPageBase[iPageBaseNum+1]&0x70)==0x00)
						blReSortNeed=TRUE;

					//���y�[�W�t���O
					if(pbPageBase[iPageBaseNum+1]&0x80)
					{
						pbPageBase[iPageBaseNum+n+1]|=0x80;
						pbPageBase[iPageBaseNum+1]&=0x7F;
					}
					memcpy(bTmp,pbPageBase+iPageBaseNum,2);
					memcpy(pbPageBase+iPageBaseNum,pbPageBase+iPageBaseNum+n,2);
					memcpy(pbPageBase+iPageBaseNum+n,bTmp,2);
					//�ް����ς����
					blRet=TRUE;
					//�C���f�b�N�X�̏���
					if(piCurIndex)
					{
						if((iIndexBase+iIndex)==*piCurIndex)
						{
							*piCurIndex=iIndexBase;
						}
						else if(iIndexBase==*piCurIndex)
						{
							*piCurIndex=iIndexBase+iIndex;
						}
					}
				}
				
				n+=2;
				iIndex++;
				if((pbPageBase[iPageBaseNum+n+1]&0x80)||
					( (pbPageBase[iPageBaseNum+n]&0x0F)==0x0D && (pbPageBase[iPageBaseNum+n+1]&0x70)==0x00)||
					(pbPageBase[iPageBaseNum+n]==0xFD)||
					(g_blIsLengthValid && ((DWORD)(uPageBaseOfs+n+2)>=dwLength)))
					break;
			}

			iPageBaseNum+=2;
			uPageBaseOfs+=2;
			iIndexBase++;
			iIndex=0;

			if((pbPageBase[iPageBaseNum+1]&0x80)||
				((pbPageBase[iPageBaseNum]&0x0F)==0x0D && (pbPageBase[iPageBaseNum+1]&0x70)==0x00)||
				(pbPageBase[iPageBaseNum]==0xFD)||
				(g_blIsLengthValid && ((DWORD)uPageBaseOfs+2>=dwLength))) break;
		}

		pbData+=iPageBaseNum;
		
		if(pbData[0]==0xFD)break;
	}

	if(!IsResort && blReSortNeed){
		blRet=SortByPosXMap(uRoomID,piCurIndex,TRUE);
	}

	return blRet;
}

BOOL SetMapData(UINT uRoomID,int iIndex,BYTE *bBuf)
{
	OBJECTSEEKINFO ObjSeek;

	if(sizeof(bBuf)<2 || iIndex<0) return FALSE;

	if(bBuf[0]==0xFD) return FALSE;
	
	if(!MapSeekFirst(&ObjSeek,uRoomID)) return FALSE;
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iIndex) break;
		if(!MapSeekNext(&ObjSeek)) return FALSE;
	}
	
	memcpy(ObjSeek.pbData ,bBuf,2);

//	gblDataChanged=TRUE;
	fr_SetDataChanged(TRUE);

	{
		int iPage;
		BYTE bTmp[2];

		SortByPosXMap(uRoomID,&iIndex,FALSE);
		//giSelectedItem=iIndex;
		SetSelectedItem(iIndex,FALSE);
		GetMapData(uRoomID,iIndex,bTmp,&iPage);
		//�J�[�\���̍X�V
		SetMapViewCursoleMap(bTmp,iPage);
	}

	return TRUE;
}

BOOL SetMapDataBinary(UINT uRoomID,int iIndex, BYTE *bBuf,int iValidSize)
{
	OBJECTSEEKINFO ObjSeek;

	if(sizeof(bBuf)<2 || iIndex<0) return FALSE;

	if(!MapSeekFirst(&ObjSeek,uRoomID)) return FALSE;

	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iIndex) break;
		if(!MapSeekNext(&ObjSeek)) return FALSE;
	}

	if(g_blIsLengthValid && (ObjSeek.dwLength<ObjSeek.dwOfs+iValidSize)) return FALSE;	

	memcpy(ObjSeek.pbData,bBuf,iValidSize);

//	gblDataChanged=TRUE;
	fr_SetDataChanged(TRUE);

	{
		int iPage;
		BYTE bTmp[2];

		SortByPosXMap(GETADDRESS_CURRENT_EDITTING,&iIndex,FALSE);
		//giSelectedItem=iIndex;
		SetSelectedItem(iIndex,FALSE);
		GetMapData(GETADDRESS_CURRENT_EDITTING,iIndex,bTmp,&iPage);

		//�J�[�\���̍X�V
		SetMapViewCursoleMap(bTmp,iPage);
	}

	return TRUE;
}

/******

  blPageOverDec�́A�w�肵���߰�ނ𒴂��Ă��܂����ꍇ�ɁA�C���f�b�N�X���P���炷��
  TRUE�c���炷 FALSE �c�@���炳�Ȃ�
*******/
int GetMapDataIndex(UINT uRoomID,GETINDEXINFO *psGetIndex,int iPage,BOOL blPageOverDec)
{
	int iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
	OBJECTSEEKINFO ObjSeek;
	BOOL fFound = FALSE;
	
	if(iPage < 0) iPage = 0;

	if(psGetIndex){
		psGetIndex->nNumSamePos = 0;
		psGetIndex->iIndex = 0;
	}

	if(MapSeekFirst(&ObjSeek,uRoomID)){
		for(;;){
			if(!fFound) iCurIndex++;
			if(ObjSeek.dwPage>=(DWORD)iPage) {
				if(!psGetIndex){
					if(ObjSeek.dwPage>(DWORD)iPage && iCurIndex>0 && blPageOverDec) iCurIndex--;
					break;
				}
				else if(ObjSeek.dwPage==(DWORD)iPage)
				{
					//if((psGetIndex->dwFlag&GETINDEX_FLAG_BIN) && !memcmp(pbBuf,ObjSeek.pbData,sizeof(pbBuf)))break;
					if((psGetIndex->dwFlag & GETINDEX_FLAG_XY)
						&& psGetIndex->x == GetMapXPos(ObjSeek.pbData)
						&& psGetIndex->y == GetMapYPos(ObjSeek.pbData)){
						fFound = TRUE;
						if(psGetIndex && (++psGetIndex->nNumSamePos > 1)){
							if(iCurIndex != GETDATAINDEX_ERROR_PL)
								psGetIndex->iIndex = iCurIndex;
							iCurIndex = GETDATAINDEX_ERROR_PL;
						}
					}
				}
			}

			if(!MapSeekNext(&ObjSeek)){
				if(!fFound){
					if(psGetIndex && (psGetIndex->dwFlag & GETINDEX_FLAG_END))
						iCurIndex = iCurIndex;
					else
						iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
				}
				break;
			}
		}
	}
	return iCurIndex;
}

/***********************************

  �G�̃f�[�^�ύX�����֐�

************************************/
int GetBadGuysData(UINT uRoomID,int iIndex, BYTE *bBuf, int *piPage)
{
	OBJECTSEEKINFO ObjSeek;

	if(iIndex<0) return -1;
	
	if(!BadGuysSeekFirst(&ObjSeek,uRoomID)) return -1;
	
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iIndex) break;
		if(!BadGuysSeekNext(&ObjSeek)) return -1;
	}

	if(sizeof(bBuf)>=3 && bBuf)	
		memcpy(bBuf,ObjSeek.pbData,BadGuysGetDataLength(ObjSeek.pbData));
	if(piPage) *piPage=ObjSeek.dwPage;
	
	return ObjSeek.dwOfs;
}

static BOOL MemorySwap(BYTE *pbBuf1,int iBuf1Size, BYTE *pbBuf2, int iBuf2Size)
{
	BYTE *pbBufSmall;//�A�h���X�̏�������
	BYTE *pbTmpSmall;
	BYTE *pbBufLarge;//�A�h���X�̑傫����
	BYTE *pbTmpLarge;
	int iSizeSmall;//�A�h���X�̏��������̃f�[�^�̃T�C�Y
	int iSizeLarge;//�A�h���X�̑傫�����̃f�[�^�̃T�C�Y

	if(pbBuf1==pbBuf2) return FALSE;

	if(pbBuf1>pbBuf2)
	{
		pbBufSmall=pbBuf2;
		iSizeSmall=iBuf2Size;
		pbBufLarge=pbBuf1;
		iSizeLarge=iBuf1Size;
	}
	else
	{
		pbBufSmall=pbBuf1;
		iSizeSmall=iBuf1Size;
		pbBufLarge=pbBuf2;
		iSizeLarge=iBuf2Size;
	}

	pbTmpSmall=Malloc(iSizeSmall);
	if(!pbTmpSmall) return FALSE;
	memcpy(pbTmpSmall,pbBufSmall,iSizeSmall);

	pbTmpLarge=Malloc(iSizeLarge);
	if(!pbTmpSmall) 
	{
		Mfree(pbTmpSmall);
		return FALSE;
	}
	memcpy(pbTmpLarge,pbBufLarge,iSizeLarge);

	memmove(pbBufSmall+iSizeLarge,pbBufSmall+iSizeSmall,pbBufLarge-(pbBufSmall+iSizeSmall));

	memcpy(pbBufSmall,pbTmpLarge,iSizeLarge);
	memcpy(pbBufLarge+iSizeLarge-iSizeSmall,pbTmpSmall,iSizeSmall);

	Mfree(pbTmpSmall);
	Mfree(pbTmpLarge);

	return TRUE;
}

/***************************************************
���בւ����N��������TRUE�A�����łȂ����FALSE��Ԃ��B
****************************************************/
/*
BOOL SortByPosXBadGuysEx(UINT uRoomID,int *piCurIndex,BOOL blIsResort)
{
	UINT uPage;
	BOOL blRet=FALSE;
	OBJECTSEEKINFO ObjSeek;
	OBJECTSEEKINFO ObjSeekBase;

	if(!BadGuysSeekFirst(&ObjSeekBase,uRoomID)) return FALSE;
	for(;;){
		uPage=ObjSeekBase.dwPage;
		for(;;){
			memcpy(&ObjSeek,&ObjSeekBase,sizeof(OBJECTSEEKINFO));
			for(;;){
				if(!BadGuysSeekNext(&ObjSeek) || uPage!=ObjSeek.dwPage) break;
				if((ObjSeekBase.pbData[0]&0xF0)>(ObjSeek.pbData[0]&0xF0)){

				}
			}
			if(!BadGuysSeekNext(&ObjSeekBase) || uPage!=ObjSeekBase.dwPage) break;
		}
	}
	return ;
}
*/
BOOL SortByPosXBadGuys(UINT uRoomID,int *piCurIndex,BOOL blIsResort)
{
	BYTE *pbData;
	BOOL blRet;
	UINT uPageBaseOfs;
	int iIndexBase;//�擪����̊�C���f�b�N�X
	int iIndex;//�����̃C���f�b�N�X
	BOOL blReSortNeed;
	DWORD dwLength;

	blReSortNeed=FALSE;
	blRet=FALSE;
	pbData=bPRGROM+GetBadGuysAddress(uRoomID);
	dwLength=BadGuysGetAllDataLength(uRoomID);
	iIndexBase=0;
	iIndex=0;
	uPageBaseOfs=0;
	for(;;)
	{
		BYTE *pbPageBase;
		int iPageBaseNum=0;

		pbPageBase=pbData;
		//�P�y�[�W���̃\�[�g
		for(;;)
		{
			int n=0;
			for(;;)
			{
				if((pbPageBase[iPageBaseNum+n]&0x0F)!=0x0E)
					n+=2;
				else
					n+=3;

				iIndex++;
				
				if((pbPageBase[iPageBaseNum+n+1]&0x80)||
					(pbPageBase[iPageBaseNum+n]==0xFF)||
					((pbPageBase[iPageBaseNum+n]&0x0F)==0x0F)||
					(g_blIsLengthValid && ((DWORD)(uPageBaseOfs+n+BadGuysGetDataLength(&pbPageBase[iPageBaseNum]))>dwLength))) break;

				//�X���b�v���K�v�Ȃ�s��
				if((pbPageBase[iPageBaseNum]&0xF0)>(pbPageBase[iPageBaseNum+n]&0xF0))
				{
					BYTE bTmp[3];
					int iData1Size;
					int iData2Size;

					//�y�[�W�̃x�[�X������R�}���h�ŁA����Ɖ������X���b�v�����ꍇ�A�ă\�[�g���K�v
					if((pbPageBase[iPageBaseNum]&0x0F)==0x0F) blReSortNeed=TRUE;

					iData1Size=((pbPageBase[iPageBaseNum]&0x0F)==0x0E)?3:2;
					iData2Size=((pbPageBase[iPageBaseNum+n]&0x0F)==0x0E)?3:2;
					if(pbPageBase[iPageBaseNum+1]&0x80)
					{
						pbPageBase[iPageBaseNum+1]&=0x7F;
						pbPageBase[iPageBaseNum+n+1]|=0x80;
					}

					if(iData1Size==iData2Size)
					{
						memcpy(bTmp,pbPageBase+iPageBaseNum,iData1Size);
						memcpy(pbPageBase+iPageBaseNum,pbPageBase+iPageBaseNum+n,iData1Size);
						memcpy(pbPageBase+iPageBaseNum+n,bTmp,iData1Size);
					}
					else
					{
						MemorySwap(pbPageBase+iPageBaseNum,iData1Size,pbPageBase+iPageBaseNum+n,iData2Size);
						n+=(iData2Size-iData1Size);
					}
					//�ް����ς����
					blRet=TRUE;
					//�C���f�b�N�X�̏���
					if(piCurIndex)
					{
						if((iIndexBase+iIndex)==*piCurIndex)
						{
								*piCurIndex=iIndexBase;
						}
						else if(iIndexBase==*piCurIndex)
						{
								*piCurIndex=iIndexBase+iIndex;
						}
					}
				}/* if */

			}/* for */

			if((pbPageBase[iPageBaseNum]&0x0F)!=0x0E){
				iPageBaseNum+=2;
				uPageBaseOfs+=2;
			}
			else{
				iPageBaseNum+=3;
				uPageBaseOfs+=3;
			}
			iIndexBase++;
			iIndex=0;

			if((pbPageBase[iPageBaseNum+1]&0x80)||
				(pbPageBase[iPageBaseNum]==0xFF)||
				((pbPageBase[iPageBaseNum]&0x0F)==0x0F)||
				(g_blIsLengthValid && ((DWORD)(uPageBaseOfs+BadGuysGetDataLength(&pbPageBase[iPageBaseNum]))>dwLength))) break;
		}/* for */

		pbData+=iPageBaseNum;

		if(pbData[0]==0xFF)	break;

	} /* for */

	if(!blIsResort && blReSortNeed){
		blRet=SortByPosXBadGuys(uRoomID,piCurIndex,TRUE);
	}

	return blRet;
}

/*
	���ӁF�@�f�[�^�̃T�C�Y�̈Ⴄ�I�u�W�F�N�g�i���[���Ԉړ��̖��߂ɃN���{�[�Ȃǁj��ݒ肵�悤�Ƃ���Ǝ��s����

*/
BOOL SetBadGuysData(UINT uRoomID,int iIndex, BYTE *bBuf)
{
	int iValidSize1,iValidSize2;
	OBJECTSEEKINFO ObjSeek;

	if(sizeof(bBuf)<3 || !bBuf) return FALSE;

	if(bBuf[0]==0xFF) return FALSE;

	if(!BadGuysSeekFirst(&ObjSeek,uRoomID)) return FALSE;
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iIndex) break;
		if(!BadGuysSeekNext(&ObjSeek)) return FALSE;
	}
	
	//���̓f�[�^�̃T�C�Y���擾
	iValidSize1=BadGuysGetDataLength(bBuf);
	//���̃f�[�^�̃T�C�Y���擾
	iValidSize2=BadGuysGetDataLength(ObjSeek.pbData);
	if(iValidSize1!=iValidSize2) return FALSE;
	memcpy(ObjSeek.pbData,bBuf,iValidSize1);

//	gblDataChanged=TRUE;
	fr_SetDataChanged(TRUE);

	//
	{
		int iPage;
		BYTE bTmp[3];

		SortByPosXBadGuys(GETADDRESS_CURRENT_EDITTING,&iIndex,FALSE);
		//giSelectedItem=iIndex;
		SetSelectedItem(iIndex,FALSE);
		GetBadGuysData(GETADDRESS_CURRENT_EDITTING,iIndex,bTmp,&iPage);
		//�}�b�v�r���[�̃J�[�\���̍X�V
		SetMapViewCursoleBadGuys(bTmp,iPage);
	}

	return TRUE;
}

BOOL SetBadGuysDataBinary(UINT uRoomID,int iIndex, BYTE *bBuf,int iValidSize)
{
	OBJECTSEEKINFO ObjSeek;

	if(sizeof(bBuf)<3 || !bBuf) return FALSE;

	if(!BadGuysSeekFirst(&ObjSeek,uRoomID)) return FALSE;
	
	for(;;){
		if(ObjSeek.dwIndex==(DWORD)iIndex) break;
		if(!BadGuysSeekNext(&ObjSeek)) return FALSE;
	}
	
	if(g_blIsLengthValid && (ObjSeek.dwLength<ObjSeek.dwOfs+iValidSize)) return FALSE;

	memcpy(ObjSeek.pbData,bBuf,iValidSize);

//	gblDataChanged=TRUE;
	fr_SetDataChanged(TRUE);

	//�}�b�v�r���[�A�J�����g�Z���N�g�̍X�V
	{
		int iPage;
		BYTE bTmp[3];

		SortByPosXBadGuys(GETADDRESS_CURRENT_EDITTING,&iIndex,FALSE);
		//giSelectedItem=iIndex;
		SetSelectedItem(iIndex,FALSE);
		GetBadGuysData(GETADDRESS_CURRENT_EDITTING,iIndex,bTmp,&iPage);

		SetMapViewCursoleBadGuys(bTmp,iPage);

	}

	return TRUE;
}

int GetBadGuysDataIndex(UINT uRoomID,GETINDEXINFO *psGetIndex,int iPage,BOOL blPageOverDec)
{
	int iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
	OBJECTSEEKINFO ObjSeek;
	int x,y;
	BOOL fFound = FALSE;

	if(iPage < 0) iPage = 0;

	if(psGetIndex){
		psGetIndex->nNumSamePos = 0;
		psGetIndex->iIndex = 0;
	}

	if(BadGuysSeekFirst(&ObjSeek,uRoomID)){
		for(;;){
			if(!fFound) iCurIndex++;
			if(!psGetIndex){
				if(ObjSeek.dwPage>=(DWORD)iPage) {
					if(ObjSeek.dwPage>(DWORD)iPage && iCurIndex>0 && blPageOverDec) iCurIndex--;
						break;
				}
			}
			else {
					//if((psGetIndex->dwFlag&GETINDEX_FLAG_BIN) && !memcmp(pbBuf,ObjSeek.pbData,sizeof(pbBuf)))break;
					if(psGetIndex->dwFlag&GETINDEX_FLAG_XY){
						int iTmpPage;
						iTmpPage=ObjSeek.dwPage;
						x=GetBadGuysXPos(ObjSeek.pbData);
						y=GetBadGuysYPos(ObjSeek.pbData);
						if(x<0){
							iTmpPage--;
							x+=16;
						}
						else if(x>15){
							iTmpPage++;
							x-=16;
						}
						if(x==psGetIndex->x
							&& y==psGetIndex->y
							&& iTmpPage==iPage){
							fFound = TRUE;
							if(psGetIndex && (++psGetIndex->nNumSamePos > 1)){
								if(iCurIndex != GETDATAINDEX_ERROR_PL)
									psGetIndex->iIndex = iCurIndex;
								iCurIndex = GETDATAINDEX_ERROR_PL;
							}
						}
					}
			}

			if(!BadGuysSeekNext(&ObjSeek)){
				if(!fFound){
					if(psGetIndex && (psGetIndex->dwFlag & GETINDEX_FLAG_END))
						iCurIndex = iCurIndex;
					else
						iCurIndex = GETDATAINDEX_ERROR_NOTFOUND;
				}
				break;
			}
		}
	}
	return iCurIndex;
}

/***************************



****************************/
extern SMBBADGUYSINFO smbBudGuysInfo[];
int GetBadGuysYPos(BYTE *pbBuf)
{
	int iRet;
	if((pbBuf[0]&0x0F)!=0x0E){
		if((pbBuf[0]&0x0F)!=0x0F){
			if(smbBudGuysInfo[pbBuf[1]&0x3F].bFixedYPos)
				iRet=smbBudGuysInfo[pbBuf[1]&0x3F].bFixedYPos;
			else
				iRet=(pbBuf[0]&0x0F)+smbBudGuysInfo[pbBuf[1]&0x3F].YDelta;
		}
		else
			iRet=0x0F;
	}else{
		iRet=0x0E;
	}
	return iRet;
}

int GetBadGuysXPos(BYTE *pbBuf)
{
	if((pbBuf[0]&0x0F)!=0x0E)
		return ((pbBuf[0]>>4)&0x0F)+smbBudGuysInfo[pbBuf[1]&0x3F].XDelta;
	else
		return ((pbBuf[0]>>4)&0x0F);
}

//
int GetMapXPos(BYTE *pbBuf)
{
	return ((pbBuf[0]>>4)&0x0F);
}
int GetMapYPos(BYTE *pbBuf)
{
	return (pbBuf[0]&0x0F);
}

BOOL BadGuysIsHardObject(BYTE *pbBuf)
{
	BYTE bBuf = pbBuf[0] &0x0F;
	return ( (bBuf != 0x0E)
		    && (bBuf != 0x0F)
			&& (pbBuf[1] & 0x40)) ? TRUE : FALSE;
}