#include <windows.h>
#include "../ini.h"
#include "../strings.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	if(IDNO==MessageBox(NULL,STRING_DELREG_CONFIRM,STRING_DELREG_TITLE,MB_YESNO|MB_ICONQUESTION))
		return 0;

	if(DeleteRegistryEntries()){
		MessageBox(NULL,STRING_DELREG_COMPLETE,STRING_DELREG_TITLE,MB_OK|MB_ICONINFORMATION);
	}
	else{
		MessageBox(NULL,STRING_DELREG_ERROR_01,STRING_DELREG_TITLE,MB_OK|MB_ICONEXCLAMATION);
	}

    return 0;
}