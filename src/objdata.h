/**********************************************************************

                                  smb Utility

  File: objdata.h
  Description:
  History:

 *********************************************************************/

#pragma once

#include "common.h"

void InitObjectData();

extern SMBMAPOBJECTINFO* smbMapObjectInfo0B;
extern SMBMAPOBJECTINFO* smbMapObjectInfoC;
extern SMBMAPOBJECTINFO* smbMapObjectInfoD;
extern SMBMAPOBJECTINFO* smbMapObjectInfoE;
extern SMBMAPOBJECTINFO* smbMapObjectInfoF;

typedef struct {
    LPTSTR Name;
}SMBMAPBASICBLOCK;
extern SMBMAPBASICBLOCK* smbMapBasicBlock;

extern LPTSTR* smbMapHeadTime;
extern LPTSTR* smbMapHeadPosition;
extern LPTSTR* smbMapHeadBackColor;
extern LPTSTR* smbMapHeadMapType;
extern LPTSTR* smbMapHeadView;

extern SMBBADGUYSINFO* smbBadGuysInfo;

int GetNum0BMapObject();
int GetNumCMapObject();
int GetNumDMapObject();
int GetNumEMapObject();
int GetNumFMapObject();
