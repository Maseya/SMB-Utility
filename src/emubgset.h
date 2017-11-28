/************************************************************************************

                                  smb Utility

  File: emubgset.h
  Description:
  History:

 ************************************************************************************/
#ifndef EMUBGSET_H
#define EMUBGSET_H

typedef struct
{
	BOOL blIsLiftSmall;
	BOOL blIsLiftCloud;
	BOOL blPeach;
}PREPAREBGSPRINFO;
/*********************


**********************/
void PrepareSpriteRAM(UINT uRoomID,int iPage);
/*********************


**********************/
void SetPrepareBadGuysSpriteInfoStruct(BYTE bRoomID,int iWorld,int iArea,PREPAREBGSPRINFO *psPreBSI);
/*********************


**********************/
void PrepareBadGuysSpriteInfo(PREPAREBGSPRINFO *psPreBSI);
#endif