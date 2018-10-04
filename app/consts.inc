;Константы - значения, возвращенные системными функциями
CM_ERROR	     equ  -1
CM_NO_ERRORS	     equ   0

FS_BASE 	     equ   100
FS_FILE_NOT_FOUND    equ   FS_BASE+1
FS_READ_ERROR	     equ   FS_BASE+2
FS_NO_EXEC_FILE      equ   FS_BASE+3

SS_BASE 	     equ   200
SS_NO_MEMORY	     equ   SS_BASE+1
SS_NOT_RUN_PROCESS   equ   SS_BASE+2

GUI_BASE	     equ   300
GUI_WINDOWS_OVERFLOW equ   GUI_BASE+1
GUI_INVALID_HANDLE   equ   GUI_BASE+2

GUI_WIN_MSG	     equ   100
GUI_WIN_REDRAW	     equ   GUI_WIN_MSG+1
GUI_WIN_KEY_PRESS    equ   GUI_WIN_MSG+2

;Флаги окна
GWF_VISIBLE	     equ   0000000000000001b
GWF_ALWAYS_ON_TOP    equ   0000000000000010b
GWF_MOVABLE	     equ   0000000000000100b
GWF_RESIZABLE	     equ   0000000000001000b
GWF_SKINED	     equ   0000000000010000b