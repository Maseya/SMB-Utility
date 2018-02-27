/**********************************************************************

                                  smb Utility

  File: objdata.h
  Description:
  History:

 *********************************************************************/

#pragma once

#include "common.h"

void InitObjectData();

SMBMAPOBJECTINFO *smbMapObjectInfo0B;
SMBMAPOBJECTINFO *smbMapObjectInfoC;
SMBMAPOBJECTINFO *smbMapObjectInfoD;
SMBMAPOBJECTINFO *smbMapObjectInfoE;
SMBMAPOBJECTINFO *smbMapObjectInfoF;

struct
{
    LPTSTR Name;
}*smbMapBasicBlock;

LPTSTR *smbMapHeadTime;
LPTSTR *smbMapHeadPosition;
LPTSTR *smbMapHeadBackColor;
LPTSTR *smbMapHeadMapType;
LPTSTR *smbMapHeadView;

SMBBADGUYSINFO *smbBadGuysInfo;

int GetNum0BMapObject();
int GetNumCMapObject();
int GetNumDMapObject();
int GetNumEMapObject();
int GetNumFMapObject();
