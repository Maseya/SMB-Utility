#include "smbutil.h"
#include "objlib.h"
#include "objviewbmp.h"

#define ASSISTBMP_1BMPWIDTH   16
#define ASSISTBMP_1BMPHEIGHT  16
#define ASSISTBMP_NUMBMPSHORIZONTAL   6//
#define ASSISTBMP_NUMBMPSVERTICAL     2//

#define MAPASSISTBMPDATAINDEX_QBLOCKPOWERMUSH 1
#define MAPASSISTBMPDATAINDEX_QBLOCKCOIN      2
#define MAPASSISTBMPDATAINDEX_HIDDENCOIN      3
#define MAPASSISTBMPDATAINDEX_HIDDEN1UPMUSH   4
#define MAPASSISTBMPDATAINDEX_BRICKPOWERMUSH  5
#define MAPASSISTBMPDATAINDEX_BRICKIVY        6
#define MAPASSISTBMPDATAINDEX_BRICKSTAR       7
#define MAPASSISTBMPDATAINDEX_BRICK10COINS    8
#define MAPASSISTBMPDATAINDEX_BRICK1UPMUSH    9
#define MAPASSISTBMPDATAINDEX_ARROWDOWN       10

ASSISTBMPDATA g_MapAssistBmpData[] = {
    0, 0, 0, // ビットマップ無し用のダミーデータ
    0, 0, 6,
    0, 0, 8,
    0, 0, 8,
    0, 0, 7,
    0, 0, 6,
    0, 0, 10,
    0, 0, 11,
    0, 0, 9,
    0, 0, 7,
    8, -16, 1
};

#define BADGUYSASSISTBMPDATAINDEX_FIREBARRIGHT      1
#define BADGUYSASSISTBMPDATAINDEX_FIREBARRIGHTFAST  2
#define BADGUYSASSISTBMPDATAINDEX_FIREBARLEFT       3
#define BADGUYSASSISTBMPDATAINDEX_FIREBARLEFTFAST   4
#define BADGUYSASSISTBMPDATAINDEX_FIREBARLONG       5
#define BADGUYSASSISTBMPDATAINDEX_LIFTTOSS          6
#define BADGUYSASSISTBMPDATAINDEX_LIFTUP            7
#define BADGUYSASSISTBMPDATAINDEX_LIFTDOWN          8
#define BADGUYSASSISTBMPDATAINDEX_LIFTROLL          9
#define BADGUYSASSISTBMPDATAINDEX_LIFTFALL          10
#define BADGUYSASSISTBMPDATAINDEX_LIFTADVANCE       11
#define BADGUYSASSISTBMPDATAINDEX_LIFTUPSMALL       12
#define BADGUYSASSISTBMPDATAINDEX_LIFTDOWNSMALL     13
#define BADGUYSASSISTBMPDATAINDEX_REDNOKONOKO       14
#define BADGUYSASSISTBMPDATAINDEX_PATAPATAJUMP      15
#define BADGUYSASSISTBMPDATAINDEX_PATAPATATOSS      16
#define BADGUYSASSISTBMPDATAINDEX_PATAPATAROLL      17
#define BADGUYSASSISTBMPDATAINDEX_CONTINUEOUS       18
#define BADGUYSASSISTBMPDATAINDEX_NOMOVE            19

ASSISTBMPDATA g_BadGuysAssistBmpData[] = {
    0, 0, 0, //補助図形なしのダミーデータ
    0, 0, 12,
    0, 0, 13,
    0, 0, 14,
    0, 0, 15,
    0, 0, 12,
    0, -16, 4,
    0, -16, 0,
    0, -16, 1,
    0, -16, 5,
    0, -16, 1,
    0, -16, 3,
    0, -16, 0,
    0, -16, 1,
    0, -8, 16,
    0, -8, 14, //パタパタ(跳ねる)
    0, -8, 17,
    0, -8, 16,
    0, -8, 14,
    0,  0, 18
};

HDC g_hAssistBmpDC = NULL;
HBITMAP g_hOldBmp = NULL;

VOID TrashAssistBmp()
{
    if (g_hAssistBmpDC)
    {
        SelectObject(g_hAssistBmpDC, g_hOldBmp);
        DeleteDC(g_hAssistBmpDC);
        g_hAssistBmpDC = NULL;
    }
}

BOOL InitAssistBmp(HWND hWnd)
{
    HBITMAP hBitmap;

    //	HBITMAP hOldBm;
    BOOL blRet = FALSE;

    //	BITMAP sBmp;
    //	DWORD nWidth,nHeight;
    HDC hDC;

    hBitmap = LoadImage(GetModuleHandle(NULL), "ASSIST_IMG", IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
    if (!hBitmap) return FALSE;

    //	if(!GetObject(hBitmap,sizeof(BITMAP),&sBmp)) return FALSE;
    //	nWidth=sBmp.bmWidth;
    //	nHeight=sBmp.bmHeight;

    hDC = GetDC(hWnd);
    if (g_hAssistBmpDC = CreateCompatibleDC(hDC))
    {
        if (g_hOldBmp = SelectObject(g_hAssistBmpDC, hBitmap))
        {
            blRet = TRUE;
        }
    }

    ReleaseDC(hWnd, hDC);

    return blRet;
}

static int GetMapAssistBmpDataIndex(LPBYTE lpbBuf)
{
    int iRet = 0;

    switch ((lpbBuf[0] & 0x0f))
    {
    case 0x0C:

        //wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfoC[(lpbBuf[1] >> 4) & 0x07].Name);
        break;
    case 0x0D:
        if (!(lpbBuf[1] & 0x40))
        {
            //wsprintf(lpszBuf, "%s:%.2d", smbMapObjectInfoD[0].Name, lpbBuf[1] & 0x3F);
        }
        else
        {
            if ((lpbBuf[1] & 0x70) == 0x40)
            {
                //wsprintf(lpszBuf, "%s", smbMapObjectInfoD[(lpbBuf[1] & 0x0F) + 1].Name);
            }
            else
            {
                //wsprintf(lpszBuf, STRING_OBJLIST_UNKNOWN);
            }
        }
        break;
    case 0x0E:
        if (lpbBuf[1] & 0x40)
        {
            //wsprintf(lpszBuf, STRING_OBJLIST_BACK, smbMapHeadBackColor[lpbBuf[1] & 0x07]);
        }
        else
        {
            //wsprintf(lpszBuf, STRING_OBJLIST_VIEWBLOCK, smbMapHeadView[(lpbBuf[1] >> 4) & 0x03], smbMapBasicBlock[lpbBuf[1] & 0x0F].Name);
        }
        break;
    case 0x0F:
    {
        switch ((lpbBuf[1] >> 4) & 0x07)
        {
        case 0:

            //wsprintf(lpszBuf, STRING_OBJLIST_ROPE);
            break;

            // オブジェクトのデータベースの都合(1と4,5との間には、追加の要素が1つ入っているため)により、
            // 1と4,5は違う処理
        case 1:

            //				wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfoF[((lpbBuf[1] >> 4) & 0x07)].Name);
            break;
        case 4:
        case 5:

            //				wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfoF[((lpbBuf[1] >> 4) & 0x07) + 1].Name);
            break;
        case 2:
        {
            BYTE bHeight;
            bHeight = lpbBuf[1] & 0x0F;
            if (0x00 <= bHeight && bHeight <= 0x0B)
            {
                //wsprintf(lpszBuf, STRING_OBJLIST_LENCASTLE, 0x0B - bHeight + 1);
            }
            else
            {
                //wsprintf(lpszBuf, STRING_OBJLIST_CRASH);
            }
        }
        break;
        case 3:
            if (!((lpbBuf[1] >> 3) & 0x01))
            {
                //wsprintf(lpszBuf,STRING_OBJLIST_STEP,(lpbBuf[1]&0x07)+1,(lpbBuf[1]&0x07)+1);
            }
            else
            {
                BYTE bLower;
                bLower = (lpbBuf[1] & 0x07);
                if (0 <= bLower && bLower <= 0x03)
                {
                    //wsprintf(lpszBuf, STRING_OBJLIST_STEP98);
                }
                else if (0x04 <= bLower && bLower <= 0x06)
                {
                    //wsprintf(lpszBuf, STRING_OBJLIST_STEP98_2);
                }
                else if (0x07 == bLower)
                {
                    //wsprintf(lpszBuf, STRING_OBJLIST_STEP98_2);
                }
            }
            break;
        case 6:
        case 7:

            //wsprintf(lpszBuf, STRING_OBJLIST_NONE);
            break;
        }
    }
    break;
    default:
        if (!(lpbBuf[1] & 0x70))
        {
            //wsprintf(lpszBuf,"%s",smbMapObjectInfo0B[lpbBuf[1] & 0x0F].Name);
            BYTE bType = lpbBuf[1] & 0x0F;
            int iItemBlocks[] = {
                MAPASSISTBMPDATAINDEX_QBLOCKPOWERMUSH,
                MAPASSISTBMPDATAINDEX_QBLOCKCOIN,
                MAPASSISTBMPDATAINDEX_HIDDENCOIN,
                MAPASSISTBMPDATAINDEX_HIDDEN1UPMUSH,
                MAPASSISTBMPDATAINDEX_BRICKPOWERMUSH,
                MAPASSISTBMPDATAINDEX_BRICKIVY,
                MAPASSISTBMPDATAINDEX_BRICKSTAR,
                MAPASSISTBMPDATAINDEX_BRICK10COINS,
                MAPASSISTBMPDATAINDEX_BRICK1UPMUSH};

            if (bType < 0x09)
                iRet = iItemBlocks[bType];
        }
        else if ((lpbBuf[1] & 0x70) != 0x70)
        {
            //wsprintf(lpszBuf, STRING_OBJLIST_LENNAME, (lpbBuf[1] & 0x0F) + 1, smbMapObjectInfo0B[0x0F + ((lpbBuf[1] >> 4) & 0x07)].Name);
        }
        else
        {
            if (lpbBuf[1] & 0x08)
            {//土管
//wsprintf(lpszBuf, STRING_OBJLIST_LENNAME,(lpbBuf[1] & 0x07) + 1, smbMapObjectInfo0B[0x17].Name);
                iRet = MAPASSISTBMPDATAINDEX_ARROWDOWN;
            }
            else
            {
                //wsprintf(lpszBuf, STRING_OBJLIST_LENNAME,(lpbBuf[1] & 0x0F) + 1, smbMapObjectInfo0B[0x16].Name);
            }
        }
        break;
    }/* switch */

    return iRet;
}

void BitBltAssistBmp(HDC hDestMemDC, LPASSISTBMPDATA lpABD, int x, int y)
{
    if (lpABD && hDestMemDC)
    {
        BitBlt(hDestMemDC,
               x + lpABD->iPosDeltaX, y + lpABD->iPosDeltaY,
               ASSISTBMP_1BMPWIDTH, ASSISTBMP_1BMPHEIGHT,
               g_hAssistBmpDC,
               (lpABD->iBmpIndex % ASSISTBMP_NUMBMPSHORIZONTAL) * ASSISTBMP_1BMPWIDTH,
               (lpABD->iBmpIndex / ASSISTBMP_NUMBMPSHORIZONTAL) * ASSISTBMP_1BMPHEIGHT * 2 + ASSISTBMP_1BMPHEIGHT,
               SRCAND);

        BitBlt(hDestMemDC,
               x + lpABD->iPosDeltaX, y + lpABD->iPosDeltaY,
               ASSISTBMP_1BMPWIDTH, ASSISTBMP_1BMPHEIGHT,
               g_hAssistBmpDC,
               (lpABD->iBmpIndex % ASSISTBMP_NUMBMPSHORIZONTAL) * ASSISTBMP_1BMPWIDTH,
               (lpABD->iBmpIndex / ASSISTBMP_NUMBMPSHORIZONTAL) * ASSISTBMP_1BMPHEIGHT * 2,
               SRCPAINT);
    }
}

BOOL DrawMapAssistBmpData(LPOBJECTSEEKINFO lpOSI, HDC hDestMemDC, int x, int y)
{
    int iBmpDataIndex;

    iBmpDataIndex = GetMapAssistBmpDataIndex(lpOSI->pbData);

    if (!iBmpDataIndex) return TRUE;

    BitBltAssistBmp(hDestMemDC, &g_MapAssistBmpData[iBmpDataIndex], x, y);

    return TRUE;
}

static int GetBadGuysAssistBmpDataIndex(LPBYTE lpbBuf)
{
    int iRet = 0;
    switch (lpbBuf[0] & 0x0F)
    {
    case 0x0E://ルーム間移動の命令（３バイト）
    {
        //			LPTSTR lpAttr[] = {STRING_SEA, STRING_SKY, STRING_UNDERGROUND, STRING_CASTLE};
        //			wsprintf(lpszBuf, STRING_OBJLIST_ROOM, lpbBuf[1] & 0x7F, lpAttr[(lpbBuf[1] >> 5) & 0x03], ((lpbBuf[2] >> 5) & 0x07) + 1,lpbBuf[2] & 0x1F);
    }
    break;
    case 0x0F://送りコマンド（２バイト）

//		wsprintf(lpszBuf, STRING_OBJLIST_PAGECOMMAND, lpbBuf[1]);
        break;
    default://（敵キャラコマンド）
    {
        //			LPTSTR bit6[]={"", STRING_OBJLIST_HARD};
        //			wsprintf(lpszBuf, "%s%s", smbBudGuysInfo[lpbBuf[1] & 0x3f].Name, bit6[(lpbBuf[1] >> 6) & 0x01]);
        BYTE bType = lpbBuf[1] & 0x3F;
        int iFireBars[] = {
            BADGUYSASSISTBMPDATAINDEX_FIREBARRIGHT,
            BADGUYSASSISTBMPDATAINDEX_FIREBARRIGHTFAST,
            BADGUYSASSISTBMPDATAINDEX_FIREBARLEFT,
            BADGUYSASSISTBMPDATAINDEX_FIREBARLEFTFAST,
            BADGUYSASSISTBMPDATAINDEX_FIREBARLONG,
        };
        int iLifts[] = {
            BADGUYSASSISTBMPDATAINDEX_LIFTTOSS,
            BADGUYSASSISTBMPDATAINDEX_LIFTUP,
            BADGUYSASSISTBMPDATAINDEX_LIFTDOWN,
            BADGUYSASSISTBMPDATAINDEX_LIFTROLL,
            BADGUYSASSISTBMPDATAINDEX_LIFTFALL,
            BADGUYSASSISTBMPDATAINDEX_LIFTADVANCE,
            BADGUYSASSISTBMPDATAINDEX_LIFTUPSMALL,
            BADGUYSASSISTBMPDATAINDEX_LIFTDOWNSMALL
        };

        if (0x1B <= bType && bType <= 0x1F)
        {//ファイアバー
            iRet = iFireBars[bType - 0x1B];
        }
        else if (0x25 <= bType && bType <= 0x2C)
        {//リフト
            iRet = iLifts[bType - 0x25];
        }
        else
        {
            switch (bType)
            {
            case 0x03:
                iRet = BADGUYSASSISTBMPDATAINDEX_REDNOKONOKO;
                break;
            case 0x0E:
                iRet = BADGUYSASSISTBMPDATAINDEX_PATAPATAJUMP;
                break;
            case 0x0F:
                iRet = BADGUYSASSISTBMPDATAINDEX_PATAPATATOSS;
                break;
            case 0x10:
                iRet = BADGUYSASSISTBMPDATAINDEX_PATAPATAROLL;
                break;
            case 0x14:
            case 0x17:
                iRet = BADGUYSASSISTBMPDATAINDEX_CONTINUEOUS;
                break;
            case 0x04:
            case 0x09:
            case 0x12:
                iRet = BADGUYSASSISTBMPDATAINDEX_NOMOVE;
            }
        }
    }
    }
    return iRet;
}

BOOL DrawBadguysAssistBmpData(LPOBJECTSEEKINFO lpOSI, HDC hDestMemDC, int x, int y)
{
    int iBmpDataIndex;

    iBmpDataIndex = GetBadGuysAssistBmpDataIndex(lpOSI->pbData);

    if (!iBmpDataIndex) return TRUE;

    BitBltAssistBmp(hDestMemDC, &g_BadGuysAssistBmpData[iBmpDataIndex], x, y);

    return TRUE;
}
