/************************************************************************************

                                  smb Utility

  File: filemng.h
  Description:
  History:

 ************************************************************************************/
#ifndef FILEMNG_H
#define FILEMNG_H

typedef struct _tagINESHEADER
{
	char cType[4];
	unsigned char bNum_PRGs;
	unsigned char bNum_CHRs;
	unsigned char bROM_CTRL1;
	unsigned char bROM_CTRL2;
	char cReserved[8];
}iNES_HEADER;

#define INES_ROMBANKSIZE   0x4000
#define INES_VROMBANKSIZE  0x2000
#define INES_TRAINERSIZE   0x200

BOOL SaveAsFile(LPSTR pFilename);

BOOL LoadROM(LPSTR pFilename);
// 書き換えられた - TRUE, 同じ - FALSE
BOOL CheckROMFileTime();

void SetROMFileTime();

#endif /* FILEMNG_H */