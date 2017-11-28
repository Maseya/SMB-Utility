/************************************************************************************

                                  smb Utility

  File: ini.c
  Description: ���W�X�g���AINI�t�@�C���A�N�Z�X�̂��߂̃��[�`��
  History:

 ************************************************************************************/
#include <windows.h>
#include <windowsx.h>
#include "ini.h"
#include "cmnlib.h"
/********************

  ���W�X�g���A�N�Z�X

*********************/

/*************************************
dwType 
 REG_BINARY �C�ӂ̌`���̃o�C�i���f�[�^
 REG_DWORD  32�r�b�g�l
 REG_SZ     ������
**************************************/
BOOL WriteToRegistry(LPSTR lpValueName,DWORD dwType,LPVOID lpData,DWORD dwSize)
{
	HKEY hRegKey;
	BOOL blRet=TRUE;

	RegCreateKeyEx(HKEY_CURRENT_USER,REGROOTPATH,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hRegKey,NULL);
	if(!hRegKey) return FALSE;
	if(ERROR_SUCCESS!=RegSetValueEx(hRegKey,lpValueName,0,dwType,lpData,dwSize))
		blRet=FALSE;
	RegCloseKey(hRegKey);
	return blRet;
}

/*************************************
dwType 
 REG_BINARY �C�ӂ̌`���̃o�C�i���f�[�^
 REG_DWORD  32�r�b�g�l
 REG_SZ     ������
**************************************/
BOOL ReadFromRegistry(LPSTR lpValueName,DWORD dwType,LPVOID lpData,DWORD dwSize)
{
	HKEY hRegKey;
	DWORD dwTypeRet;
	BOOL blRet=TRUE;
	LONG lResult; 

	RegCreateKeyEx(HKEY_CURRENT_USER,REGROOTPATH,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hRegKey,NULL);
	if(!hRegKey) return FALSE;
	lResult=RegQueryValueEx(hRegKey,lpValueName,NULL,&dwTypeRet,lpData,&dwSize);
	if(lResult!=ERROR_SUCCESS || dwType!=dwTypeRet) blRet=FALSE;
	RegCloseKey(hRegKey);
	return blRet;
}

BOOL DeleteRegistryEntries()
{
	if(ERROR_SUCCESS!=RegDeleteKey(HKEY_CURRENT_USER,REGROOTPATH))
		return FALSE;
	return TRUE;
}

/**********************



 **********************/

static BOOL CheckFileExistance(LPTSTR lpFile)
{
	return (0xFFFFFFFF != GetFileAttributes(lpFile)) ? TRUE : FALSE;
}

int GetAppPathName(LPTSTR lpBuffer,int iBufferSize,LPTSTR lpFileName)
{
	TCHAR FullPath[MAX_PATH];
	LPTSTR pt, p;
	int cb;
	
	// NOTE : �R�}���h�v�����v�g������s���ꂽ�ꍇ�A���͂��ꂽ�R�}���h�����񂪂��̂܂܎擾�����B
	//        �Ⴆ�΁AWinIPS�̒u����Ă���f�B���N�g��������R�}���h�v�����v�g��"winips"�ƋN������ƁA
	//        "winips"���擾�����̂ŁA�ȉ��̕��@�ł͂��߁B
	// p = GetCommandLine();
	// ���s�\�t�@�C���̃t���p�X�����擾����
	GetModuleFileName(GetModuleHandle(NULL), FullPath, MAX_PATH);
	p = FullPath;

	while(*p == '"')
		p = CharNext(p);
	pt = p; // "���������擪�ւ̃|�C���^
	// �}���`�o�C�g�������𓾂�
	for (;*p && *p != '"';p = CharNext(p)); // �I�[��"��T��
	for (;pt < p && *(CharPrev(pt, p)) != '\\';p = CharPrev(pt, p)); // �I�[����\��T��
	//
	cb = p - pt; // �o�C�g��
	if (iBufferSize <= cb + (int)sizeof(TCHAR)) // + NULL����
		return 0;
	//
	memcpy(lpBuffer, pt, cb);
	//
	*(LPTSTR)((LPBYTE)lpBuffer + cb) = '\0';

	// �t�@�C�����̎w�肪����΁A������R�s�[
	if (lpFileName) {
		if (iBufferSize <= cb + (int)sizeof(TCHAR) + (int)strlen(lpFileName))
			return 0;
		lstrcat(lpBuffer, lpFileName);
	}

	return cb;

/*
	TCHAR FullPath[MAX_PATH];
	LPTSTR lpCmdLine;
	int iCopySize;
	int iDirNameSize;//�J�����g�f�B���N�g���̖��O�̃T�C�Y
	int iFileNameSize;//

	if(!lpBuffer) return 0;
	
	lpCmdLine = GetCommandLine();

	while(*lpCmdLine == '"') lpCmdLine++;
	for(iDirNameSize = 0;;iDirNameSize++) {
		if((*(lpCmdLine + iDirNameSize) == '\0')
			||(*(lpCmdLine + iDirNameSize) == '"')
			||(*(lpCmdLine + iDirNameSize)==' ')) {
			memcpy(FullPath, lpCmdLine, iDirNameSize * sizeof(TCHAR));
			FullPath[iDirNameSize] = 0;
			if(CheckFileExistance(FullPath))
				break;
		}
	}

	for(;iDirNameSize >= 0;iDirNameSize--)if(*(lpCmdLine + iDirNameSize - 1) == '\\')break;

	if(lpFileName)
	{
		for(iFileNameSize = 0;;iFileNameSize++) {
			if(*(lpFileName + iFileNameSize) == '\0')
				break;
		}
	}
	else
		iFileNameSize = 0;

	memcpy(FullPath, lpCmdLine, iDirNameSize * sizeof(TCHAR));
	FullPath[iDirNameSize] = 0;
	if(lpFileName) memcpy(FullPath + iDirNameSize, lpFileName, iFileNameSize);

	iCopySize = (iBufferSize < iDirNameSize + iFileNameSize) ? iBufferSize : iDirNameSize + iFileNameSize;

	memcpy(lpBuffer, FullPath, iCopySize * sizeof(TCHAR));
	lpBuffer[iCopySize] = 0;

	return iCopySize;
*/
}

UINT GetIntegerFromINI(LPCTSTR lpAppName,LPCTSTR lpKeyName,INT nDefault)
{
	TCHAR szPath[MAX_PATH];
	GetAppPathName(szPath,MAX_PATH,INIFILE_FILENAME);
	return GetPrivateProfileInt(lpAppName,lpKeyName,nDefault,szPath);
}
