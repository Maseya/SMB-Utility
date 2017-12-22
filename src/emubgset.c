﻿/************************************************************************************

                                  smb Utility

  File: emubgset.c
  Description:ビューで敵を表示するためにスプライトRAMにデータを書き込むルーチン
  History:

 ************************************************************************************/
#include "smbutil.h"
#include "M6502.h"
#include "emulator.h"
#include "objlib.h"
#include "roommng.h"
#include "emubgset.h"

typedef struct
{
    int iXTiles;
    int iYTiles;
    int iYTileDelta;
    int iXPixelDelta;
    int iYPixelDelta;
    BYTE bTileData[24];
    BYTE bColors;
    DWORD dwXFlip;
    DWORD dwYFlip;
}BADGUYSSPRITEINFO;

BADGUYSSPRITEINFO g_sBadGuysSpriteInfo[] = {

    //青いのこのこ
    2,3,1,0,0,0xA0,0xFC,0xA2,0xA1,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000003F,0x00000000,

    //赤いのこのこ
    2,3,1,0,0,0xA0,0xFC,0xA2,0xA1,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000003F,0x00000000,

    //メット
    2,2,2,0,0,0xAB,0xAA,0xAD,0xAC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x0000000F,0x00000000,

    //赤いのこのこ（往復）
    2,3,1,0,0,0xA0,0xFC,0xA2,0xA1,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000003F,0x00000000,

    //青いのこのこ
    2,3,1,0,0,0xA0,0xFC,0xA2,0xA1,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000003F,0x00000000,

    //ハンマーブロス
    2,3,1,0,0,0x7C,0x7D,0x88,0x89,0x8A,0x8B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000003F,0x00000000,

    //クリボー
    2,2,2,0,0,0x70,0x71,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x00000000,0x00000000,

    //ゲッソー
    2,3,1,0,0,0xDC,0xDC,0xDD,0xDD,0xDE,0xDE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x0000002A,0x00000000,

    //キラー
    2,2,2,0,0,0xE7,0xE8,0xE9,0xEA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x0000000F,0x00000000,

    //青いパタパタ（停止）
    2,3,1,0,0,0xA0,0x69,0xA2,0x6A,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000003F,0x00000000,

    //青いぷくぷく（直進）
    2,2,2,0,0,0xB3,0xB2,0xB5,0xB4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000003F,0x00000000,

    //赤いぷくぷく（直進）
    2,2,2,0,0,0xB3,0xB2,0xB5,0xB4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000003F,0x00000000,

    //バブル
    2,2,2,0,0,0xD0,0xD0,0xD7,0xD7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000000A,0x00000000,

    //パックンフラワー
    2,3,1,0,0,0xE5,0xE5,0xE6,0xE6,0xEB,0xEB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000002A,0x00000000,

    //青いパタパタ（跳ねる）
    2,3,1,0,0,0xA0,0x69,0xA2,0x6A,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000003F,0x00000000,

    //赤いパタパタ（上下）
    2,3,1,0,0,0xA0,0x69,0xA2,0x6A,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000003F,0x00000000,

    //青いパタパタ（左右）
    2,3,1,0,0,0xA0,0x69,0xA2,0x6A,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000003F,0x00000000,

    //ジュゲム
    2,3,1,0,0,0xB9,0xB8,0xBB,0xBA,0xBC,0xBC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x00000020,0x00000000,

    //トゲゾー
    2,2,2,0,0,0x9B,0x9A,0x9D,0x9C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000000F,0x00000000,

    //カラスみたいな赤いのこのこ
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //赤いプクプク（連続）
    2,2,4,0,0,0xB3,0xB2,0xB5,0xB4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000003F,0x00000000,

    //クッパの炎
    3,1,4,0,0,0x51,0x52,0x53,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //暴走
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //キラー連続
    2,2,4,0,0,0xE7,0xE8,0xE9,0xEA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x0000000F,0x00000000,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //ファイアーバー（右回転）
    1,6,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //ファイアーバー（右高速回転）
    1,6,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //ファイアーバー（左回転）
    1,6,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //ファイアーバー（左高速回転）
    1,6,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //ロングファイアーバー
    1,12,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //ファイアーバー
    1,6,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //ファイアーバー
    1,6,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //ファイアーバー
    1,6,0,3,3,0x65,0x65,0x65,0x65,0x65,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //リフト（天秤）
    6,1,0,0,0,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //リフト（上下）
    6,1,0,0,0,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //リフト（上昇）
    6,1,0,-4,0,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //リフト（下降）
    6,1,0,-4,0,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //リフト（左右）
    6,1,0,0,0,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //リフト（落下）
    6,1,0,0,0,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //リフト（前進）
    6,1,0,0,0,0x5B,0x5B,0x5B,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x0000000,

    //小さいリフト（上昇）
    3,1,0,-4,0,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //小さいリフト（下降）
    3,1,0,-4,0,0x5B,0x5B,0x5B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000000,0x00000000,

    //クッパ
    4,4,0,0,0,0xBE,0xBF,0xFC,0xFC,0xC0,0xC1,0xC3,0xC4,0xFC,0xC2,0xC5,0xC6,0xFC,0xFC,0xC7,0xC8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x0000FFFF,0x00000000,

    //生きているきのこ
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //きのこの出る音だけ
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //にせジャンプ台
    2,3,0,0,0,0xF2,0xF2,0xF3,0xF3,0xF2,0xF2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000002A,0x00000030,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //ワープゾーン
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //ピノキオ、ピーチ姫
    2,3,3,0,0,0xCD,0xCD,0xCE,0xCE,0xCF,0xCF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000002A,0x00000000,

    //暴走
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000,

    //２体のクリボー
    5,2,2,0,0,0x70,0x71,0xFC,0x70,0x71,0x72,0x73,0xFC,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x00000000,0x00000000,

    //３体のクリボー
    8,2,2,0,0,0x70,0x71,0xFC,0x70,0x71,0xFC,0x70,0x71,0x72,0x73,0xFC,0x72,0x73,0xFC,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x00000000,0x00000000,

    //２体のクリボー
    5,2,2,0,0,0x70,0x71,0xFC,0x70,0x71,0x72,0x73,0xFC,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x00000000,0x00000000,

    //３体のクリボー
    8,2,2,0,0,0x70,0x71,0xFC,0x70,0x71,0xFC,0x70,0x71,0x72,0x73,0xFC,0x72,0x73,0xFC,0x72,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3,0x00000000,0x00000000,

    //２体の青いのこのこ
    5,3,1,0,0,0xA0,0xFC,0xFC,0xA0,0xFC,0xA2,0xA1,0xFC,0xA2,0xA1,0xA4,0xA3,0xFC,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x00007FFF,0x00000000,

    //３体の青いのこのこ
    8,3,1,0,0,0xA0,0xFC,0xFC,0xA0,0xFC,0xFC,0xA0,0xFC,0xA2,0xA1,0xFC,0xA2,0xA1,0xFC,0xA2,0xA1,0xA4,0xA3,0xFC,0xA4,0xA3,0xFC,0xA4,0xA3,0x1,0x00FFFFFF,0x00000000,

    //２体の青いのこのこ
    5,3,1,0,0,0xA0,0xFC,0xFC,0xA0,0xFC,0xA2,0xA1,0xFC,0xA2,0xA1,0xA4,0xA3,0xFC,0xA4,0xA3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x00007FFF,0x00000000,

    //３体の青いのこのこ
    8,3,1,0,0,0xA0,0xFC,0xFC,0xA0,0xFC,0xFC,0xA0,0xFC,0xA2,0xA1,0xFC,0xA2,0xA1,0xFC,0xA2,0xA1,0xA4,0xA3,0xFC,0xA4,0xA3,0xFC,0xA4,0xA3,0x1,0x00FFFFFF,0x00000000,

    //無し
    0,0,0,0,0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0,0x00000000,0x00000000
};

extern BYTE pbSPRRAM[0x100];
static int SetBadGuysDataToSpriteRAM(int iPosX, int iPosY, BYTE bType, int iRAMIndex, int iPageDelta)
{
    int iYTiles, iYTileDelta, iXTiles, iXPixelDelta, iYPixelDelta, n, x, y;

    if (abs(iPageDelta) > 1) return iRAMIndex;

    iXTiles = g_sBadGuysSpriteInfo[bType].iXTiles;
    iYTiles = g_sBadGuysSpriteInfo[bType].iYTiles;
    iYTileDelta = g_sBadGuysSpriteInfo[bType].iYTileDelta;
    iXPixelDelta = g_sBadGuysSpriteInfo[bType].iXPixelDelta;
    iYPixelDelta = g_sBadGuysSpriteInfo[bType].iYPixelDelta;

    for (y = 0, n = 0; y < iYTiles; y++)
    {
        for (x = 0; x < iXTiles&&iRAMIndex < 64; x++, n++)
        {
            int iTmpX;

            iTmpX = iPosX * 16 + x * 8 + iXPixelDelta;

            //ﾍﾟｰｼﾞの境界をまたぐ敵の処理
            if (iTmpX >= NES_SCREENSIZEX)
            {
                if (!iPageDelta || iPageDelta == 1) continue;
            }
            else if (iTmpX < 0)
            {
                if (!iPageDelta || iPageDelta == -1) continue;
            }
            else
            {
                if (iPageDelta == -1 || iPageDelta == 1) continue;
            }

            pbSPRRAM[iRAMIndex * 4 + 3] = (BYTE)iTmpX;
            pbSPRRAM[iRAMIndex * 4 + 0] = iPosY * 16 + y * 8 + iYTileDelta * 8 + iYPixelDelta;
            pbSPRRAM[iRAMIndex * 4 + 1] = g_sBadGuysSpriteInfo[bType].bTileData[n];
            pbSPRRAM[iRAMIndex * 4 + 2] = 0;
            pbSPRRAM[iRAMIndex * 4 + 2] |= (g_sBadGuysSpriteInfo[bType].bColors & 0x03);
            pbSPRRAM[iRAMIndex * 4 + 2] |= (((g_sBadGuysSpriteInfo[bType].dwXFlip >> n) & 0x01) << 6);
            pbSPRRAM[iRAMIndex * 4 + 2] |= (((g_sBadGuysSpriteInfo[bType].dwYFlip >> n) & 0x01) << 7);

            iRAMIndex++;
        }
    }

    return iRAMIndex;
}

typedef struct
{
    int iXTiles;
    int iYTiles;
    int iYTileDelta;
    int iPixelX;
    int iPixelY;
    BYTE bTileData[24];
    BYTE bColors;
    DWORD dwXFlip;
    DWORD dwYFlip;
}BADGUYSSPRITEINFOEX;

static int SetMapDataToSpriteRAM(int iPosX, int iPosY, BADGUYSSPRITEINFOEX* psBGSPRInfoEx, int iRAMIndex, int iPageDelta)
{
    int iYTiles, iYTileDelta, iXTiles, iPixelX, iPixelY, n, x, y;

    if (abs(iPageDelta) > 1) return iRAMIndex;

    iXTiles = psBGSPRInfoEx->iXTiles;
    iYTiles = psBGSPRInfoEx->iYTiles;
    iYTileDelta = psBGSPRInfoEx->iYTileDelta;
    iPixelX = psBGSPRInfoEx->iPixelX;
    iPixelY = psBGSPRInfoEx->iPixelY;

    for (y = 0, n = 0; y < iYTiles; y++)
    {
        for (x = 0; x < iXTiles&&iRAMIndex < 64; x++, n++)
        {
            int iTmpX;

            iTmpX = iPosX * 16 + x * 8 + iPixelX;

            //ﾍﾟｰｼﾞの境界をまたぐ敵の処理
            if (iTmpX >= NES_SCREENSIZEX)
            {
                if (!iPageDelta || iPageDelta == 1) continue;
            }
            else if (iTmpX < 0)
            {
                if (!iPageDelta || iPageDelta == -1) continue;
            }
            else
            {
                if (iPageDelta == -1 || iPageDelta == 1) continue;
            }

            pbSPRRAM[iRAMIndex * 4 + 3] = (BYTE)iTmpX;
            pbSPRRAM[iRAMIndex * 4 + 0] = (BYTE)(iPosY * 16 + y * 8 + iYTileDelta * 8 + iPixelY);
            pbSPRRAM[iRAMIndex * 4 + 1] = psBGSPRInfoEx->bTileData[n];
            pbSPRRAM[iRAMIndex * 4 + 2] = 0;
            pbSPRRAM[iRAMIndex * 4 + 2] |= (psBGSPRInfoEx->bColors & 0x03);
            pbSPRRAM[iRAMIndex * 4 + 2] |= (((psBGSPRInfoEx->dwXFlip >> n) & 0x01) << 6);
            pbSPRRAM[iRAMIndex * 4 + 2] |= (((psBGSPRInfoEx->dwYFlip >> n) & 0x01) << 7);

            iRAMIndex++;
        }
    }
    return iRAMIndex;
}

extern struct
{
    LPTSTR Name; int YDelta; BYTE bFixedYPos; int XDelta;
} *smbBadGuysInfo;

static int GetBadGuysYPosPSRAM(BYTE *pbBuf)
{
    int iRet;
    if ((pbBuf[0] & 0x0F) != 0x0E)
    {
        if (smbBadGuysInfo[pbBuf[1] & 0x3F].bFixedYPos)
            iRet = smbBadGuysInfo[pbBuf[1] & 0x3F].bFixedYPos + 1;
        else
            iRet = (pbBuf[0] & 0x0F);
    }
    else
    {
        iRet = 0x0E;
    }
    return iRet;
}

void PrepareSpriteRAM(UINT uRoomID, int iPage)
{
    OBJECTSEEKINFO ObjSeek;
    int iRAMIndex = 0;

    memset(pbSPRRAM, 0xFF, 0x100);

    //敵
    if (BadGuysSeekFirst(&ObjSeek, uRoomID))
    {
        for (;;)
        {
            if (ObjSeek.dwPage
                && (ObjSeek.pbData[0] & 0x0F) != 0x0F
                && (ObjSeek.pbData[0] & 0x0F) != 0x0E)
            {
                iRAMIndex = SetBadGuysDataToSpriteRAM(GetBadGuysXPos(ObjSeek.pbData), GetBadGuysYPosPSRAM(ObjSeek.pbData), (BYTE)(ObjSeek.pbData[1] & 0x3F), iRAMIndex, ObjSeek.dwPage - iPage);
            }

            if (!BadGuysSeekNext(&ObjSeek)) break;
        }
    }

    //ﾏｯﾌﾟの補助ｵﾌﾞｼﾞｪｸﾄ
    if (MapSeekFirst(&ObjSeek, uRoomID))
    {
        BADGUYSSPRITEINFOEX sMapSprInfo[] =
        {
            //ジャンプ台
            2,3,4,0,0,0xF2,0xF2,0xF3,0xF3,0xF2,0xF2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000002A,0x00000030,

            //ポールの旗
            2,2,0,-8,0,0x7E,0x7F,0xFC,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1,0x00000000,0x00000030,
        };
        int iYPos, iType;

        for (;;)
        {
            iYPos = GetMapYPos(ObjSeek.pbData);
            iType = (ObjSeek.pbData[1] & 0x7F);

            //
            if ((0 <= iYPos && iYPos <= 0x0B) && iType == 0x0B)
            {
                iRAMIndex = SetMapDataToSpriteRAM(GetMapXPos(ObjSeek.pbData), iYPos, &sMapSprInfo[0], iRAMIndex, ObjSeek.dwPage - iPage);
            }

            //
            else if ((iYPos == 0x0D && iType == 0x41) || ((0 <= iYPos && iYPos <= 0x0B) && iType == 0x0D))
            {
                iRAMIndex = SetMapDataToSpriteRAM(GetMapXPos(ObjSeek.pbData), 3, &sMapSprInfo[1], iRAMIndex, ObjSeek.dwPage - iPage);
            }

            if (!MapSeekNext(&ObjSeek)) break;
        }
    }
}

void SetPrepareBadGuysSpriteInfoStruct(BYTE bRoomID, int iWorld, int iArea, PREPAREBGSPRINFO *psPreBSI)
{
    BYTE bBuf[2];

    memset(psPreBSI, 0, sizeof(PREPAREBGSPRINFO));

    if (!psPreBSI) return;

    if (iWorld >= (GetNumWorlds() - 1))
        psPreBSI->blPeach = TRUE;

    if (bPRGROM[0x903B] <= iWorld && bPRGROM[0x9044] <= iArea)
        psPreBSI->blIsLiftSmall = TRUE;

    if (((bRoomID >> 5) & 0x03) == 0x03)
        psPreBSI->blIsLiftSmall = TRUE;

    GetMapHeadData(bRoomID, bBuf);
    if (((bBuf[1] >> 6) & 0x03) == 0x03)
        psPreBSI->blIsLiftCloud = TRUE;
}

void PrepareBadGuysSpriteInfo(PREPAREBGSPRINFO *psPreBSI)
{
    int n;

    if (psPreBSI->blIsLiftSmall)
    {
        for (n = 0x24; n <= 0x2A; n++)
        {
            g_sBadGuysSpriteInfo[n].iXTiles = 4;
            if (n == 0x24)g_sBadGuysSpriteInfo[n].iXPixelDelta = 8;
        }
    }
    else
    {
        for (n = 0x24; n <= 0x2A; n++)
        {
            g_sBadGuysSpriteInfo[n].iXTiles = 6;
            if (n == 0x24)g_sBadGuysSpriteInfo[n].iXPixelDelta = 0;
        }
    }

    if (psPreBSI->blIsLiftCloud)
    {
        for (n = 0x24; n <= 0x2A; n++)
            memset(g_sBadGuysSpriteInfo[n].bTileData, 0x75, 6);
    }
    else
    {
        for (n = 0x24; n <= 0x2A; n++)
            memset(g_sBadGuysSpriteInfo[n].bTileData, 0x5B, 6);
    }

    if (psPreBSI->blPeach)
    {
        BADGUYSSPRITEINFO bgspiPeach = {2,3,3,0,0,0x7A,0x7B,0xDA,0xDB,0xD8,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x00000020,0x00000000};
        memcpy(&g_sBadGuysSpriteInfo[0x35], &bgspiPeach, sizeof(BADGUYSSPRITEINFO));
    }
    else
    {
        BADGUYSSPRITEINFO bgspiToad = {2,3,3,0,0,0xCD,0xCD,0xCE,0xCE,0xCF,0xCF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2,0x0000002A,0x00000000};
        memcpy(&g_sBadGuysSpriteInfo[0x35], &bgspiToad, sizeof(BADGUYSSPRITEINFO));
    }
}
