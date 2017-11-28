/************************************************************************************

                                  smb Utility

  File: cmnlib.h
  Description:
  History:

 ************************************************************************************/
#ifndef CMNLIB_H
#define CMNLIB_H

#define COMCTRL_ERROR 0
#define COMCTRL_V400  1 
#define COMCTRL_V470  2
#define COMCTRL_V471  4
#define COMCTRL_V472  8
#define COMCTRL_LATER 16

//一時文字列バッファーのサイズ、GetTempStringBuffer()関数で確保されている
#define TMPSTRBUFSIZ 512

#define Malloc(S)     HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, (S))
#define Mfree(P)      HeapFree(GetProcessHeap(), 0, (P))
#define Mrealloc(P,S) HeapReAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, (P), (S))

#define LONG2POINT(l, pt)  ((pt).x = (SHORT)LOWORD(l), (pt).y = (SHORT)HIWORD(l))

BOOL IsMMXAvailable();
void MyCheckMenuItem(HWND, UINT, UINT);
void MySetMenuItemText(HWND, UINT, LPTSTR);
LPTSTR GetTempStringBuffer();
LPTSTR GetTempStringBuffer2();

#endif