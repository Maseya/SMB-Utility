/**********************************************************************

                                  smb Utility

  File: filemng.h
  Description:
  History:

 *********************************************************************/
#ifndef FILEMNG_H
#define FILEMNG_H

typedef struct _tagINESHEADER
{
    BYTE cType[4];
    BYTE bNum_PRGs;
    BYTE bNum_CHRs;
    BYTE bROM_CTRL1;
    BYTE bROM_CTRL2;
    BYTE cReserved[8];
}iNES_HEADER;

#define INES_ROMBANKSIZE   0x4000
#define INES_VROMBANKSIZE  0x2000
#define INES_TRAINERSIZE   0x200

BOOL SaveAsFile(LPTSTR pFilename);

BOOL LoadROM(LPTSTR pFilename);

// 書き換えられた - TRUE, 同じ - FALSE
// rewritten - TRUE, same - FALSE
BOOL CheckROMFileTime();

void SetROMFileTime();

#endif /* FILEMNG_H */
