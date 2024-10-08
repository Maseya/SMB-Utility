﻿/**********************************************************************

smb Utility

File: string.h
Description: strings for international version
History:
Contributors: Chezzman1, Insectduel, MirracleMXX, Yy, spel werdz rite

*********************************************************************/

#pragma once

#ifndef STRINGS_H
#define STRINGS_H

#include "resrc1.h"

#include <Windows.h>

void InitResourceStrings();

extern LPTSTR PROGRAMNAME;
extern LPTSTR STRING_VERSION_CONTRIBUTION;
extern LPTSTR STRING_EMPTY;
extern LPTSTR STRING_UNKNOWN;
extern LPTSTR STRING_LOGVIEW_FILESAVE;
extern LPTSTR STRING_LOGVIEW_FILEOPEN;
extern LPTSTR STRING_LOGVIEW_LOADCHR;
extern LPTSTR STRING_FILEOPENDIALOG_CHRLOAD;
extern LPTSTR STRING_WINDOW_EMULATOR;
extern LPTSTR STRING_WINDOW_OBJLIST;
extern LPTSTR STRING_WINDOW_OBJVIEW;
extern LPTSTR STRING_OBJVIEW_TITLE;
extern LPTSTR STRING_OBJVIEW_TITLE2;
extern LPTSTR STRING_OBJVIEW_TOOLTIP;
extern LPTSTR STRING_STATUSBAR_EMULOAD;
extern LPTSTR STRING_STATUSBAR_EMUSAVE;
extern LPTSTR STRING_STATUSBAR_FILESAVE;
extern LPTSTR STRING_FILEERROR_SAVE;
extern LPTSTR STRING_FILEERROR_SAVEAS;
extern LPTSTR STRING_FILEERROR_FILEFORMAT;
extern LPTSTR STRING_FILEERROR_NOTFOUND;
extern LPTSTR STRING_CONFIRM_SAVE;
extern LPTSTR STRING_CONFIRM_EXIT;
extern LPTSTR STRING_CONFIRM_RELOAD;
extern LPTSTR STRING_CONFIRM_DEMORECORD;
extern LPTSTR STRING_CONFIRM_DEMORECORD2;
extern LPTSTR STRING_CONFIRM_UPDATEWORLD;
extern LPTSTR STRING_OPTIONDIALOG_TITLE;
extern LPTSTR STRING_OPTIONDIALOG_EMULATOR;
extern LPTSTR STRING_OPTIONDIALOG_EDITOR;
extern LPTSTR STRING_OPTIONDIALOG_APPLICATION;
extern LPTSTR STRING_OPTIONDIALOG_OBJECTVIEW;
extern LPTSTR STRING_TOOLTIP_OPEN;
extern LPTSTR STRING_TOOLTIP_SAVE;
extern LPTSTR STRING_TOOLTIP_ROOM;
extern LPTSTR STRING_TOOLTIP_BADGUYS;
extern LPTSTR STRING_TOOLTIP_MAP;
extern LPTSTR STRING_TOOLTIP_TESTPLAY;
extern LPTSTR STRING_TOOLTIP_PAGETESTPLAY;
extern LPTSTR STRING_TOOLTIP_HALFPOINTTESTPLAY;
extern LPTSTR STRING_TOOLTIP_STOP;
extern LPTSTR STRING_TOOLTIP_TESTPLAYSETTING;
extern LPTSTR STRING_SENDOBJECT_SRCERROR;
extern LPTSTR STRING_SENDOBJECT_DSTERROR;
extern LPTSTR STRING_SENDOBJECT_OVEROBJ;
extern LPTSTR STRING_UNDO_MENUTEXT;
extern LPTSTR STRING_UNDONAME_KEYEDIT;
extern LPTSTR STRING_UNDONAME_DLGEDIT;
extern LPTSTR STRING_UNDONAME_SENDOBJ;
extern LPTSTR STRING_UNDONAME_HEADDLG;
extern LPTSTR STRING_UNDONAME_CHRLOAD;
extern LPTSTR STRING_UNDONAME_TOOLSTR;
extern LPTSTR STRING_UNDONAME_TOOLLOOPBIN;
extern LPTSTR STRING_UNDONAME_TOOLWORLD;
extern LPTSTR STRING_UNDONAME_TOOLAREAROOM;
extern LPTSTR STRING_UNDONAME_TOOLDEMORECORDER;
extern LPTSTR STRING_UNDONAME_TOOLOTHER;
extern LPTSTR STRING_POLEGFX_DEFAULT;
extern LPTSTR STRING_POLEGFX_ROPE;
extern LPTSTR STRING_POLEGFX_TREE;
extern LPTSTR STRING_POLEGFX_OTHER;
extern LPTSTR STRING_SETTING_WORLDX;
extern LPTSTR STRING_SETTING_WARPZONE;
extern LPTSTR STRING_SETTING_1UP;
extern LPTSTR STRING_SETTING_KOOPA;
extern LPTSTR STRING_SETTING_WORLD;
extern LPTSTR STRING_SETTING_OTHER;
extern LPTSTR STRING_SETTING_TITLE;
extern LPTSTR STRING_STRINGDATA_01;
extern LPTSTR STRING_STRINGDATA_02;
extern LPTSTR STRING_STRINGDATA_03;
extern LPTSTR STRING_STRINGDATA_04;
extern LPTSTR STRING_STRINGDATA_05;
extern LPTSTR STRING_STRINGDATA_06;
extern LPTSTR STRING_STRINGDATA_07;
extern LPTSTR STRING_STRINGDATA_08;
extern LPTSTR STRING_STRINGDATA_09;
extern LPTSTR STRING_STRINGDATA_10;
extern LPTSTR STRING_STRINGDATA_11;
extern LPTSTR STRING_STRINGDATA_12;
extern LPTSTR STRING_STRINGDATA_13;
extern LPTSTR STRING_STRINGDATA_14;
extern LPTSTR STRING_STRINGDATA_15;
extern LPTSTR STRING_STRINGDATA_16;
extern LPTSTR STRING_STRINGDATA_17;
extern LPTSTR STRING_STRINGDATA_18;
extern LPTSTR STRING_STRINGDATA_19;
extern LPTSTR STRING_STRINGDATA_20;
extern LPTSTR STRING_TESTPLAYSETTING_MARIO;
extern LPTSTR STRING_TESTPLAYSETTING_SUPERMARIO;
extern LPTSTR STRING_TESTPLAYSETTING_FIREMARIO;
extern LPTSTR STRING_TESTPLAYSETTING_NONE;
extern LPTSTR STRING_TESTPLAYSETTING_POS;
extern LPTSTR STRING_TESTPLAYSETTING_DOT;
extern LPTSTR STRING_EMULATOROPTION_JOYSTICK;
extern LPTSTR STRING_KEYCONFIG_EMU_A;
extern LPTSTR STRING_KEYCONFIG_EMU_B;
extern LPTSTR STRING_KEYCONFIG_EMU_SELECT;
extern LPTSTR STRING_KEYCONFIG_EMU_START;
extern LPTSTR STRING_KEYCONFIG_EMU_UP;
extern LPTSTR STRING_KEYCONFIG_EMU_DOWN;
extern LPTSTR STRING_KEYCONFIG_EMU_LEFT;
extern LPTSTR STRING_KEYCONFIG_EMU_RIGHT;
extern LPTSTR STRING_KEYCONFIG_EDIT_ADD1;
extern LPTSTR STRING_KEYCONFIG_EDIT_DEC1;
extern LPTSTR STRING_KEYCONFIG_EDIT_ADD16;
extern LPTSTR STRING_KEYCONFIG_EDIT_DEC16;
extern LPTSTR STRING_KEYCONFIG_EDIT_UP;
extern LPTSTR STRING_KEYCONFIG_EDIT_DOWN;
extern LPTSTR STRING_KEYCONFIG_EDIT_LEFT;
extern LPTSTR STRING_KEYCONFIG_EDIT_RIGHT;
extern LPTSTR STRING_KEYCONFIG_EDIT_NEXTPAGE;
extern LPTSTR STRING_KEYCONFIG_EDIT_PREVPAGE;
extern LPTSTR STRING_KEYCONFIG_EDIT_NEXTOBJ;
extern LPTSTR STRING_KEYCONFIG_EDIT_PREVOBJ;
extern LPTSTR STRING_KEYCONFIG_EDIT_SAVEFILE;
extern LPTSTR STRING_KEYCONFIG_EDIT_UNDO;
extern LPTSTR STRING_KEYCONFIG_EDIT_ROOM;
extern LPTSTR STRING_KEYCONFIG_EDIT_BADGUYS;
extern LPTSTR STRING_KEYCONFIG_EDIT_MAP;
extern LPTSTR STRING_KEYCONFIG_EDIT_TESTPLAY;
extern LPTSTR STRING_KEYCONFIG_EDIT_PAGETESTPLAY;
extern LPTSTR STRING_KEYCONFIG_EDIT_PAGETESTPLAY2;
extern LPTSTR STRING_KEYCONFIG_EDIT_STOP;
extern LPTSTR STRING_KEYCONFIG_EDIT_SAVEEMU;
extern LPTSTR STRING_KEYCONFIG_EDIT_LOADEMU;
extern LPTSTR STRING_KEYCONFIG_EDIT_EMUSETTING;
extern LPTSTR STRING_KEYCONFIG_EDIT_NEXTWIN;
extern LPTSTR STRING_KEYCONFIG_EDIT_PREVWIN;
extern LPTSTR STRING_KEYCONFIG_KEYACCEL;
extern LPTSTR STRING_KEYCONFIG_EMULATOR;
extern LPTSTR STRING_KEYCONFIG_OVERLAPPED;
extern LPTSTR STRING_KEYCONFIG_INITIALIZE;
extern LPTSTR STRING_KEYCONFIG_NOTIFY;
extern LPTSTR STRING_KEYCONFIG_PRESET_DESKTOP;
extern LPTSTR STRING_KEYCONFIG_PRESET_NOTE;
extern LPTSTR STRING_KEYCONFIG_WHEEL_NONE;
extern LPTSTR STRING_KEYCONFIG_WHEEL_SHIFT;
extern LPTSTR STRING_KEYCONFIG_WHEEL_CTRL;
extern LPTSTR STRING_KEYCONFIG_WHEEL_SHIFTCTRL;
extern LPTSTR STRING_KEYCONFIG_WHEEL_TYPE1;
extern LPTSTR STRING_KEYCONFIG_WHEEL_TYPE16;
extern LPTSTR STRING_KEYCONFIG_WHEEL_PAGE;
extern LPTSTR STRING_KEYCONFIG_WHEEL_OBJ;
extern LPTSTR STRING_KEYCONFIG_BUTTONX;
extern LPTSTR STRING_OBJLIST_LENNAME;
extern LPTSTR STRING_OBJLIST_UNKNOWN;
extern LPTSTR STRING_OBJLIST_CRASH;
extern LPTSTR STRING_OBJLIST_BACK;
extern LPTSTR STRING_OBJLIST_VIEWBLOCK;
extern LPTSTR STRING_OBJLIST_ROPE;
extern LPTSTR STRING_OBJLIST_LENCASTLE;
extern LPTSTR STRING_OBJLIST_STEP;
extern LPTSTR STRING_OBJLIST_STEP98;
extern LPTSTR STRING_OBJLIST_STEP98_2;
extern LPTSTR STRING_OBJLIST_NONE;
extern LPTSTR STRING_OBJLIST_ROOM;
extern LPTSTR STRING_OBJLIST_PAGECOMMAND;
extern LPTSTR STRING_OBJLIST_HARD;
extern LPTSTR STRING_AREASORT_TITLE;
extern LPTSTR STRING_SEA;
extern LPTSTR STRING_SKY;
extern LPTSTR STRING_UNDERGROUND;
extern LPTSTR STRING_CASTLE;
extern LPTSTR STRING_OBJLIST_COLUMN_BIN;
extern LPTSTR STRING_OBJLIST_COLUMN_PAGE;
extern LPTSTR STRING_OBJLIST_COLUMN_POS;
extern LPTSTR STRING_OBJLIST_COLUMN_TYPE;
extern LPTSTR STRING_OBJDATA_MAP_B_01;
extern LPTSTR STRING_OBJDATA_MAP_B_02;
extern LPTSTR STRING_OBJDATA_MAP_B_03;
extern LPTSTR STRING_OBJDATA_MAP_B_04;
extern LPTSTR STRING_OBJDATA_MAP_B_05;
extern LPTSTR STRING_OBJDATA_MAP_B_06;
extern LPTSTR STRING_OBJDATA_MAP_B_07;
extern LPTSTR STRING_OBJDATA_MAP_B_08;
extern LPTSTR STRING_OBJDATA_MAP_B_09;
extern LPTSTR STRING_OBJDATA_MAP_B_10;
extern LPTSTR STRING_OBJDATA_MAP_B_11;
extern LPTSTR STRING_OBJDATA_MAP_B_12;
extern LPTSTR STRING_OBJDATA_MAP_B_13;
extern LPTSTR STRING_OBJDATA_MAP_B_14;
extern LPTSTR STRING_OBJDATA_MAP_B_15;
extern LPTSTR STRING_OBJDATA_MAP_B_16;
extern LPTSTR STRING_OBJDATA_MAP_B_17;
extern LPTSTR STRING_OBJDATA_MAP_B_18;
extern LPTSTR STRING_OBJDATA_MAP_B_19;
extern LPTSTR STRING_OBJDATA_MAP_B_20;
extern LPTSTR STRING_OBJDATA_MAP_B_21;
extern LPTSTR STRING_OBJDATA_MAP_B_22;
extern LPTSTR STRING_OBJDATA_MAP_B_23;
extern LPTSTR STRING_OBJDATA_MAP_B_24;
extern LPTSTR STRING_OBJDATA_HELP_B12;
extern LPTSTR STRING_OBJDATA_HELP_B13;
extern LPTSTR STRING_OBJDATA_HELP_B14;
extern LPTSTR STRING_OBJDATA_HELP_B23;
extern LPTSTR STRING_OBJDATA_MAP_C_01;
extern LPTSTR STRING_OBJDATA_MAP_C_02;
extern LPTSTR STRING_OBJDATA_MAP_C_03;
extern LPTSTR STRING_OBJDATA_MAP_C_04;
extern LPTSTR STRING_OBJDATA_MAP_C_05;
extern LPTSTR STRING_OBJDATA_MAP_C_06;
extern LPTSTR STRING_OBJDATA_MAP_C_07;
extern LPTSTR STRING_OBJDATA_MAP_C_08;
extern LPTSTR STRING_OBJDATA_HELP_C01;
extern LPTSTR STRING_OBJDATA_HELP_C02;
extern LPTSTR STRING_OBJDATA_HELP_C06;
extern LPTSTR STRING_OBJDATA_MAP_D_01;
extern LPTSTR STRING_OBJDATA_MAP_D_02;
extern LPTSTR STRING_OBJDATA_MAP_D_03;
extern LPTSTR STRING_OBJDATA_MAP_D_04;
extern LPTSTR STRING_OBJDATA_MAP_D_05;
extern LPTSTR STRING_OBJDATA_MAP_D_06;
extern LPTSTR STRING_OBJDATA_MAP_D_07;
extern LPTSTR STRING_OBJDATA_MAP_D_08;
extern LPTSTR STRING_OBJDATA_MAP_D_10;
extern LPTSTR STRING_OBJDATA_MAP_D_11;
extern LPTSTR STRING_OBJDATA_MAP_D_12;
extern LPTSTR STRING_OBJDATA_MAP_D_13;
extern LPTSTR STRING_OBJDATA_MAP_D_14;
extern LPTSTR STRING_OBJDATA_MAP_D_15;
extern LPTSTR STRING_OBJDATA_HELP_D01;
extern LPTSTR STRING_OBJDATA_HELP_D03;
extern LPTSTR STRING_OBJDATA_HELP_D04;
extern LPTSTR STRING_OBJDATA_HELP_D05;
extern LPTSTR STRING_OBJDATA_HELP_D06;
extern LPTSTR STRING_OBJDATA_HELP_D12;
extern LPTSTR STRING_OBJDATA_HELP_D13;
extern LPTSTR STRING_OBJDATA_MAP_E_01;
extern LPTSTR STRING_OBJDATA_MAP_E_02;
extern LPTSTR STRING_OBJDATA_MAP_F_01;
extern LPTSTR STRING_OBJDATA_MAP_F_02;
extern LPTSTR STRING_OBJDATA_MAP_F_03;
extern LPTSTR STRING_OBJDATA_MAP_F_04;
extern LPTSTR STRING_OBJDATA_MAP_F_05;
extern LPTSTR STRING_OBJDATA_MAP_F_06;
extern LPTSTR STRING_OBJDATA_MAP_F_07;
extern LPTSTR STRING_OBJDATA_MAP_F_08;
extern LPTSTR STRING_OBJDATA_HELP_F01;
extern LPTSTR STRING_OBJDATA_HELP_F02;
extern LPTSTR STRING_OBJDATA_HELP_F03;
extern LPTSTR STRING_OBJDATA_HELP_F05;
extern LPTSTR STRING_OBJDATA_HELP_F07;
extern LPTSTR STRING_OBJDATA_BB_01;
extern LPTSTR STRING_OBJDATA_BB_02;
extern LPTSTR STRING_OBJDATA_BB_03;
extern LPTSTR STRING_OBJDATA_BB_04;
extern LPTSTR STRING_OBJDATA_BB_05;
extern LPTSTR STRING_OBJDATA_BB_06;
extern LPTSTR STRING_OBJDATA_BB_07;
extern LPTSTR STRING_OBJDATA_BB_08;
extern LPTSTR STRING_OBJDATA_BB_09;
extern LPTSTR STRING_OBJDATA_BB_10;
extern LPTSTR STRING_OBJDATA_BB_11;
extern LPTSTR STRING_OBJDATA_BB_12;
extern LPTSTR STRING_OBJDATA_BB_13;
extern LPTSTR STRING_OBJDATA_BB_14;
extern LPTSTR STRING_OBJDATA_BB_15;
extern LPTSTR STRING_OBJDATA_BB_16;
extern LPTSTR STRING_OBJDATA_T_01;
extern LPTSTR STRING_OBJDATA_T_02;
extern LPTSTR STRING_OBJDATA_T_03;
extern LPTSTR STRING_OBJDATA_T_04;
extern LPTSTR STRING_OBJDATA_P_01;
extern LPTSTR STRING_OBJDATA_P_02;
extern LPTSTR STRING_OBJDATA_P_03;
extern LPTSTR STRING_OBJDATA_P_04;
extern LPTSTR STRING_OBJDATA_P_07;
extern LPTSTR STRING_OBJDATA_BC_01;
extern LPTSTR STRING_OBJDATA_BC_02;
extern LPTSTR STRING_OBJDATA_BC_03;
extern LPTSTR STRING_OBJDATA_BC_04;
extern LPTSTR STRING_OBJDATA_BC_05;
extern LPTSTR STRING_OBJDATA_BC_06;
extern LPTSTR STRING_OBJDATA_BC_07;
extern LPTSTR STRING_OBJDATA_BC_08;
extern LPTSTR STRING_OBJDATA_MT_01;
extern LPTSTR STRING_OBJDATA_MT_02;
extern LPTSTR STRING_OBJDATA_MT_03;
extern LPTSTR STRING_OBJDATA_MT_04;
extern LPTSTR STRING_OBJDATA_V_01;
extern LPTSTR STRING_OBJDATA_V_02;
extern LPTSTR STRING_OBJDATA_V_03;
extern LPTSTR STRING_OBJDATA_V_04;
extern LPTSTR STRING_OBJDATA_BADGUYS_01;
extern LPTSTR STRING_OBJDATA_BADGUYS_02;
extern LPTSTR STRING_OBJDATA_BADGUYS_03;
extern LPTSTR STRING_OBJDATA_BADGUYS_04;
extern LPTSTR STRING_OBJDATA_BADGUYS_05;
extern LPTSTR STRING_OBJDATA_BADGUYS_06;
extern LPTSTR STRING_OBJDATA_BADGUYS_07;
extern LPTSTR STRING_OBJDATA_BADGUYS_08;
extern LPTSTR STRING_OBJDATA_BADGUYS_09;
extern LPTSTR STRING_OBJDATA_BADGUYS_10;
extern LPTSTR STRING_OBJDATA_BADGUYS_11;
extern LPTSTR STRING_OBJDATA_BADGUYS_12;
extern LPTSTR STRING_OBJDATA_BADGUYS_13;
extern LPTSTR STRING_OBJDATA_BADGUYS_14;
extern LPTSTR STRING_OBJDATA_BADGUYS_15;
extern LPTSTR STRING_OBJDATA_BADGUYS_16;
extern LPTSTR STRING_OBJDATA_BADGUYS_17;
extern LPTSTR STRING_OBJDATA_BADGUYS_18;
extern LPTSTR STRING_OBJDATA_BADGUYS_19;
extern LPTSTR STRING_OBJDATA_BADGUYS_20;
extern LPTSTR STRING_OBJDATA_BADGUYS_21;
extern LPTSTR STRING_OBJDATA_BADGUYS_22;
extern LPTSTR STRING_OBJDATA_BADGUYS_23;
extern LPTSTR STRING_OBJDATA_BADGUYS_24;
extern LPTSTR STRING_OBJDATA_BADGUYS_25;
extern LPTSTR STRING_OBJDATA_BADGUYS_28;
extern LPTSTR STRING_OBJDATA_BADGUYS_29;
extern LPTSTR STRING_OBJDATA_BADGUYS_30;
extern LPTSTR STRING_OBJDATA_BADGUYS_31;
extern LPTSTR STRING_OBJDATA_BADGUYS_32;
extern LPTSTR STRING_OBJDATA_BADGUYS_33;
extern LPTSTR STRING_OBJDATA_BADGUYS_37;
extern LPTSTR STRING_OBJDATA_BADGUYS_38;
extern LPTSTR STRING_OBJDATA_BADGUYS_39;
extern LPTSTR STRING_OBJDATA_BADGUYS_40;
extern LPTSTR STRING_OBJDATA_BADGUYS_41;
extern LPTSTR STRING_OBJDATA_BADGUYS_42;
extern LPTSTR STRING_OBJDATA_BADGUYS_43;
extern LPTSTR STRING_OBJDATA_BADGUYS_44;
extern LPTSTR STRING_OBJDATA_BADGUYS_45;
extern LPTSTR STRING_OBJDATA_BADGUYS_46;
extern LPTSTR STRING_OBJDATA_BADGUYS_47;
extern LPTSTR STRING_OBJDATA_BADGUYS_48;
extern LPTSTR STRING_OBJDATA_BADGUYS_51;
extern LPTSTR STRING_OBJDATA_BADGUYS_53;
extern LPTSTR STRING_OBJDATA_BADGUYS_54;
extern LPTSTR STRING_OBJDATA_BADGUYS_56;
extern LPTSTR STRING_OBJDATA_BADGUYS_57;
extern LPTSTR STRING_OBJDATA_BADGUYS_58;
extern LPTSTR STRING_OBJDATA_BADGUYS_59;
extern LPTSTR STRING_OBJDATA_BADGUYS_60;
extern LPTSTR STRING_OBJDATA_BADGUYS_61;
extern LPTSTR STRING_OBJDATA_BADGUYS_62;
extern LPTSTR STRING_OBJDATA_BADGUYS_63;
extern LPTSTR STRING_DELREG_TITLE;
extern LPTSTR STRING_DELREG_CONFIRM;
extern LPTSTR STRING_DELREG_COMPLETE;
extern LPTSTR STRING_DELREG_ERROR_01;

#endif /* STRING_H */
