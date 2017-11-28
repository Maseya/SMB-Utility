/************************************************************************************

                                  smb Utility

  File: filemng.h
  Description: ファイルのロード。セーブを行うルーチン
  History:

 ************************************************************************************/
#include "smbutil.h"

// 外部エディタ変更対応
FILETIME gFileTime;
BOOL gblFileCmpEnable;
extern char gFilePath[MAX_PATH];

static BOOL GetFileLastWrite(LPFILETIME lpFileTime)
{
	HANDLE hFile;

	gblFileCmpEnable=TRUE;
	hFile=CreateFile(gFilePath,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile){
		gblFileCmpEnable=FALSE;
		return FALSE;
	}

	if(!GetFileTime(hFile,NULL,NULL,lpFileTime)){
		gblFileCmpEnable=FALSE;
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile); 
	return TRUE;
}

void SetROMFileTime()
{
	GetFileLastWrite(&gFileTime);
}

//（再）ロードした時点のファイルの最終更新時間と
//　　同じ…TRUE
//　違う…FALSE
//  を返す。
BOOL CheckROMFileTime()
{
	FILETIME TmpFileTime;

	if(!gblFileCmpEnable) return TRUE;//失敗
	
	if(!GetFileLastWrite(&TmpFileTime)) return TRUE;//失敗

	if(!memcmp(&TmpFileTime,&gFileTime,sizeof(FILETIME))) return TRUE;
	
	return FALSE;
}

BOOL LoadROM(LPSTR pFilename)
{
	FILE *fp;

	iTrainer=0;
	gblIsROMLoaded=FALSE;

	if((fp=fopen(pFilename,"rb"))==NULL){
		Msg(STRING_FILEERROR_NOTFOUND, MB_OK|MB_ICONWARNING);
		return FALSE;
	}

	fread(&Head,1,sizeof(INESHEADER),fp);
	if(memcmp(Head.cType,"NES\x1a",4) || (Head.bNum_CHARs!=SMB_NUM_CHARS || Head.bNum_PRGs!=SMB_NUM_PRGS) || ((Head.bROM_Type&0x01)!=0x01)){
		Msg(STRING_FILEERROR_FILEFORMAT, MB_OK|MB_ICONWARNING);
		fclose(fp);
		return FALSE;
	}

	if(Head.bROM_Type & 0x4){
		iTrainer=INES_TRAINERSIZE;
		fread(bPRGROM+0x7000,INES_TRAINERSIZE,1,fp);
	}

	fread(bPRGROM + 0x8000, SMB_PRG_SIZE, 1, fp);

	fread(bCHRROM, SMB_CHR_SIZE, 1, fp);

	fclose(fp);

	//
	gblIsROMLoaded=TRUE;
	SetROMFileTime();
	
	return TRUE;
}

BOOL SaveAsFile(LPSTR pFilename)
{
	FILE *fp;

	if((fp=fopen(pFilename,"w+b"))==NULL) return FALSE;

	fwrite(&Head,sizeof(INESHEADER),1,fp);
	if(iTrainer) fwrite(bPRGROM+0x7000,INES_TRAINERSIZE,1,fp);
	fwrite(bPRGROM+0x8000,INES_PRGROMBANKSIZE*SMB_NUM_PRGS,1,fp);
	fwrite(bCHRROM,INES_CHRROMBANKSIZE*SMB_NUM_CHARS,1,fp);

	fclose(fp);

	//
//	gblDataChanged=FALSE;
	fr_SetDataChanged(FALSE);
	SetROMFileTime();

	return TRUE;
}
