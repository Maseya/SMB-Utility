﻿/************************************************************************************

                                  smb Utility

  File: string.h
  Description: strings for international version
  History:
  Contributors: Chezzman1, Insectduel, MirracleMXX, Yy

 ************************************************************************************/
#ifndef STRING_J_H
#define STRING_J_H

#include <tchar.h>

#define PROGRAMNAME                         __T("SMB Utility")

#define STRING_VERSION_CONTRIBUTION         __T("Multi-6502 CPU emulator by Neil Bradley (neil@synthcom.com)")

#ifndef STRING_EMPTY
#define STRING_EMPTY                         __T("")
#endif
#define STRING_UNKNOWN                      __T("Unknown")

#define STRING_LOGVIEW_FILESAVE             __T("Saved as %s.")//__T("%sに保存しました。")
#define STRING_LOGVIEW_FILEOPEN             __T("Loaded %s.")//__T("%sを開きました。")
#define STRING_LOGVIEW_LOADCHR              __T("Imported CHR-ROM from %s.(%.4xH bytes)")//__T("%sから%.4xHﾊﾞｲﾄのCHRROMを読み込みました。")

 //
 //
#define STRING_FILEOPENDIALOG_CHRLOAD       __T("Import CHR-ROM from file")
#define STRING_WINDOW_EMULATOR              __T("Emulator")
#define STRING_WINDOW_OBJLIST               __T("Object List")
#define STRING_WINDOW_OBJVIEW               __T("Object View")

//
#define STRING_OBJVIEW_TITLE                __T("%s <%dpage>")
#define STRING_OBJVIEW_TITLE2               __T("%s <%d-%dpage>")
#define STRING_OBJVIEW_TOOLTIP              __T("%d,(%d,%d) %s")

//
#define STRING_STATUSBAR_EMULOAD            __T("Loaded state.")
#define STRING_STATUSBAR_EMUSAVE            __T("Saved state.")
#define STRING_STATUSBAR_FILESAVE           __T("Saved file.")

//
#define STRING_FILEERROR_SAVE               __T("Failed to save.")
#define STRING_FILEERROR_SAVEAS             __T("Failed to save as.")
#define STRING_FILEERROR_FILEFORMAT         __T("Found file format(file type) error.")
#define STRING_FILEERROR_NOTFOUND           __T("Not found file.")

//
#define STRING_CONFIRM_SAVE                 __T("Overwrite file?")//__T("上書き保存しますか？")
#define STRING_CONFIRM_EXIT                 __T("Save and exit?")
#define STRING_CONFIRM_RELOAD               __T("File has been changed by another editor.\nReload?")
#define STRING_CONFIRM_DEMORECORD           __T("Start recording demonstration?")
#define STRING_CONFIRM_DEMORECORD2          __T("Stoped recording demonstration.")
#define STRING_CONFIRM_UPDATEWORLD          __T("Refresh world data?")

//
#define STRING_OPTIONDIALOG_TITLE           __T("Option")
#define STRING_OPTIONDIALOG_EMULATOR        __T("Emulator")
#define STRING_OPTIONDIALOG_EDITOR          __T("Editor")
#define STRING_OPTIONDIALOG_APPLICATION     __T("Application")
#define STRING_OPTIONDIALOG_OBJECTVIEW      __T("Object View")

//
#define STRING_TOOLTIP_OPEN                 __T("Open")
#define STRING_TOOLTIP_SAVE                 __T("Save")
#define STRING_TOOLTIP_ROOM                 __T("Choose Room")
#define STRING_TOOLTIP_BADGUYS              __T("Badguys Mode")
#define STRING_TOOLTIP_MAP                  __T("Map Mode")
#define STRING_TOOLTIP_TESTPLAY             __T("Start of Room")
#define STRING_TOOLTIP_PAGETESTPLAY         __T("Current Page")
#define STRING_TOOLTIP_HALFPOINTTESTPLAY    __T("Halfway Point")
#define STRING_TOOLTIP_STOP                 __T("End")
#define STRING_TOOLTIP_TESTPLAYSETTING      __T("Test Configuration")

//
#define STRING_SENDOBJECT_SRCERROR          __T("Can't send source object.")
#define STRING_SENDOBJECT_DSTERROR          __T("There are no objects in destination page.")
#define STRING_SENDOBJECT_OVEROBJ           __T("Can't send any more objects to the destination room.")//__T("送り先のﾙｰﾑには、これ以上ｵﾌﾞｼﾞｪｸﾄを送ることができません。")

#define STRING_UNDO_MENUTEXT                __T("&Undo \"%s\"")
#define STRING_UNDONAME_KEYEDIT             __T("Keyboard Input")
#define STRING_UNDONAME_DLGEDIT             __T("Dialog Input")
#define STRING_UNDONAME_SENDOBJ             __T("Send Object")
#define STRING_UNDONAME_HEADDLG             __T("Edit Header")
#define STRING_UNDONAME_CHRLOAD             __T("Import CHR-ROM")
#define STRING_UNDONAME_TOOLSTR             __T("Edit Text")
#define STRING_UNDONAME_TOOLLOOPBIN         __T("Edit Looping")
#define STRING_UNDONAME_TOOLWORLD           __T("World Data Update")//__T("Auto Refresh World Data")
#define STRING_UNDONAME_TOOLAREAROOM        __T("Sort Areas")
#define STRING_UNDONAME_TOOLDEMORECORDER    __T("Record Intro Demo")//__T("Demo Recording")
#define STRING_UNDONAME_TOOLOTHER           __T("Edit Some Game Stuff")

//#define STRING_UNDONAME_TOOLGLOBAL        __T("General Setting")

#define STRING_POLEGFX_DEFAULT              __T("Default")
#define STRING_POLEGFX_ROPE                 __T("Rope")
#define STRING_POLEGFX_TREE                 __T("Tree")
#define STRING_POLEGFX_OTHER                __T("Other")

#define STRING_SETTING_WORLDX               __T("World%d")
#define STRING_SETTING_WARPZONE             __T("Warp Zone")
#define STRING_SETTING_1UP                  __T("1UP Mushroom")
#define STRING_SETTING_KOOPA                __T("Bowser")
#define STRING_SETTING_WORLD                __T("World")
#define STRING_SETTING_OTHER                __T("Other")
#define STRING_SETTING_TITLE                __T("Edit Some Game Stuff")

#define STRING_STRINGDATA_01                __T("MARIO(Status bar)")
#define STRING_STRINGDATA_02                __T("MARIO(Time up at two players mode)")
#define STRING_STRINGDATA_03                __T("MARIO(Game over)")
#define STRING_STRINGDATA_04                __T("LUIGI(Status bar/Time up/Game over)")
#define STRING_STRINGDATA_05                __T("WORLD  TIME(Status Bar)")
#define STRING_STRINGDATA_06                __T("WORLD(On the World 1-1 ect screen)")
#define STRING_STRINGDATA_07                __T("TIME UP")
#define STRING_STRINGDATA_08                __T("GAME OVER")
#define STRING_STRINGDATA_09                __T("WELCOME TO WARP ZONE!")
#define STRING_STRINGDATA_10                __T("THANK YOU MARIO!")
#define STRING_STRINGDATA_11                __T("THANK YOU LUIGI!")
#define STRING_STRINGDATA_12                __T("BUT OUR PRINCESS IS IN")
#define STRING_STRINGDATA_13                __T("ANOTHER CASTLE!")
#define STRING_STRINGDATA_14                __T("YOUR QUEST IS OVER.")
#define STRING_STRINGDATA_15                __T("WE PRESENT YOU A NEW QUEST.")
#define STRING_STRINGDATA_16                __T("PUSH BUTTON B")
#define STRING_STRINGDATA_17                __T("TO SELECT A WORLD")
#define STRING_STRINGDATA_18                __T("Copyright notice(Title screen)")
#define STRING_STRINGDATA_19                __T("1 PLAYER GAME(Title screen)")
#define STRING_STRINGDATA_20                __T("2 PLAYER GAME(Title screen)")

#define STRING_TESTPLAYSETTING_MARIO        __T("Regular Mario")
#define STRING_TESTPLAYSETTING_SUPERMARIO   __T("Super Mario")
#define STRING_TESTPLAYSETTING_FIREMARIO    __T("Fiery Mario")
#define STRING_TESTPLAYSETTING_NONE         __T("Default") //__T("無効")
#define STRING_TESTPLAYSETTING_POS          __T("Position")//__T("位置")
#define STRING_TESTPLAYSETTING_DOT          __T("Pixel")//__T("ﾄﾞｯﾄ")

#define STRING_EMULATOROPTION_JOYSTICK      __T("Pad")//__T("ｼﾞｮｲｽﾃｨｯｸ")

#define STRING_KEYCONFIG_EMU_A              __T("A")
#define STRING_KEYCONFIG_EMU_B              __T("B")
#define STRING_KEYCONFIG_EMU_SELECT         __T("Select")
#define STRING_KEYCONFIG_EMU_START          __T("Start")
#define STRING_KEYCONFIG_EMU_UP             __T("Up")
#define STRING_KEYCONFIG_EMU_DOWN           __T("Down")
#define STRING_KEYCONFIG_EMU_LEFT           __T("Left")
#define STRING_KEYCONFIG_EMU_RIGHT          __T("Right")
#define STRING_KEYCONFIG_EDIT_ADD1          __T("Object value +1")
#define STRING_KEYCONFIG_EDIT_DEC1          __T("Object value -1")
#define STRING_KEYCONFIG_EDIT_ADD16         __T("Object value +16")
#define STRING_KEYCONFIG_EDIT_DEC16         __T("Object value -16")
#define STRING_KEYCONFIG_EDIT_UP            __T("Object position to up")
#define STRING_KEYCONFIG_EDIT_DOWN          __T("Object position to down")
#define STRING_KEYCONFIG_EDIT_LEFT          __T("Object position to left")
#define STRING_KEYCONFIG_EDIT_RIGHT         __T("Object position to right")
#define STRING_KEYCONFIG_EDIT_NEXTPAGE      __T("Next page")
#define STRING_KEYCONFIG_EDIT_PREVPAGE      __T("Previous page")
#define STRING_KEYCONFIG_EDIT_NEXTOBJ       __T("Next object")
#define STRING_KEYCONFIG_EDIT_PREVOBJ       __T("Previous object")
#define STRING_KEYCONFIG_EDIT_SAVEFILE      __T("File:Save")
#define STRING_KEYCONFIG_EDIT_UNDO          __T("Edit:Undo")
#define STRING_KEYCONFIG_EDIT_ROOM          __T("Edit:Choose Room")
#define STRING_KEYCONFIG_EDIT_BADGUYS       __T("Edit:Badguys Mode")
#define STRING_KEYCONFIG_EDIT_MAP           __T("Edit:Map Mode")
#define STRING_KEYCONFIG_EDIT_TESTPLAY      __T("Emulator:Title Screen")
#define STRING_KEYCONFIG_EDIT_PAGETESTPLAY  __T("Emulator:Current Page")
#define STRING_KEYCONFIG_EDIT_PAGETESTPLAY2 __T("Emulator:Halfway Point")
#define STRING_KEYCONFIG_EDIT_STOP          __T("Emulator:End")
#define STRING_KEYCONFIG_EDIT_SAVEEMU       __T("Emulator:Save")
#define STRING_KEYCONFIG_EDIT_LOADEMU       __T("Emulator:Load")
#define STRING_KEYCONFIG_EDIT_EMUSETTING    __T("Emulator:Test Configuration")
#define STRING_KEYCONFIG_EDIT_NEXTWIN       __T("Window:Next")
#define STRING_KEYCONFIG_EDIT_PREVWIN       __T("Window:Previous")
#define STRING_KEYCONFIG_KEYACCEL           __T("Keyboard accelerator")
#define STRING_KEYCONFIG_EMULATOR           __T("Emulator")
#define STRING_KEYCONFIG_OVERLAPPED         __T("It is overlapped at %s - \"%s\".")
#define STRING_KEYCONFIG_INITIALIZE         __T("Load preset-key?")
#define STRING_KEYCONFIG_NOTIFY             __T("It is enabled at next start.")

#define STRING_KEYCONFIG_PRESET_DESKTOP     __T("Desktop PC(Ten key)")
#define STRING_KEYCONFIG_PRESET_NOTE        __T("Note PC")

#define STRING_KEYCONFIG_WHEEL_NONE         __T("None")//__T("なし")
#define STRING_KEYCONFIG_WHEEL_SHIFT        __T("+Shift")//__T("+ｼﾌﾄ")
#define STRING_KEYCONFIG_WHEEL_CTRL         __T("+Ctrl")//__T("+ｺﾝﾄﾛｰﾙ")
#define STRING_KEYCONFIG_WHEEL_SHIFTCTRL    __T("+Shift+Ctrl")//__T("+ｼﾌﾄ+ｺﾝﾄﾛｰﾙ")
#define STRING_KEYCONFIG_WHEEL_TYPE1        __T("Change object value by 1")//__T("種類を増減1")
#define STRING_KEYCONFIG_WHEEL_TYPE16       __T("Change object value by 16")//__T("種類を増減16")
#define STRING_KEYCONFIG_WHEEL_PAGE         __T("Select object on next page")//__T("ﾍﾟｰｼﾞごとの移動")
#define STRING_KEYCONFIG_WHEEL_OBJ          __T("Select object")//__T("ｵﾌﾞｼﾞｪｸﾄごとの移動")

#define STRING_KEYCONFIG_BUTTONX            __T("Button%d")//__T("ﾎﾞﾀﾝ%d")

#define STRING_OBJLIST_LENNAME              __T("Length:%d, %s")
#define STRING_OBJLIST_UNKNOWN              __T("Unknown object")
#define STRING_OBJLIST_CRASH                __T("*Freezes")
#define STRING_OBJLIST_BACK                 __T("Background:%s")
#define STRING_OBJLIST_VIEWBLOCK            __T("Scene:%s Block:%s")
#define STRING_OBJLIST_ROPE                 __T("Lift's vertical rope")
#define STRING_OBJLIST_LENCASTLE            __T("Castle, Length:%d")
#define STRING_OBJLIST_STEP                 __T("Staircase, Length:%d, Height:%d")
#define STRING_OBJLIST_STEP98               __T("Staircase, Length:9 Height:8")
#define STRING_OBJLIST_STEP98_2             __T("*Staircase, Length:9, Height:8")
#define STRING_OBJLIST_NONE                 __T("*Nothing")

#define STRING_OBJLIST_ROOM                 __T("Room change[%.2xH(%s), w=%d, p=%d]")
#define STRING_OBJLIST_PAGECOMMAND          __T("Page skip:%.2d")
#define STRING_OBJLIST_HARD                 __T(" (Only after 5-3)")

#define STRING_AREASORT_TITLE               __T("Choose Room")

#define STRING_SEA                          __T("Sea")
#define STRING_SKY                          __T("Ground")
#define STRING_UNDERGROUND                  __T("Underground")
#define STRING_CASTLE                       __T("Castle")

#define STRING_OBJLIST_COLUMN_BIN           __T("Hex")
#define STRING_OBJLIST_COLUMN_PAGE          __T("Page")
#define STRING_OBJLIST_COLUMN_POS           __T("Position")
#define STRING_OBJLIST_COLUMN_TYPE          __T("Type")

#define STRING_OBJDATA_MAP_B_01             __T("?Block(Mushroom)")
#define STRING_OBJDATA_MAP_B_02             __T("?Block(Coin)")
#define STRING_OBJDATA_MAP_B_03             __T("Hidden block(Coin)")
#define STRING_OBJDATA_MAP_B_04             __T("Hidden block(1UP mushroom)")
#define STRING_OBJDATA_MAP_B_05             __T("Brick(Mushroom)")
#define STRING_OBJDATA_MAP_B_06             __T("Brick(Vine)")
#define STRING_OBJDATA_MAP_B_07             __T("Brick(Star)")
#define STRING_OBJDATA_MAP_B_08             __T("Brick(10 coins)")
#define STRING_OBJDATA_MAP_B_09             __T("Brick(1UP mushroom)")
#define STRING_OBJDATA_MAP_B_10             __T("Sideways pipe")
#define STRING_OBJDATA_MAP_B_11             __T("Used block")
#define STRING_OBJDATA_MAP_B_12             __T("Jumping board")
#define STRING_OBJDATA_MAP_B_13             __T("Reverse L pipe")
#define STRING_OBJDATA_MAP_B_14             __T("Flagpole")
#define STRING_OBJDATA_MAP_B_15             __T("*Nothing")
#define STRING_OBJDATA_MAP_B_16             __T("*Nothing")
#define STRING_OBJDATA_MAP_B_17             __T("Island(Cannon)")
#define STRING_OBJDATA_MAP_B_18             __T("Horizontal bricks(corals)")
#define STRING_OBJDATA_MAP_B_19             __T("Horizontal blocks")
#define STRING_OBJDATA_MAP_B_20             __T("Horizontal coins")
#define STRING_OBJDATA_MAP_B_21             __T("Vertical bricks(corals)")
#define STRING_OBJDATA_MAP_B_22             __T("Vertical blocks")
#define STRING_OBJDATA_MAP_B_23             __T("Pipe(Player can't enter.)")
#define STRING_OBJDATA_MAP_B_24             __T("Pipe(Player can enter.)")

#define STRING_OBJDATA_HELP_B12             STRING_NULL//__T("ｷｬﾗｸﾀの１つ。ｷｬﾗｸﾀｵｰﾊﾞｰで表示されなくなると、ﾏﾘｵが挟まって出れなくなるので注意。")
#define STRING_OBJDATA_HELP_B13             STRING_NULL//__T("高さ9固定。横からでも上からでも入れる。(横位置＋2)から2ｷｬﾗ分は、先に指定された障害物を消す。")
#define STRING_OBJDATA_HELP_B14             STRING_NULL//__T("やたらと使用しないこと。1ﾙｰﾑに2回以上使用すると暴走しやすくなるので注意。")
#define STRING_OBJDATA_HELP_B23             STRING_NULL//__T("長さを1にしても必ず2になる。")

#define STRING_OBJDATA_MAP_C_01             __T("Hole")
#define STRING_OBJDATA_MAP_C_02             __T("Balance's horizontal rope")
#define STRING_OBJDATA_MAP_C_03             __T("Bridge(V=7)")
#define STRING_OBJDATA_MAP_C_04             __T("Bridge(V=8)")
#define STRING_OBJDATA_MAP_C_05             __T("Bridge(V=10)")
#define STRING_OBJDATA_MAP_C_06             __T("Hole filled with water")
#define STRING_OBJDATA_MAP_C_07             __T("Horizontal ?Blocks(Coin, V=3)")
#define STRING_OBJDATA_MAP_C_08             __T("Horizontal ?Blocks(Coin, V=7)")

#define STRING_OBJDATA_HELP_C01             STRING_NULL//__T("縦位置8から12の先指定したすべてのﾏｯﾌﾟｷｬﾗを消す。")
#define STRING_OBJDATA_HELP_C02             STRING_NULL//__T("縦位置0固定")
#define STRING_OBJDATA_HELP_C06             STRING_NULL//__T("縦位置10から12の先指定したすべてのﾏｯﾌﾟｷｬﾗを消す。")

#define STRING_OBJDATA_MAP_D_01             __T("Page skip")
#define STRING_OBJDATA_MAP_D_02             __T("Reverse L pipe")
#define STRING_OBJDATA_MAP_D_03             __T("Flagpole")
#define STRING_OBJDATA_MAP_D_04             __T("Axe")
#define STRING_OBJDATA_MAP_D_05             __T("Rope")
#define STRING_OBJDATA_MAP_D_06             __T("Bridge")
#define STRING_OBJDATA_MAP_D_07             __T("Scroll stop(warp zone)")
#define STRING_OBJDATA_MAP_D_08             __T("Scroll stop")
#define STRING_OBJDATA_MAP_D_10             __T("Cheep-cheep(red, fly)")
#define STRING_OBJDATA_MAP_D_11             __T("Bullet Bill(Cheep-cheep) continuation")
#define STRING_OBJDATA_MAP_D_12             __T("Stop continuation")
#define STRING_OBJDATA_MAP_D_13             __T("Loop command")
#define STRING_OBJDATA_MAP_D_14             __T("?")
#define STRING_OBJDATA_MAP_D_15             __T("*Freeze")

#define STRING_OBJDATA_HELP_D01             STRING_NULL
#define STRING_OBJDATA_HELP_D03             STRING_NULL//__T("やたらと使用しない事。1ﾙｰﾑに2回以上使用すると暴走しやすくなるので注意。")
#define STRING_OBJDATA_HELP_D04             STRING_NULL//__T("縦位置6固定")
#define STRING_OBJDATA_HELP_D05             STRING_NULL//__T("縦位置7固定")
#define STRING_OBJDATA_HELP_D06             STRING_NULL//__T("縦位置8、長さ13固定")
#define STRING_OBJDATA_HELP_D12             STRING_NULL//__T("連続出現ｷｬﾗを出した後、暴走を防ぐためにﾎﾟｰﾙを出す前に指定。")
#define STRING_OBJDATA_HELP_D13             STRING_NULL//__T("ﾍﾟｰｼﾞ送りｺﾏﾝﾄﾞとｾｯﾄでﾙｰﾌﾟを形成する。")

#define STRING_OBJDATA_MAP_E_01             __T("Basic brick & scenery")
#define STRING_OBJDATA_MAP_E_02             __T("Change background")

#define STRING_OBJDATA_MAP_F_01             __T("Rope of lift")
#define STRING_OBJDATA_MAP_F_02             __T("Vertical rope of balance lift")
#define STRING_OBJDATA_MAP_F_03             __T("Castle")
#define STRING_OBJDATA_MAP_F_04             __T("Staircase(length:n, height:n)")
#define STRING_OBJDATA_MAP_F_05             __T("Staircase(length:9, height:8)")
#define STRING_OBJDATA_MAP_F_06             __T("Long reverse L pipe")
#define STRING_OBJDATA_MAP_F_07             __T("Vertical balls")
#define STRING_OBJDATA_MAP_F_08             __T("Nothing")

#define STRING_OBJDATA_HELP_F01             STRING_NULL//__T("縦位置0、長さ13固定。指定横位置において、先指定したすべてのﾏｯﾌﾟｷｬﾗを消す。")
#define STRING_OBJDATA_HELP_F02             STRING_NULL//__T("縦位置1固定。指定横位置において、先指定したすべてのﾏｯﾌﾟｷｬﾗを消す。")
#define STRING_OBJDATA_HELP_F03             STRING_NULL//__T("やたらと使用しないこと。多く使用すると暴走しやすくなる。")
#define STRING_OBJDATA_HELP_F05             STRING_NULL//__T("長さ0～3 問題なし。\n長さ4～6 ｸﾞﾗﾌｨｯｸｽが潰れる。長さ7 ｸﾞﾗﾌｨｯｸｽが潰れる。(横位置+16)の位置に縦位置7の壊せないﾌﾞﾛｯｸ1個。")
#define STRING_OBJDATA_HELP_F07             STRING_NULL//__T("ﾂﾀのようにつかまって上下移動が可能。縦位置2固定")

#define STRING_OBJDATA_BB_01                __T("Brick:None")
#define STRING_OBJDATA_BB_02                __T("Brick:Surface")
#define STRING_OBJDATA_BB_03                __T("Brick:Surface&ceiling")
#define STRING_OBJDATA_BB_04                __T("Brick:Surface&ceiling(3)")
#define STRING_OBJDATA_BB_05                __T("Brick:Surface&ceiling(4)")
#define STRING_OBJDATA_BB_06                __T("Brick:Surface&ceiling(8)")
#define STRING_OBJDATA_BB_07                __T("Brick:Surface(4)&ceiling")
#define STRING_OBJDATA_BB_08                __T("Brick:Surface(4)&ceiling(3)")
#define STRING_OBJDATA_BB_09                __T("Brick:Surface(4)&ceiling(4)")
#define STRING_OBJDATA_BB_10                __T("Brick:Surface(5)&ceiling")
#define STRING_OBJDATA_BB_11                __T("Brick:Ceiling")
#define STRING_OBJDATA_BB_12                __T("Brick:Surface(5)&ceiling(4)")
#define STRING_OBJDATA_BB_13                __T("Brick:Surface(8)&ceiling")
#define STRING_OBJDATA_BB_14                __T("Brick:Surface&ceiling&middle(5)")
#define STRING_OBJDATA_BB_15                __T("Brick:Surface&ceiling&middle(4)")
#define STRING_OBJDATA_BB_16                __T("Brick:All")

#define STRING_OBJDATA_T_01                 __T("Not set")
#define STRING_OBJDATA_T_02                 __T("400")
#define STRING_OBJDATA_T_03                 __T("300")
#define STRING_OBJDATA_T_04                 __T("200")

#define STRING_OBJDATA_P_01                 __T("V=-1, H=1.5")
#define STRING_OBJDATA_P_02                 __T("Start of area:X, from another room; V=-1, H=1.5")
#define STRING_OBJDATA_P_03                 __T("V=10, H=1.5")
#define STRING_OBJDATA_P_04                 __T("V=4, H=1.5")
#define STRING_OBJDATA_P_07                 __T("V=10, H=1.5(walking)")

#define STRING_OBJDATA_BC_01                __T("Nothing")
#define STRING_OBJDATA_BC_02                __T("In water")
#define STRING_OBJDATA_BC_03                __T("Wall")
#define STRING_OBJDATA_BC_04                __T("Over water")
#define STRING_OBJDATA_BC_05                __T("Night")
#define STRING_OBJDATA_BC_06                __T("Snow")
#define STRING_OBJDATA_BC_07                __T("Night and snow")
#define STRING_OBJDATA_BC_08                __T("Castle")

#define STRING_OBJDATA_MT_01                __T("Green and tree")
#define STRING_OBJDATA_MT_02                __T("Orange and mushroom")
#define STRING_OBJDATA_MT_03                __T("Guns")
#define STRING_OBJDATA_MT_04                __T("Clouds")

#define STRING_OBJDATA_V_01                 __T("Nothing")
#define STRING_OBJDATA_V_02                 __T("Clouds")
#define STRING_OBJDATA_V_03                 __T("Mountain")
#define STRING_OBJDATA_V_04                 __T("Fence")

#define STRING_OBJDATA_BADGUYS_01           __T("Koopa Troopa(green)")
#define STRING_OBJDATA_BADGUYS_02           __T("Koopa Troopa(red)")
#define STRING_OBJDATA_BADGUYS_03           __T("Buzzy Beetle")
#define STRING_OBJDATA_BADGUYS_04           __T("Koopa Troopa(red, patrol)")
#define STRING_OBJDATA_BADGUYS_05           __T("Koopa Troopa(green, stopped)")
#define STRING_OBJDATA_BADGUYS_06           __T("The Hammer Brothers")
#define STRING_OBJDATA_BADGUYS_07           __T("Little Goomba")
#define STRING_OBJDATA_BADGUYS_08           __T("Bloober")
#define STRING_OBJDATA_BADGUYS_09           __T("Bullet Bill")
#define STRING_OBJDATA_BADGUYS_10           __T("Koopa Paratroopa(green, stopped)")
#define STRING_OBJDATA_BADGUYS_11           __T("Cheep-cheep(green)")
#define STRING_OBJDATA_BADGUYS_12           __T("Cheep-cheep(red)")
#define STRING_OBJDATA_BADGUYS_13           __T("Podoboo")
#define STRING_OBJDATA_BADGUYS_14           __T("Pirana plants")
#define STRING_OBJDATA_BADGUYS_15           __T("Koopa Paratroopa(green, leaping)")
#define STRING_OBJDATA_BADGUYS_16           __T("Koopa Paratroopa(red, up&down)")
#define STRING_OBJDATA_BADGUYS_17           __T("Koopa Paratroopa(green, right&left)")
#define STRING_OBJDATA_BADGUYS_18           __T("Lakitu")
#define STRING_OBJDATA_BADGUYS_19           __T("Spiny")
#define STRING_OBJDATA_BADGUYS_20           __T("*Unknown")
#define STRING_OBJDATA_BADGUYS_21           __T("Cheep-cheep(fly)")
#define STRING_OBJDATA_BADGUYS_22           __T("Bowser's fire")
#define STRING_OBJDATA_BADGUYS_23           __T("*Crash")
#define STRING_OBJDATA_BADGUYS_24           __T("Bullet Bill(Cheep-cheep) continuation")
#define STRING_OBJDATA_BADGUYS_25           __T("*None")
#define STRING_OBJDATA_BADGUYS_28           __T("Fire Bar(right)")
#define STRING_OBJDATA_BADGUYS_29           __T("Fire Bar(right, fast)")
#define STRING_OBJDATA_BADGUYS_30           __T("Fire Bar(left)")
#define STRING_OBJDATA_BADGUYS_31           __T("Fire Bar(left, fast)")
#define STRING_OBJDATA_BADGUYS_32           __T("Long Fire Bar(right)")
#define STRING_OBJDATA_BADGUYS_33           __T("Fire Bar")
#define STRING_OBJDATA_BADGUYS_37           __T("Lift(balance)")
#define STRING_OBJDATA_BADGUYS_38           __T("Lift(up&down)")
#define STRING_OBJDATA_BADGUYS_39           __T("Lift(up)")
#define STRING_OBJDATA_BADGUYS_40           __T("Lift(down)")
#define STRING_OBJDATA_BADGUYS_41           __T("Lift(right&left)")
#define STRING_OBJDATA_BADGUYS_42           __T("Lift(fall)")
#define STRING_OBJDATA_BADGUYS_43           __T("Lift(surfing)")
#define STRING_OBJDATA_BADGUYS_44           __T("Short lift(up)")
#define STRING_OBJDATA_BADGUYS_45           __T("Short lift(down)")
#define STRING_OBJDATA_BADGUYS_46           __T("Bowser, King of the Koopa")
#define STRING_OBJDATA_BADGUYS_47           __T("*Unknown")
#define STRING_OBJDATA_BADGUYS_48           __T("*Unknown")
#define STRING_OBJDATA_BADGUYS_51           __T("Jumping board(fake)")
#define STRING_OBJDATA_BADGUYS_53           __T("Warp Zone")
#define STRING_OBJDATA_BADGUYS_54           __T("Mushroom retainers")
#define STRING_OBJDATA_BADGUYS_56           __T("2 Little Goomba(V=10)")
#define STRING_OBJDATA_BADGUYS_57           __T("3 Little Goomba(V=10)")
#define STRING_OBJDATA_BADGUYS_58           __T("2 Little Goomba(V=6)")
#define STRING_OBJDATA_BADGUYS_59           __T("3 Little Goomba(V=6)")
#define STRING_OBJDATA_BADGUYS_60           __T("2 Koopa Troopa(green, V=10)")
#define STRING_OBJDATA_BADGUYS_61           __T("3 Koopa Troopa(green, V=10)")
#define STRING_OBJDATA_BADGUYS_62           __T("2 Koopa Troopa(green, V=6)")
#define STRING_OBJDATA_BADGUYS_63           __T("3 Koopa Troopa(green, V=6)")

// delreg.exe
#define STRING_DELREG_TITLE                 __T("Deleting registry entries of SMB Utility")//ダイアログのタイトル __T("SMB Utility ﾚｼﾞｽﾄﾘの削除")
#define STRING_DELREG_CONFIRM               __T("You are about to delete registry keys that were used in SMB Utility. Are you sure?")//__T("SMB Utilityで使用したﾚｼﾞｽﾄﾘのｷｰを削除しますか？")
#define STRING_DELREG_COMPLETE              __T("Deleted registry keys that were used in SMB Utility.")//__T("SMB Utilityで使用したﾚｼﾞｽﾄﾘのｷｰを削除しました。")
#define STRING_DELREG_ERROR_01              __T("Registry keys that were used in SMB Utility don't exist.")//__T("SMB Utilityのﾚｼﾞｽﾄﾘのｷｰは、存在しません。")

#endif
