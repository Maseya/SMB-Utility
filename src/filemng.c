/************************************************************************************

                                  smb Utility

  File: filemng.h
  Description: ファイルのロード。セーブを行うルーチン
  Description: Load file. Save routine
  History:

 ************************************************************************************/
#include "smbutil.h"

 // 外部エディタ変更対応
 // Supports external editor change
FILETIME gFileTime;
BOOL gblFileCmpEnable;
extern TCHAR gFilePath[MAX_PATH];

static BOOL GetFileLastWrite(LPFILETIME lpFileTime)
{
    HANDLE hFile;

    gblFileCmpEnable = TRUE;
    hFile = CreateFile(gFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile)
    {
        gblFileCmpEnable = FALSE;
        return FALSE;
    }

    if (!GetFileTime(hFile, NULL, NULL, lpFileTime))
    {
        gblFileCmpEnable = FALSE;
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
// Last update time of the file at the time of loading(re)
// same ... TRUE
// different ... FALSE
//  return it.
BOOL CheckROMFileTime()
{
    FILETIME TmpFileTime;

    // 失敗
    // Failure
    if (!gblFileCmpEnable) return TRUE;

    //失敗
    // Failure
    if (!GetFileLastWrite(&TmpFileTime)) return TRUE;

    if (!memcmp(&TmpFileTime, &gFileTime, sizeof(FILETIME))) return TRUE;

    return FALSE;
}

void LoadPrgRom(int prgNumber)
{
    int offset = sizeof(INESHEADER);
    memcpy(
        bPRGROM + 0x8000,
        bpROM + offset,
        0x4000);

    offset += (prgNumber * 0x8000);
    memcpy(
        bPRGROM + 0xC000,
        bpROM + 0x4000 + offset,
        0x4000);

    memcpy(
        bCHRROM,
        bpROM + iROMSize - SMB_CHR_SIZE,
        SMB_CHR_SIZE);
}

BOOL LoadROM(LPTSTR pFilename)
{
    FILE *fp;

    iTrainer = 0;
    gblIsROMLoaded = FALSE;

    if ((fp = _tfopen(pFilename, __T("rb"))) == NULL)
    {
        Msg(STRING_FILEERROR_NOTFOUND, MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    fseek(fp, 0, SEEK_END);
    iROMSize = ftell(fp);

    // Remove old ROM allocation in case we get a new size.
    if (bpROM)
    {
        free(bpROM);
    }

    bpROM = (BYTE*)malloc(iROMSize * sizeof(BYTE));

    rewind(fp);
    fread(bpROM, iROMSize, 1, fp);

    fclose(fp);

    memcpy(&Head, bpROM, sizeof(INESHEADER));
    if (memcmp(Head.cType, "NES\x1a", 4) ||
        Head.bNum_CHARs != SMB_NUM_CHARS ||
        (Head.bNum_PRGs != SMB_NUM_PRGS && Head.bNum_PRGs != SMB_NUM_PRGS * 2) ||
        (Head.bROM_Type & 0x01) != 0x01)
    {
        Msg(STRING_FILEERROR_FILEFORMAT, MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    // What is this?
    if (Head.bROM_Type & 0x4)
    {
        iTrainer = INES_TRAINERSIZE;
        fread(bPRGROM + 0x7000, INES_TRAINERSIZE, 1, fp);
    }

    int prgNum = (iROMSize - 0xA010) / 0x8000;
    LoadPrgRom(prgNum);

    gblIsROMLoaded = TRUE;
    SetROMFileTime();

    return TRUE;
}

BOOL SaveAsFile(LPTSTR pFilename)
{
    FILE *fp;

    if ((fp = _tfopen(pFilename, __T("w+b"))) == NULL) return FALSE;

    fwrite(&Head, sizeof(INESHEADER), 1, fp);
    if (iTrainer) fwrite(bPRGROM + 0x7000, INES_TRAINERSIZE, 1, fp);
    fwrite(bPRGROM + 0x8000, INES_PRGROMBANKSIZE*SMB_NUM_PRGS, 1, fp);
    fwrite(bCHRROM, INES_CHRROMBANKSIZE*SMB_NUM_CHARS, 1, fp);

    fclose(fp);

    fr_SetDataChanged(FALSE);
    SetROMFileTime();

    return TRUE;
}
