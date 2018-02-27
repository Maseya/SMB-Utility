#include "smbutil.h"
#include "objlib.h"
#include "objviewbmp.h"

#define ASSISTBMP_1BMPWIDTH   16
#define ASSISTBMP_1BMPHEIGHT  16
#define ASSISTBMP_NUMBMPSHORIZONTAL   6
#define ASSISTBMP_NUMBMPSVERTICAL     2

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

    // ビットマップ無し用のダミーデータ
    // dummy data for no bitmap
    0, 0, 0,
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

    // 補助図形なしのダミーデータ
    // dummy data without auxiliary figure
    0, 0, 0,
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
    0, -8, 14, //パタパタ(跳ねる) - Scattering (bouncing)
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
    BOOL blRet = FALSE;
    HDC hDC;

    hBitmap = LoadImage(GetModuleHandle(NULL), __T("ASSIST_IMG"), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
    if (!hBitmap) return FALSE;

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
    case 0x0D:
    case 0x0E:
    case 0x0F:
        break;
    default:
        if (!(lpbBuf[1] & 0x70))
        {
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
        else if (lpbBuf[1] & 0x08)
        {
            iRet = MAPASSISTBMPDATAINDEX_ARROWDOWN;
        }
        break;
    }

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
    case 0x0E:
    case 0x0F:
        break;

        //（敵キャラコマンド）
        // (Enemy character command)
    default:
    {
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
        {
            // ファイアバー
            // Fire bar
            iRet = iFireBars[bType - 0x1B];
        }
        else if (0x25 <= bType && bType <= 0x2C)
        {
            // リフト
            // Lift
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
