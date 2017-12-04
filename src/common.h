/************************************************************************************

                                  smb Utility

  File: common.h
  Description:
  History:

 ************************************************************************************/
#ifndef COMMON_H
#define COMMON_H

 /*****************

    General

 ******************/
#define SMB_NUM_PRGS  2
#define SMB_NUM_CHARS 1

#define INES_HEADERSIZE     0x10
#define INES_TRAINERSIZE    0x200
#define INES_PRGROMBANKSIZE 0x4000
#define INES_CHRROMBANKSIZE 0x2000

#define SMB_PRG_SIZE (INES_PRGROMBANKSIZE * SMB_NUM_PRGS)
#define PRG_SIZE     (SMB_PRG_SIZE + 0x8000)
#define SMB_CHR_SIZE (INES_CHRROMBANKSIZE * SMB_NUM_CHARS)

typedef struct
{
    BYTE cType[4];
    BYTE bNum_PRGs;
    BYTE bNum_CHARs;
    BYTE bROM_Type;
    BYTE bROM_Type2;
    BYTE Reserved[8];
}INESHEADER;

#define SMB_OBJECT_DATA_MAX_SIZE 3

#define SMB_MAX_PAGE 0x7F

typedef union
{
    struct
    {
        BYTE bLower;
        BYTE bUpper;
    }byte;
    WORD word;
}ADDRESSDATA;

#define ADDRESSDATA_LOAD(a,p) memcpy(&(a).byte.bLower,bPRGROM+(p),2)
#define ADDRESSDATA_GET(a) (a).word

typedef struct
{
    LPTSTR Name;
    BYTE  bBasicDataMask;
    BYTE  bBasicData;
    BYTE  bIsSizeValid;
    BYTE  bSizeMask;
    int   iSizeDelta;
    LPTSTR Note;
}SMBMAPOBJECTINFO;

typedef struct _tagSMBBADGUYSINFO
{
    LPTSTR Name;
    int YDelta;
    BYTE bFixedYPos;
    int XDelta;
}SMBBADGUYSINFO;

extern INESHEADER Head;
extern BYTE bPRGROM[INES_PRGROMBANKSIZE*SMB_NUM_PRGS + 0x8000];
extern BYTE bCHRROM[INES_CHRROMBANKSIZE*SMB_NUM_CHARS];
extern int iTrainer;

extern BOOL gblIsROMLoaded;

//extern BOOL gblDataChanged;

#endif /* COMMON_H */
