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


fat_pos      equ     0500h		  ;Местонахождение FAT
root_pos     equ     1700h		  ;Местонахождение ROOT

os_ver_major equ     0
os_ver_minor equ     3

BASE_CONST   equ     100000h

					  ;Общие данные системы
SCR_LFB      equ     (BASE_CONST+0000h)   ;(dword) адрес LFB
SCR_WIDTH    equ     (BASE_CONST+0004h)   ;(word) ширина экрана
SCR_HEIGHT   equ     (BASE_CONST+0006h)   ;(word) высота экрана
SCR_BPP      equ     (BASE_CONST+0008h)   ;(word) глубина цвета (кол-во бит на пиксель)
SCR_BPL      equ     (BASE_CONST+000Ah)   ;(word) число целых байт в логической строке
RAM_SIZE     equ     (BASE_CONST+000Ch)   ;(word) размер оперативной памяти (1-16 2-32 3-64 4-128 5-256 6-512)
FONT_SEG     equ     (BASE_CONST+000Eh)   ;(word) адрес сегмента системного шрифта
SCR_MODE     equ     (BASE_CONST+0010h)   ;(word) номер видео режима vesa

					  ;Данные процессов (256 слотов)
TASK_COUNT   equ     (BASE_CONST+0100h)   ;(word) общее количество процессов
TASK_CURR    equ     (BASE_CONST+0102h)   ;(word) номер текущей задачи
TASK_CHANGE  equ     (BASE_CONST+0104h)   ;(word) флаг переглючения задач (Можно было и байт выделить... но для выравнивания лучше слово)
		    ;(BASE_CONST+0106h)
		    ;(BASE_CONST+0108h)
		    ;(BASE_CONST+010Ah)   FREE!!! зарезервировано :)
		    ;(BASE_CONST+010Ch)
		    ;(BASE_CONST+010Eh)
TASK_BASE    equ     (BASE_CONST+0110h)   ;Адрес, с которого начинаются структуры процессов
					  ;Данные окон (256 слотов)
WINDOW_BASE  equ     (BASE_CONST+3110h)   ;Адрес начала структур окон
WINDOW_STACK equ     (BASE_CONST+5110h)   ;256 байт
MOUSE_UNDER  equ     (BASE_CONST+5210h)   ;Буфер под область, накрываемую курсором 4096

DISPLAY_INFO equ     200000h