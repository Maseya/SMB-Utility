/**********************************************************************

                                  smb Utility

  File: objdata.h
  Description:
  History:

 *********************************************************************/

#pragma once

#include "common.h"

void InitObjectData();

typedef struct {
    LPTSTR Name;
}SMBMAPBASICBLOCK;

int GetNum0BMapObject();
int GetNumCMapObject();
int GetNumDMapObject();
int GetNumEMapObject();
int GetNumFMapObject();
