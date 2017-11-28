#ifndef OBJVIEWBMP_H
#define OBJVIEWBMP_H


typedef struct _tagASSISTBMPDATA
{
	int iPosDeltaX;
	int iPosDeltaY;
	int iBmpIndex;
}ASSISTBMPDATA, FAR *LPASSISTBMPDATA;



VOID TrashAssistBmp();
BOOL InitAssistBmp(HWND hWnd);
BOOL DrawMapAssistBmpData(LPOBJECTSEEKINFO lpOSI, HDC hDestMemDC, int x, int y);
BOOL DrawBadguysAssistBmpData(LPOBJECTSEEKINFO lpOSI, HDC hDestMemDC, int x, int y);
void BitBltAssistBmp(HDC hDestMemDC, LPASSISTBMPDATA lpABD, int x, int y);

#endif