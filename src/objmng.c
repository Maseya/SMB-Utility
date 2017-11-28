/************************************************************************************

                                  smb Utility

  File: objmng.c
  Description:
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "roommng.h"
#include "objlib.h"
#include "objmng.h"
//���݃G�f�B�g���Ă���I�u�W�F�N�g�̎�ނ�ۑ����Ă���
//0�̓}�b�v�A 1�͓G�������B
int giEditMode = 0;
//���݃G�f�B�g���Ă���I�u�W�F�N�g�̃C���f�b�N�X��ۑ�
int giSelectedItem = 0;

void SetSelectedItem(int iItem,BOOL blInitKeyUndo)
{
	if(giSelectedItem==iItem || iItem<0) return;

	giSelectedItem=iItem;

	//
	if(blInitKeyUndo)
		g_blKeyCommandUndo=TRUE;
}

/********************************************

  �G�f�C�b�g�̑Ώہi�}�b�v<->�G�j��؂�ւ���

  iMode CHANGEEDITMODE_SWITCHING �c�@���݂̃��[�h�Ƃ͋t�̃��[�h�ɂ���i�G���}�b�v�A�}�b�v���G�j
        CHANGEEDITMODE_MAP�@�c�@�}�b�v
		CHANGEEDITMODE_BADGUYS�@�c�@�G
		CHANGEEDITMODE_BADGUYS �c�@���[�h�̕ύX����
**********************************************/
void ChangeMapEditMode(int iMode, BOOL blInitSelect)
{
	int iPage;
	BYTE bBuf[3]={0};
	GETINDEXINFO gii = {0};

	if(iMode == CHANGEEDITMODE_SWITCHING) 
		giEditMode = (giEditMode) ? EDITMODE_MAP : EDITMODE_BADGUYS;
	else if(iMode == CHANGEEDITMODE_MAP || CHANGEEDITMODE_BADGUYS == 1) 
		giEditMode = iMode;

	if(!gblIsROMLoaded) return;

	//�V�����C���f�b�N�X�̐ݒ�
	if(blInitSelect){
		//giSelectedItem=0;//������
		SetSelectedItem(0,TRUE);
	}
	else{
		if(giEditMode == CHANGEEDITMODE_BADGUYS){//�G�ւ̐؂�ւ��[ϯ�߂őI������Ă����߰�ނ�I��
			int iNewIndex;
			if(GETDATAINDEX_ERROR_NOTFOUND != GetMapData(GETADDRESS_CURRENT_EDITTING,giSelectedItem,NULL,&iPage)){
				iNewIndex = GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING, NULL, iPage, TRUE);
				if(iNewIndex == GETDATAINDEX_ERROR_NOTFOUND){
					gii.dwFlag = GETINDEX_FLAG_END;
					iNewIndex = GetBadGuysDataIndex(GETADDRESS_CURRENT_EDITTING, &gii, iPage, TRUE);
				}
				//giSelectedItem=iNewIndex;
				SetSelectedItem(iNewIndex,TRUE);
			}
		}
		else{
			int iNewIndex;
			if(GETDATAINDEX_ERROR_NOTFOUND != GetBadGuysData(GETADDRESS_CURRENT_EDITTING,giSelectedItem,NULL,&iPage)){
				iNewIndex = GetMapDataIndex(GETADDRESS_CURRENT_EDITTING,NULL,iPage,TRUE);
				if(iNewIndex == GETDATAINDEX_ERROR_NOTFOUND){
					gii.dwFlag = GETINDEX_FLAG_END;
					iNewIndex = GetMapDataIndex(GETADDRESS_CURRENT_EDITTING,&gii,iPage,TRUE);
				}
				//giSelectedItem=iNewIndex;
				SetSelectedItem(iNewIndex,TRUE);
			}
		}
	}
}

/***************************************
EDITMODE_MAP=Map EDITMODE_BADGUYS=Bad Guys
****************************************/
int GetMapEditMode()
{
	return giEditMode;
}

int GetSelectedIndex()
{
	return giSelectedItem;
}
