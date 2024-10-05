#ifndef LOGVIEW_H
#define LOGVIEW_H

#define LOGVIEW_DEFAULTHEIGHT 512
#define LOGVIEW_BORDERWIDTH 5

#define LOGVIEW_FONTHEIGHT 12

#define LOGVIEW_FONTNAME __T("ＭＳ Ｐゴシック")
#define LOGVIEW_FONTNAMEI __T("MS Sans Serif")

#define LOGVIEW_OUTPUTSTRING_CR 0x00000001
#define LOGVIEW_OUTPUTSTRING_BEEP 0x00000002

BOOL lv_OutputDebugString(LPTSTR);

HWND lv_GetBorderWnd();
HWND lv_GetLogViewWnd();
BOOL lv_OutputString(LPTSTR, DWORD);
BOOL lv_ShowLogView();
VOID lv_AdjustLogView();
BOOL lv_Initialize();
VOID lv_Trash();
VOID lv_DestroyLogView();
HWND lv_CreateLogView(HWND hWndParent, int nHeight);

#endif
