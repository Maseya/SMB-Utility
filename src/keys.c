﻿/**********************************************************************

                                  smb Utility

  File: keys.c
  Description:
  History:

 *********************************************************************/
#include "keys.h"

#include "emulator.h"
#include "ini.h"
#include "keyaccel.h"
#include "objeditcom.h"
#include "smbutil.h"

// none, +shift, +ctrl, +shift+ctrl
#define CUSTOMIZE_WHEEL_NUMFUNCS 4

#define CUSTOMIZE_WHEEL_TYPE_NONE 0
#define CUSTOMIZE_WHEEL_TYPE_CTRL 1
#define CUSTOMIZE_WHEEL_TYPE_SHIFT 2
#define CUSTOMIZE_WHEEL_TYPE_CTRLSHIFT 3

WHEELFUNC g_wfWheelFunc[CUSTOMIZE_WHEEL_NUMFUNCS];
const WHEELFUNC g_wfWheelFuncDefault[CUSTOMIZE_WHEEL_NUMFUNCS] = {PAGE, TYPE16, TYPE1,
                                                                  OBJ};

static void ExecuteWheelCommand(WHEELFUNC wf, short zDelta) {
    switch (wf) {
        case TYPE1:
            if (zDelta > 0)
                MapEditCommand(IDM_EDITCOMMAND_ADD1TOTYPE);
            else if (zDelta)
                MapEditCommand(IDM_EDITCOMMAND_DEC1TOTYPE);
            break;
        case TYPE16:
            if (zDelta > 0)
                MapEditCommand(IDM_EDITCOMMAND_ADD16TOTYPE);
            else if (zDelta)
                MapEditCommand(IDM_EDITCOMMAND_DEC16TOTYPE);
            break;
        case PAGE:
            if (zDelta > 0)
                MapEditCommand(IDM_EDITCOMMAND_REWPAGE);
            else if (zDelta)
                MapEditCommand(IDM_EDITCOMMAND_FWDPAGE);
            break;
        case OBJ:
            if (zDelta > 0)
                MapEditCommand(IDM_EDITCOMMAND_PREVOBJ);
            else if (zDelta)
                MapEditCommand(IDM_EDITCOMMAND_NEXTOBJ);
            break;
    }
}

VOID ky_WM_MOUSEWHEEL(WPARAM wParam, LPARAM lParam) {
    DWORD fwKeys = LOWORD(wParam);
    short zDelta = (short)HIWORD(wParam);

    if ((fwKeys & MK_CONTROL) && (fwKeys & MK_SHIFT)) {
        ExecuteWheelCommand(g_wfWheelFunc[CUSTOMIZE_WHEEL_TYPE_CTRLSHIFT], zDelta);
    } else if (fwKeys & MK_CONTROL) {
        ExecuteWheelCommand(g_wfWheelFunc[CUSTOMIZE_WHEEL_TYPE_CTRL], zDelta);
    } else if (fwKeys & MK_SHIFT) {
        ExecuteWheelCommand(g_wfWheelFunc[CUSTOMIZE_WHEEL_TYPE_SHIFT], zDelta);
    } else {
        ExecuteWheelCommand(g_wfWheelFunc[CUSTOMIZE_WHEEL_TYPE_NONE], zDelta);
    }
}

HANDLE ky_Initialize() {
    if (!ReadFromRegistry(INI_EDITOR_WHEEL, REG_BINARY, g_wfWheelFunc,
                          CUSTOMIZE_WHEEL_NUMFUNCS * sizeof(WHEELFUNC))) {
        CopyMemory(g_wfWheelFunc, g_wfWheelFuncDefault,
                   CUSTOMIZE_WHEEL_NUMFUNCS * sizeof(WHEELFUNC));
    }

    return CreateEditCommandAccel();
}

#define NUM_VKEYINFO 83

const struct {
    BYTE bVKey;
    LPTSTR lpKeyName;
} g_VKeyInfo[NUM_VKEYINFO] = {
        VK_BACK,     __T("Back Space"), VK_TAB,      __T("Tab"),
        VK_CLEAR,    __T("Clear"),      VK_RETURN,   __T("Enter"),
        VK_PAUSE,    __T("Pause"),      VK_CAPITAL,  __T("Caps Lock"),
        VK_ESCAPE,   __T("Esc"),        VK_SPACE,    __T("Space"),
        VK_PRIOR,    __T("Page Up"),    VK_NEXT,     __T("Page Down"),

        VK_END,      __T("End"),        VK_HOME,     __T("Home"),
        VK_LEFT,     __T("Left"),       VK_UP,       __T("Up"),
        VK_RIGHT,    __T("Right"),      VK_DOWN,     __T("Down"),
        VK_SELECT,   __T("Select"),     VK_SNAPSHOT, __T("Print Screen"),
        VK_INSERT,   __T("Insert"),     VK_DELETE,   __T("Delete"),

        0x30,        __T("0"),          0x31,        __T("1"),
        0x32,        __T("2"),          0x33,        __T("3"),
        0x34,        __T("4"),          0x35,        __T("5"),
        0x36,        __T("6"),          0x37,        __T("7"),
        0x38,        __T("8"),          0x39,        __T("9"),

        0x41,        __T("a"),          0x42,        __T("b"),
        0x43,        __T("c"),          0x44,        __T("d"),
        0x45,        __T("e"),          0x46,        __T("f"),
        0x47,        __T("g"),          0x48,        __T("h"),
        0x49,        __T("i"),          0x4A,        __T("j"),

        0x4B,        __T("k"),          0x4C,        __T("l"),
        0x4D,        __T("m"),          0x4E,        __T("n"),
        0x4F,        __T("o"),          0x50,        __T("p"),
        0x51,        __T("q"),          0x52,        __T("r"),
        0x53,        __T("s"),          0x54,        __T("t"),

        0x55,        __T("u"),          0x56,        __T("v"),
        0x57,        __T("w"),          0x58,        __T("x"),
        0x59,        __T("y"),          0x5A,        __T("z"),
        VK_NUMPAD0,  __T("Numpad 0"),   VK_NUMPAD1,  __T("Numpad 1"),
        VK_NUMPAD2,  __T("Numpad 2"),   VK_NUMPAD3,  __T("Numpad 3"),

        VK_NUMPAD4,  __T("Numpad 4"),   VK_NUMPAD5,  __T("Numpad 5"),
        VK_NUMPAD6,  __T("Numpad 6"),   VK_NUMPAD7,  __T("Numpad 7"),
        VK_NUMPAD8,  __T("Numpad 8"),   VK_NUMPAD9,  __T("Numpad 9"),
        VK_MULTIPLY, __T("Numpad *"),   VK_ADD,      __T("Numpad +"),
        VK_SUBTRACT, __T("Numpad -"),   VK_DECIMAL,  __T("Numpad ."),

        VK_DIVIDE,   __T("Numpad /"),   VK_F1,       __T("f1"),
        VK_F2,       __T("f2"),         VK_F3,       __T("f3"),
        VK_F4,       __T("f4"),         VK_F5,       __T("f5"),
        VK_F6,       __T("f6"),         VK_F7,       __T("f7"),
        VK_F8,       __T("f8"),         VK_F9,       __T("f9"),

        VK_F10,      __T("f10"),        VK_F11,      __T("f11"),
        VK_F12,      __T("f12")};

static UINT FindVkeyIndex(BYTE bVkey) {
    INT h, l, m;

    l = 0;
    h = NUM_VKEYINFO;

    while (l <= h) {
        m = (l + h) / 2;
        if (g_VKeyInfo[m].bVKey <= bVkey) l = m + 1;
        if (g_VKeyInfo[m].bVKey >= bVkey) h = m - 1;
    }

    if (l - h == 2) return (l - 1);

    return -1L;
}

// A, B, SELECT, START, UP, DOWN, LEFT, RIGHTの順
// NOTE : A, B, SELECT, STARTが最初にくることを前提にして、
//        ジョイスティックのボタン設定を実装している
//        (UP, DOWN, LEFT, RIGHTは未使用)
// in order of A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
// NOTE: On the premise that A, B, SELECT, START comes first,
// Implement the joystick button settings
// (UP, DOWN, LEFT, RIGHT are not used)
LPTSTR g_szEmuKeyName[EMULATOR_NUM_BUTTONS];

LPTSTR g_szEditKeyName[KEYACCEL_NUM_COMMANDS];

void InitEmuKeyName() {
    LPTSTR tmp[EMULATOR_NUM_BUTTONS] = {
            STRING_KEYCONFIG_EMU_A,      STRING_KEYCONFIG_EMU_B,
            STRING_KEYCONFIG_EMU_SELECT, STRING_KEYCONFIG_EMU_START,
            STRING_KEYCONFIG_EMU_UP,     STRING_KEYCONFIG_EMU_DOWN,
            STRING_KEYCONFIG_EMU_LEFT,   STRING_KEYCONFIG_EMU_RIGHT};

    memcpy(g_szEmuKeyName, tmp, sizeof(tmp));
}

void InitEditKeyName() {
    LPTSTR tmp[KEYACCEL_NUM_COMMANDS] = {
            STRING_KEYCONFIG_EDIT_ADD1,         STRING_KEYCONFIG_EDIT_DEC1,
            STRING_KEYCONFIG_EDIT_ADD16,        STRING_KEYCONFIG_EDIT_DEC16,
            STRING_KEYCONFIG_EDIT_UP,           STRING_KEYCONFIG_EDIT_DOWN,
            STRING_KEYCONFIG_EDIT_LEFT,         STRING_KEYCONFIG_EDIT_RIGHT,
            STRING_KEYCONFIG_EDIT_NEXTPAGE,     STRING_KEYCONFIG_EDIT_PREVPAGE,
            STRING_KEYCONFIG_EDIT_NEXTOBJ,      STRING_KEYCONFIG_EDIT_PREVOBJ,
            STRING_KEYCONFIG_EDIT_SAVEFILE,     STRING_KEYCONFIG_EDIT_UNDO,
            STRING_KEYCONFIG_EDIT_ROOM,         STRING_KEYCONFIG_EDIT_BADGUYS,
            STRING_KEYCONFIG_EDIT_MAP,          STRING_KEYCONFIG_EDIT_TESTPLAY,
            STRING_KEYCONFIG_EDIT_PAGETESTPLAY, STRING_KEYCONFIG_EDIT_PAGETESTPLAY2,
            STRING_KEYCONFIG_EDIT_STOP,         STRING_KEYCONFIG_EDIT_SAVEEMU,
            STRING_KEYCONFIG_EDIT_LOADEMU,      STRING_KEYCONFIG_EDIT_EMUSETTING,
            STRING_KEYCONFIG_EDIT_NEXTWIN,      STRING_KEYCONFIG_EDIT_PREVWIN,
    };

    memcpy(g_szEditKeyName, tmp, sizeof(tmp));
}

void InitKeys() {
    InitEmuKeyName();
    InitEditKeyName();
}

#define CUSTOMIZE_KEYIDS 2

#define CUSTOMIZE_KEYS_ID_INVALID 0
#define CUSTOMIZE_KEYS_ID_EDIT 1
#define CUSTOMIZE_KEYS_ID_EMU 2

static void EnableAccessoryKey(HWND hDlg, BOOL blEnable) {
    CheckDlgButton(hDlg, IDC_CTRL, FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_CTRL), blEnable);
    CheckDlgButton(hDlg, IDC_ALT, FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_ALT), blEnable);
    CheckDlgButton(hDlg, IDC_SHIFT, FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_SHIFT), blEnable);
}

static DWORD FindOverlappedKeyDefinition(WORD wVkey, LPWORD lpwEditVKeys,
                                         LPWORD lpbEmuVKeys) {
    int N;

    for (N = 0; N < KEYACCEL_NUM_COMMANDS; N++) {
        if (lpwEditVKeys[N] == wVkey) return ((CUSTOMIZE_KEYS_ID_EDIT << 16) | N);
    }

    for (N = 0; N < EMULATOR_NUM_BUTTONS; N++) {
        if (lpbEmuVKeys[N] == wVkey) return ((CUSTOMIZE_KEYS_ID_EMU << 16) | N);
    }

    return 0;
}

static LPTSTR FormatOverlappedKeyInformation(LPTSTR szBuf, DWORD dwResult) {
    switch (dwResult >> 16) {
        case CUSTOMIZE_KEYS_ID_EDIT:
            wsprintf(szBuf, STRING_KEYCONFIG_OVERLAPPED, STRING_KEYCONFIG_KEYACCEL,
                     g_szEditKeyName[dwResult & 0xFFFF]);
            break;
        case CUSTOMIZE_KEYS_ID_EMU:
            wsprintf(szBuf, STRING_KEYCONFIG_OVERLAPPED, STRING_KEYCONFIG_EMULATOR,
                     g_szEmuKeyName[dwResult & 0xFFFF]);
            break;
    }
    return szBuf;
}

#define WM_UPDATEKEYVALUE (WM_USER + 1)

// ジョイスティック
// Joystick
DWORD g_adwJoyButtonFlags[JOYSTICK_MAX_BUTTONS] = {
        JOY_BUTTON1,  JOY_BUTTON2,  JOY_BUTTON3,  JOY_BUTTON4,  JOY_BUTTON5,
        JOY_BUTTON6,  JOY_BUTTON7,  JOY_BUTTON8,  JOY_BUTTON9,  JOY_BUTTON10,
        JOY_BUTTON11, JOY_BUTTON12, JOY_BUTTON13, JOY_BUTTON14, JOY_BUTTON15,
        JOY_BUTTON16, JOY_BUTTON17, JOY_BUTTON18, JOY_BUTTON19, JOY_BUTTON20,
        JOY_BUTTON21, JOY_BUTTON22, JOY_BUTTON23, JOY_BUTTON24, JOY_BUTTON25,
        JOY_BUTTON26, JOY_BUTTON27, JOY_BUTTON28, JOY_BUTTON29, JOY_BUTTON30,
        JOY_BUTTON31, JOY_BUTTON32};

// ジョイスティックの一つのボタンフラグビットからコンボボックスでのインデックスを得る
// Get index in combo box from one button flag bit of joystick
static UINT FindJoyButtonIndex(DWORD dwJoyButton) {
    DWORD n;

    for (n = 0; n < JOYSTICK_MAX_BUTTONS; n++) {
        if (g_adwJoyButtonFlags[n] == dwJoyButton) return n;
    }

    return (UINT)-1;
}

LRESULT CALLBACK CustomizeDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
    static WORD bEmuVKeys[EMULATOR_NUM_BUTTONS];
    static WORD bEditVKeys[KEYACCEL_NUM_COMMANDS];
    static DWORD dwEmuJoyButtons[EMULATOR_NUM_JOYBUTTONS];
    static WHEELFUNC wfWheelFunc[CUSTOMIZE_WHEEL_NUMFUNCS];
    static LRESULT uKeyID, uKey, uKeyValue;
    switch (message) {
        case WM_INITDIALOG: {
            UINT N;
            LPTSTR lpKeyID[CUSTOMIZE_KEYIDS] = {STRING_KEYCONFIG_KEYACCEL,
                                                STRING_KEYCONFIG_EMULATOR};
            LPTSTR lpPreset[KEYACCEL_NUM_PRESETS] = {STRING_KEYCONFIG_PRESET_DESKTOP,
                                                     STRING_KEYCONFIG_PRESET_NOTE};
            LPTSTR lpWheel[CUSTOMIZE_WHEEL_NUMFUNCS] = {
                    STRING_KEYCONFIG_WHEEL_NONE, STRING_KEYCONFIG_WHEEL_CTRL,
                    STRING_KEYCONFIG_WHEEL_SHIFT, STRING_KEYCONFIG_WHEEL_SHIFTCTRL};
            LPTSTR lpWheelFunc[CUSTOMIZE_WHEEL_NUMFUNCS] = {
                    STRING_KEYCONFIG_WHEEL_TYPE1, STRING_KEYCONFIG_WHEEL_TYPE16,
                    STRING_KEYCONFIG_WHEEL_PAGE, STRING_KEYCONFIG_WHEEL_OBJ};
            LPTSTR szButtonName = GetTempStringBuffer();

            //---------------
            //  初期化
            //  Initialization
            //---------------
            //
            GetEditorVKeys(&bEditVKeys[0]);
            GetEmulatorVKeys(&bEmuVKeys[0]);
            GetEmulatorJoyButtons(&dwEmuJoyButtons[0]);
            CopyMemory(wfWheelFunc, g_wfWheelFunc,
                       sizeof(WHEELFUNC) * CUSTOMIZE_WHEEL_NUMFUNCS);

            // キーボード
            // keyboard
            for (N = 0; N < CUSTOMIZE_KEYIDS; N++)
                SendDlgItemMessage(hDlg, IDC_KEYID, CB_ADDSTRING, 0,
                                   (LPARAM)lpKeyID[N]);
            for (N = 0; N < NUM_VKEYINFO; N++)
                SendDlgItemMessage(hDlg, IDC_KEYVALUE, CB_ADDSTRING, 0,
                                   (LPARAM)g_VKeyInfo[N].lpKeyName);
            for (N = 0; N < KEYACCEL_NUM_PRESETS; N++)
                SendDlgItemMessage(hDlg, IDC_PRESET, CB_ADDSTRING, 0,
                                   (LPARAM)lpPreset[N]);
            SendDlgItemMessage(hDlg, IDC_PRESET, CB_SETCURSEL, 0, 0);

            // マウスホイール
            // Mouse Wheel
            for (N = 0; N < CUSTOMIZE_WHEEL_NUMFUNCS; N++)
                SendDlgItemMessage(hDlg, IDC_WHEELLIST, LB_ADDSTRING, 0,
                                   (LPARAM)lpWheel[N]);
            for (N = 0; N < CUSTOMIZE_WHEEL_NUMFUNCS; N++)
                SendDlgItemMessage(hDlg, IDC_WHEELCOMMAND, CB_ADDSTRING, 0,
                                   (LPARAM)lpWheelFunc[N]);

            // ジョイスティックのボタン
            // リストにコントローラーのボタン名を追加
            // Joystick button
            // Add controller button name to the list
            for (N = 0; N < EMULATOR_NUM_JOYBUTTONS; N++)
                SendDlgItemMessage(hDlg, IDC_JOYBUTTONLIST, LB_ADDSTRING, 0,
                                   (LPARAM)g_szEmuKeyName[N]);

            // コンボボックスにジョイスティックのボタン名を追加
            // Add joystick button name to combo box
            for (N = 0; N < JOYSTICK_MAX_BUTTONS; N++) {
                wsprintf(szButtonName, STRING_KEYCONFIG_BUTTONX, N + 1);
                SendDlgItemMessage(hDlg, IDC_JOYBUTTONVALUE, CB_ADDSTRING, 0,
                                   (LPARAM)szButtonName);
            }

            uKeyID = uKey = uKeyValue = 0;

            SendDlgItemMessage(hDlg, IDC_KEYID, CB_SETCURSEL, uKeyID, 0);
            SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_KEYID, CBN_SELCHANGE), 0);

            SendDlgItemMessage(hDlg, IDC_WHEELLIST, LB_SETCURSEL, 0, 0);
            SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_WHEELLIST, LBN_SELCHANGE), 0);

            SendDlgItemMessage(hDlg, IDC_JOYBUTTONLIST, LB_SETCURSEL, 0, 0);
            SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_JOYBUTTONLIST, LBN_SELCHANGE),
                        0);
        } break;
        case WM_UPDATEKEYVALUE: {
            BYTE bVKey = 0;

            uKey = SendDlgItemMessage(hDlg, IDC_KEYLIST, LB_GETCURSEL, 0, 0);
            if (uKey == LB_ERR) {
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
            switch (uKeyID) {
                case CUSTOMIZE_KEYS_ID_EDIT: {
                    WORD wVKey = bEditVKeys[uKey];

                    bVKey = GETVKEY(wVKey);
                    CheckDlgButton(hDlg, IDC_CTRL, GETCTRL(wVKey));
                    CheckDlgButton(hDlg, IDC_ALT, GETALT(wVKey));
                    CheckDlgButton(hDlg, IDC_SHIFT, GETSHIFT(wVKey));
                } break;
                case CUSTOMIZE_KEYS_ID_EMU:
                    bVKey = GETVKEY(bEmuVKeys[uKey]);
                    break;
            }

            uKeyValue = FindVkeyIndex(bVKey);
            SendDlgItemMessage(hDlg, IDC_KEYVALUE, CB_SETCURSEL, uKeyValue, 0);
        }
            return TRUE;
        case WM_COMMAND: {
            WORD wNotifyCode = HIWORD(wParam);
            WORD wID = LOWORD(wParam);
            switch (wID) {
                case IDC_KEYID: {
                    if (wNotifyCode == CBN_SELCHANGE) {
                        UINT N;

                        uKeyID =
                                SendDlgItemMessage(hDlg, IDC_KEYID, CB_GETCURSEL, 0, 0);
                        if (uKeyID == CB_ERR) {
                            EndDialog(hDlg, FALSE);
                            return TRUE;
                        }

                        // キーIDは、1から始まる
                        // Key ID starts with 1
                        uKeyID++;

                        SendDlgItemMessage(hDlg, IDC_KEYLIST, LB_RESETCONTENT, 0, 0);
                        switch (uKeyID) {
                            case CUSTOMIZE_KEYS_ID_EDIT:
                                for (N = 0; N < KEYACCEL_NUM_COMMANDS; N++)
                                    SendDlgItemMessage(hDlg, IDC_KEYLIST, LB_ADDSTRING,
                                                       0, (LPARAM)g_szEditKeyName[N]);
                                EnableAccessoryKey(hDlg, TRUE);
                                break;
                            case CUSTOMIZE_KEYS_ID_EMU:
                                for (N = 0; N < EMULATOR_NUM_BUTTONS; N++)
                                    SendDlgItemMessage(hDlg, IDC_KEYLIST, LB_ADDSTRING,
                                                       0, (LPARAM)g_szEmuKeyName[N]);
                                EnableAccessoryKey(hDlg, FALSE);
                                break;
                        }

                        uKey = 0;
                        SendDlgItemMessage(hDlg, IDC_KEYLIST, LB_SETCURSEL, uKey, 0);

                        SendMessage(hDlg, WM_COMMAND,
                                    MAKEWPARAM(IDC_KEYLIST, LBN_SELCHANGE), 0);
                        return TRUE;
                    }
                } break;
                case IDC_KEYLIST: {
                    if (wNotifyCode == LBN_SELCHANGE) {
                        SendMessage(hDlg, WM_UPDATEKEYVALUE, 0, 0);
                    }
                } break;
                case IDC_KEYVALUE: {
                    if (wNotifyCode == CBN_SELCHANGE) {
                        BYTE bNewVKey;
                        WORD wCurVKey;
                        WORD wNewKey;
                        DWORD dwResult;
                        LPTSTR szBuf = GetTempStringBuffer();

                        uKeyValue = SendDlgItemMessage(hDlg, IDC_KEYVALUE, CB_GETCURSEL,
                                                       0, 0);
                        if (uKeyValue == CB_ERR) {
                            EndDialog(hDlg, FALSE);
                            return TRUE;
                        }
                        bNewVKey = g_VKeyInfo[uKeyValue].bVKey;
                        switch (uKeyID) {
                            case CUSTOMIZE_KEYS_ID_EDIT:
                                wCurVKey = bEditVKeys[uKey];
                                wNewKey = GETASSISTKEY(wCurVKey);
                                SETVKEY(wNewKey, bNewVKey);
                                dwResult = FindOverlappedKeyDefinition(
                                        wNewKey, bEditVKeys, bEmuVKeys);

                                // 新たに選択した項目が選択されていたものと同じ項目でなければ
                                // If the newly selected item is not the same item that
                                // was selected
                                if (dwResult &&
                                    (CUSTOMIZE_KEYS_ID_EDIT != HIWORD(dwResult) ||
                                     uKey != LOWORD(dwResult))) {
                                    FormatOverlappedKeyInformation(szBuf, dwResult);
                                    Msg(szBuf, MB_OK | MB_ICONEXCLAMATION);
                                    SendMessage(hDlg, WM_UPDATEKEYVALUE, 0, 0);
                                    return TRUE;
                                }
                                bEditVKeys[uKey] = wNewKey;
                                break;
                            case CUSTOMIZE_KEYS_ID_EMU:
                                wCurVKey = bEmuVKeys[uKey];
                                wNewKey = (WORD)bNewVKey;
                                dwResult = FindOverlappedKeyDefinition(
                                        wNewKey, bEditVKeys, bEmuVKeys);

                                // 新たに選択した項目が選択されていたものと同じ項目でなければ
                                // If the newly selected item is not the same item that
                                // was selected
                                if (dwResult &&
                                    (CUSTOMIZE_KEYS_ID_EMU != HIWORD(dwResult) ||
                                     uKey != LOWORD(dwResult))) {
                                    FormatOverlappedKeyInformation(szBuf, dwResult);
                                    Msg(szBuf, MB_OK | MB_ICONEXCLAMATION);
                                    SendMessage(hDlg, WM_UPDATEKEYVALUE, 0, 0);
                                    return TRUE;
                                }
                                bEmuVKeys[uKey] = wNewKey;
                                break;
                        }
                        return TRUE;
                    }
                } break;
                case IDC_SHIFT:
                case IDC_ALT:
                case IDC_CTRL: {
                    if (wNotifyCode == BN_CLICKED) {
                        if (uKeyID == CUSTOMIZE_KEYS_ID_EDIT) {
                            WORD wMask, wNewKey;
                            DWORD dwResult;
                            LPTSTR szBuf = GetTempStringBuffer();

                            switch (wID) {
                                case IDC_SHIFT:
                                    wMask = SHIFTBIT;
                                    break;
                                case IDC_ALT:
                                    wMask = ALTBIT;
                                    break;
                                case IDC_CTRL:
                                    wMask = CTRLBIT;
                                    break;
                            }
                            wNewKey = bEditVKeys[uKey];
                            if (IsDlgButtonChecked(hDlg, wID))
                                wNewKey |= wMask;
                            else
                                wNewKey &= ~wMask;

                            dwResult = FindOverlappedKeyDefinition(wNewKey, bEditVKeys,
                                                                   bEmuVKeys);
                            if (dwResult) {
                                // チェックボックスの状態の変化によって同じキー入力になることはない
                                // It will never be the same key input due to change in
                                // the state of the check box
                                FormatOverlappedKeyInformation(szBuf, dwResult);
                                Msg(szBuf, MB_OK | MB_ICONEXCLAMATION);
                                SendMessage(hDlg, WM_UPDATEKEYVALUE, 0, 0);
                                return TRUE;
                            }

                            bEditVKeys[uKey] = wNewKey;
                        }
                    }
                } break;
                case IDC_RESET: {
                    if (wNotifyCode == BN_CLICKED) {
                        if (IDNO == Msg(STRING_KEYCONFIG_INITIALIZE,
                                        MB_YESNO | MB_ICONQUESTION))
                            break;

                        GetPresetEditorKeys(&bEditVKeys[0],
                                            SendDlgItemMessage(hDlg, IDC_PRESET,
                                                               CB_GETCURSEL, 0, 0));
                        GetDefaultEmulatorKeys(&bEmuVKeys[0]);

                        uKey = 0;
                        SendDlgItemMessage(hDlg, IDC_KEYLIST, LB_SETCURSEL, uKey, 0);
                        SendMessage(hDlg, WM_COMMAND,
                                    MAKEWPARAM(IDC_KEYLIST, LBN_SELCHANGE), 0);
                    }
                } break;
                case IDC_WHEELLIST: {
                    LRESULT L;
                    if (wNotifyCode == LBN_SELCHANGE) {
                        L = SendDlgItemMessage(hDlg, IDC_WHEELLIST, LB_GETCURSEL, 0, 0);
                        if (L != LB_ERR) {
                            SendDlgItemMessage(hDlg, IDC_WHEELCOMMAND, CB_SETCURSEL,
                                               wfWheelFunc[L], 0);
                        }
                    }
                } break;
                case IDC_WHEELCOMMAND: {
                    LRESULT L, C, N;
                    WHEELFUNC T;
                    if (wNotifyCode == CBN_SELCHANGE) {
                        L = SendDlgItemMessage(hDlg, IDC_WHEELLIST, LB_GETCURSEL, 0, 0);
                        if (L != LB_ERR) {
                            C = SendDlgItemMessage(hDlg, IDC_WHEELCOMMAND, CB_GETCURSEL,
                                                   0, 0);
                            if (C != CB_ERR) {
                                // 現在設定されている操作を見つける
                                // Find the currently set operation
                                for (N = 0; N < CUSTOMIZE_WHEEL_NUMFUNCS; N++) {
                                    if (wfWheelFunc[N] == (WHEELFUNC)C) break;
                                }
                                if (N == CUSTOMIZE_WHEEL_NUMFUNCS) {
                                    wfWheelFunc[L] = (WHEELFUNC)C;
                                } else {
                                    T = wfWheelFunc[N];
                                    wfWheelFunc[N] = wfWheelFunc[L];
                                    wfWheelFunc[L] = T;
                                }
                            }
                        }
                    }
                } break;
                case IDC_JOYBUTTONLIST: {
                    LRESULT L;
                    if (wNotifyCode == LBN_SELCHANGE) {
                        L = SendDlgItemMessage(hDlg, IDC_JOYBUTTONLIST, LB_GETCURSEL, 0,
                                               0);

                        // コンボボックスを更新
                        // Update Combo Box
                        if (L != LB_ERR)
                            SendDlgItemMessage(hDlg, IDC_JOYBUTTONVALUE, CB_SETCURSEL,
                                               FindJoyButtonIndex(dwEmuJoyButtons[L]),
                                               0);
                    }
                } break;
                case IDC_JOYBUTTONVALUE: {
                    LRESULT L, V, N;
                    DWORD dwNewButton, T;
                    if (wNotifyCode == CBN_SELCHANGE) {
                        L = SendDlgItemMessage(hDlg, IDC_JOYBUTTONLIST, LB_GETCURSEL, 0,
                                               0);
                        if (L != LB_ERR) {
                            V = SendDlgItemMessage(hDlg, IDC_JOYBUTTONVALUE,
                                                   CB_GETCURSEL, 0, 0);
                            if (V != CB_ERR) {
                                // ボタンの新しい値
                                // New value for the button
                                dwNewButton = g_adwJoyButtonFlags[V];

                                // 重複設定を検出
                                // Detect duplicate settings
                                for (N = 0; N < EMULATOR_NUM_JOYBUTTONS; N++) {
                                    if (dwEmuJoyButtons[N] == dwNewButton) break;
                                }
                                if (N != EMULATOR_NUM_JOYBUTTONS) {
                                    // 重複がある場合、
                                    // 重複したボタン設定と新しいボタン設定を入れ替える
                                    // If there is duplication,
                                    // Swap duplicate button settings and new button
                                    // settings
                                    T = dwEmuJoyButtons[N];
                                    dwEmuJoyButtons[N] = dwEmuJoyButtons[L];
                                    dwEmuJoyButtons[L] = T;
                                } else {
                                    // 重複がなければ、
                                    // ジョイスティックのボタンフラグ定数がセットされた
                                    // 配列に新しいデータをセット
                                    // Without duplication,
                                    // Joystick button flag constant set
                                    // Set new data to array
                                    dwEmuJoyButtons[L] = dwNewButton;
                                }
                            }
                        }
                    }
                } break;
                case IDOK:
                    WriteToRegistry(INI_EMULATOR_KEYS, REG_BINARY, bEmuVKeys,
                                    EMULATOR_NUM_BUTTONS * sizeof(WORD));
                    WriteToRegistry(INI_EDITOR_KEYS, REG_BINARY, bEditVKeys,
                                    KEYACCEL_NUM_COMMANDS * sizeof(WORD));
                    WriteToRegistry(INI_EDITOR_WHEEL, REG_BINARY, wfWheelFunc,
                                    CUSTOMIZE_WHEEL_NUMFUNCS * sizeof(WHEELFUNC));
                    WriteToRegistry(INI_EMULATOR_JOYBUTTONS, REG_BINARY,
                                    dwEmuJoyButtons,
                                    EMULATOR_NUM_JOYBUTTONS * sizeof(DWORD));
                    Msg(STRING_KEYCONFIG_NOTIFY, MB_OK | MB_ICONINFORMATION);
                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return TRUE;
            }
        }
    }

    return FALSE;
}
