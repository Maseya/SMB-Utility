/************************************************************************************

                                  smb Utility

  File: resource.h
  Description:
  History:

 ************************************************************************************/
#ifndef RESOURCE_H
#define RESOURCE_H

#define IDS_MENUHELP_OPEN               1
#define IDS_MENUHELP_SAVE               2

#define	IDM_FILE_OPEN         1000
#define IDM_FILE_SAVE         1001
#define IDM_FILE_SAVEAS       1002
#define IDM_FILE_EXIT         1003
#define IDM_FILE_CHRLOAD      1010
#define IDM_FILE_IPS          1018

#define IDM_SETTING_AREA      1004
#define IDM_SETTING_MAPHEAD   1005
#define IDM_SETTING_MAP       1006
#define IDM_SETTING_BADGUYS   1007
#define IDM_SETTING_GAME      1008

#define IDM_EDIT_STRINGS      1009
#define IDM_EDIT_LOOP         1011
#define IDM_EDIT_AREASORT     1012
#define IDM_EDIT_LOOPWIZARD    1013
#define IDM_EDIT_UNDO          1017

#define IDM_TOOL_GENERALSETTING  1040
#define IDM_TOOL_WORLDDATAUPDATE 1041
#define IDM_TOOL_OPTION          1042
#define IDM_TOOL_DEMORECORD      1043
#define IDM_TOOL_CUSTOMIZE       1044
//#define IDM_TOOL_DUMPPRG         1045
//#define IDM_TOOL_DUMPCHR         1046
//#define IDM_TOOL_DUMPTRAINER     1047



#define IDM_VIEW_TOOLBAR 1070
#define IDM_VIEW_STATUSBAR 1071
#define IDM_VIEW_LOGVIEW   1072


#define IDM_EMULATOR_NORMALPLAY      1020
#define IDM_EMULATOR_LOADPLAY        1021
#define IDM_EMULATOR_TESTPLAYSETTING 1022
//#define IDM_EMULATOR_FRAMESKIP0      1023
//#define IDM_EMULATOR_FRAMESKIP1      1024
//#define IDM_EMULATOR_FRAMESKIP2      1025
//#define IDM_EMULATOR_FRAMESKIP3      1026
#define IDM_EMULATOR_SAVE            1027
#define IDM_EMULATOR_LOAD            1028
#define IDM_EMULATOR_STOP            1029
#define IDM_EMULATOR_PAGEPLAY        1019
#define IDM_EMULATOR_PAGEPLAYHALF    1050


#define IDM_WINDOW_NEXT                 1030
#define IDM_WINDOW_PREV                 1031
#define IDM_WINDOW_CLOSEALL             1032
#define IDM_WINDOW_CASCADE              1033
#define IDM_WINDOW_CHILD                4000

#define	IDM_HELP_VERSION      1060

/*
#define IDM_MAPVIEW_SCREENSIZE_1 1040
#define IDM_MAPVIEW_SCREENSIZE_2 1041
#define IDM_MAPVIEW_SCREENSIZE_3 1042
*/
#define IDM_MAPVIEW_NEXTPAGE        1100
#define IDM_MAPVIEW_PREVPAGE        1101
#define IDM_MAPVIEW_SHOWEDITDIALOG  1102
#define IDM_MAPVIEW_SENDOBJECT      1103
#define IDM_MAPVIEW_TESTPLAY        1104
#define IDM_MAPVIEW_SWITCHEDITMODE  1105

#define IDC_STATIC            -1

#define IDC_BIT6              2000
#define IDC_PAGE              2001
#define IDC_PAGESPIN          2002
#define IDC_XPOS              2003
#define IDC_XPOSSPIN          2004
#define IDC_YPOS              2005
#define IDC_YPOSSPIN          2006
#define IDC_TYPE              2007
#define IDC_LEN               2008
#define IDC_LENSPIN           2009
#define IDC_BIN               2010
#define IDC_ISBIN             2011
#define IDC_ISADDRESS         2015
//#define IDC_APPLY             2168

#define IDC_STATIC_XPOS1                2154
#define IDC_STATIC_YPOS                 2155
#define IDC_STATIC_TYPE                 2156
#define IDC_STATIC_OBJECT               2157
#define IDC_STATIC_ROOMMOVE             2158
#define IDC_STATIC_XPOS2                2159
#define IDC_STATIC_ROOM                 2160
#define IDC_STATIC_WORLD                2161
#define IDC_STATIC_PAGE                 2162

#define IDC_STATIC_BLOCK                2163
#define IDC_STATIC_VIEW                 2164
#define IDC_STATIC_BACKCOLOR            2165
#define IDC_STATIC_LENGTH               2166

#define IDC_AREA                        2012
#define IDC_AREASPIN                    2013
#define IDC_PAGEFLAG                    2014

#define IDC_NOTE                        2051
#define IDC_TITLE                       2052
#define IDC_CONTRIBUTION                2204

#define IDC_ADDRESS                     2016

#define IDC_BROWSE                      2168


#define IDC_TIME                        2017
#define IDC_POSITION                    2018
#define IDC_BACKCOLOR                   2019
#define IDC_MAPTYPE                     2020
#define IDC_VIEW                        2021
#define IDC_FIRSTBLOCK                  2022
#define IDC_MAPATTR                     2023

//マップ全般の設定
#define IDC_MARIOLEFT                   2030
#define IDC_MARIOLEFTSPIN               2031
#define IDC_ISWORLD                     2032
#define IDC_TIME400                     2150
#define IDC_TIME300                     2151
#define IDC_TIME200                     2152

#define IDC_WORLD                       2056
#define IDC_RETURNPOS                   2059
#define IDC_DATA1                       2060
#define IDC_DATA2                       2061
#define IDC_DATA3                       2062
#define IDC_DATA4                       2063
//文字列の編集
#define IDC_STRINGSELECT                2040
#define IDC_STRING                      2041
#define IDC_STRINGWRITE                 2042

#define IDC_DATA                        2050
#define IDC_POLEGFX                     2054
#define IDC_FLOWER                      2053

#define IDC_WARPA1                      2067
#define IDC_WARPASPIN1                  2068
#define IDC_WARPA2                      2073
#define IDC_WARPASPIN2                  2074
#define IDC_WARPA3                      2075
#define IDC_WARPASPIN3                  2076
#define IDC_WARPB1                      2077
#define IDC_WARPBSPIN1                  2078
#define IDC_WARPC1                      2079
#define IDC_WARPCSPIN1                  2080
#define IDC_WARPC2                      2081
#define IDC_WARPCSPIN2                  2082
#define IDC_WARPC3                      2083
#define IDC_WARPCSPIN3                  2084
#define IDC_WARPB2                      2085
#define IDC_WARPBSPIN2                  2086
#define IDC_WARPB3                      2087
#define IDC_WARPBSPIN3                  2088

#define IDC_COINSFOR1UP                 2117
#define IDC_COINSFOR1UPSPIN             2118


#define IDC_KOOPA                       2134
#define IDC_KOOPAWORLD                  2135

#define IDC_CLEARWORLD                  2142
#define IDC_CLEARWORLDSPIN              2143
#define IDC_ISCLEARWORLD                2144
#define IDC_DIFFICULTYWORLD             2145
#define IDC_DIFFICULTYWORLDSPIN         2146
#define IDC_DIFFICULTYAREA              2147
#define IDC_DIFFICULTYAREASPIN          2148
#define IDC_SEABLOCKWORLD               2169
#define IDC_SEABLOCKWORLDSPIN           2170

//
#define IDC_ISCLEARED                   2093
#define IDC_MARIOSTATE                  2094
#define IDC_FROMCURPAGE                 2095
#define IDC_FROMDESTPAGE                2096
#define IDC_0PAGEBADGUYS                2097
#define IDC_MARIOSTART                  2098
#define IDC_ISMARIOSTART                2099
#define IDC_INVINCIBLE                  2211

#define IDC_XPOS2                       2008
#define IDC_XPOS2SPIN                   2009
#define IDC_PAGEFLAG2                   2015
#define IDC_ISPAGECOMMAND               2095
#define IDC_PAGEEDIT                    2096
#define IDC_PAGEEDITSPIN                2097
#define IDC_WORLDSPIN                   2099
#define IDC_PAGEEDIT2                   2100
#define IDC_PAGEEDIT2SPIN               2101

#define IDC_OPENPREVIEW                 2102

//#define IDC_LOOPWIZARD                  2102

#define IDC_UP                          2110
#define IDC_DOWN                        2111

#define IDC_CHANGE                      2106

#define IDC_MAPVIEW                     2107
#define IDC_ROOMID                      2108
#define IDC_ISROOMID                    2113

#define IDC_ROOM                        2114
#define IDC_OFFSET                      2115

#define IDC_FRAMESKIP                   2116
#define IDC_SPEEDLIMIT                  2149
#define IDC_PALOPTIMIZE                 2153
#define IDC_SPEEDOPTIMIZE               2171
#define IDC_SKIPJOYREAD                 2172
#define IDC_USEPOV                      2213
#define IDC_JOYSTICKID                  2214
#define IDC_STATIC_JOYSTICKID           2215


#define IDC_HELPPATH            2167

#define IDC_MSGONSAVE                   2196

#define IDC_HALFPAGE                    2198

#define IDC_XPOSHACK                    2199
#define IDC_YPOSHACK                    2200

#define IDC_NOTDRAWASSISTBMP            2197

#define IDC_KEYLIST                     2187
#define IDC_KEYVALUE                    2188
#define IDC_KEYID                       2189
#define IDC_CTRL                        2190
#define IDC_SHIFT                       2191
#define IDC_ALT                         2192
#define IDC_RESET                       2193
#define IDC_PRESET                      2195
#define IDC_COPYRIGHT                   2194
#define IDC_WHEELCOMMAND                2201
#define IDC_WHEELLIST                   2202
#define IDC_JOYBUTTONVALUE              2203
#define IDC_JOYBUTTONLIST               2204


//ルーム選択ダイアログボックス

#define IDW_TOOLBAR 5000
#define TOOLBAR_IMG 3002
#define IDW_STATUSBAR 5002
#define IDW_MDICLIENT 5003
#define IDW_LISTVIEW  5004
#define IDB_XGAUGE_IMG 3000
#define IDB_YGAUGE_IMG 3001

#define IDM_EDITCOMMAND_ADD1TOTYPE  10000
#define IDM_EDITCOMMAND_DEC1TOTYPE  10001
#define IDM_EDITCOMMAND_ADD16TOTYPE 10002
#define IDM_EDITCOMMAND_DEC16TOTYPE 10003
#define IDM_EDITCOMMAND_RIGHT       10004
#define IDM_EDITCOMMAND_LEFT        10005
#define IDM_EDITCOMMAND_UP          10006
#define IDM_EDITCOMMAND_DOWN        10007
#define IDM_EDITCOMMAND_FWDPAGE     10008
#define IDM_EDITCOMMAND_REWPAGE     10009
#define IDM_EDITCOMMAND_NEXTOBJ     10010
#define IDM_EDITCOMMAND_PREVOBJ     10011

#endif /* RESOURCE_H */
