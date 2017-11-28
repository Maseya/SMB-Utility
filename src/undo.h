/************************************************************************************

                                  smb Utility

  File: undo.h
  Description:
  History:

 ************************************************************************************/
#ifndef UNDO_H
#define UNDO_H

#define UNDO_MENUTEXT              STRING_UNDO_MENUTEXT
#define UNDO_MENUTEXT_BUFFERSIZE   50

#define UNDONAME_KEYEDIT           STRING_UNDONAME_KEYEDIT
#define UNDONAME_DLGEDIT           STRING_UNDONAME_DLGEDIT
#define UNDONAME_SENDOBJ           STRING_UNDONAME_SENDOBJ
#define UNDONAME_HEADDLG           STRING_UNDONAME_HEADDLG
#define UNDONAME_CHRLOAD           STRING_UNDONAME_CHRLOAD
#define UNDONAME_TOOLSTR           STRING_UNDONAME_TOOLSTR
#define UNDONAME_TOOLLOOPBIN       STRING_UNDONAME_TOOLLOOPBIN
#define UNDONAME_TOOLWORLD         STRING_UNDONAME_TOOLWORLD
#define UNDONAME_TOOLAREAROOM      STRING_UNDONAME_TOOLAREAROOM
#define UNDONAME_TOOLDEMORECORDER  STRING_UNDONAME_TOOLDEMORECORDER
#define UNDONAME_TOOLOTHER         STRING_UNDONAME_TOOLOTHER
#define UNDONAME_TOOLGLOBAL        STRING_UNDONAME_TOOLGLOBAL

void undoPrepare(LPCTSTR lpUndoName);
void undoRestore();
void undoReset();
BOOL undoIsEnabled();
LPTSTR undoGetNameBuffer();

extern BOOL g_blKeyCommandUndo;

#endif