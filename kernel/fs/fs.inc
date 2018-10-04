;***
; XenonOS - File System Service
; Copyright 2007-2008 Ryabov Alexey aka Sh@dy (alex_megasoft@mail.ru)
;***

include  'fd.inc'

;Файловый системный сервис
;in:  ax - номер ф-ии
;     es:bx - адрес буфера
;     cx - размер файла
;     ds:si - ASCIIZ-строка с именем файла
;out: ax - статус
file_system:
  mov  word [gs:TASK_CHANGE],0
  cmp  ax,0   ;Читать файл
  je   comm_read_file
  cmp  ax,1   ;Читать папку
  je   comm_read_dir
  cmp  ax,2   ;Считать информацию о файле
  je   comm_file_info
  mov  word [gs:TASK_CHANGE],1
  ret

;
; Чтение файла
;
comm_read_file:
  cmp  byte [si],'/'
  jne  err_file_not_found

  cmp  word [si+1],'fd'
  je   .read_file_fd
  cmp  word [si+1],'FD'
  je   .read_file_fd
  cmp  word [si+1],'Fd'
  je   .read_file_fd
  cmp  word [si+1],'fD'
  je   .read_file_fd

  jmp  err_file_not_found
.read_file_fd:
  cmp  byte [si+3],'/'
  jne  err_file_not_found
  add  si,3
  call fd_read_file
  mov  word [gs:TASK_CHANGE],1
  ret


;
; Чтение папки
;
comm_read_dir:
  cmp  byte [si],'/'
  jne  err_file_not_found

  cmp  word [si+1],'fd'
  je   .read_dir_fd
  cmp  word [si+1],'FD'
  je   .read_dir_fd
  cmp  word [si+1],'Fd'
  je   .read_dir_fd
  cmp  word [si+1],'fD'
  je   .read_dir_fd

  jmp  err_file_not_found
.read_dir_fd:
  cmp  byte [si+3],'/'
  jne  err_file_not_found
  add  si,3
  call fd_read_folder
  mov  word [gs:TASK_CHANGE],1
  ret


;
; Информация о файле
;
comm_file_info:
  cmp  byte [si],'/'
  jne  err_file_not_found

  cmp  word [si+1],'fd'
  je   .get_info_fd
  cmp  word [si+1],'FD'
  je   .get_info_fd
  cmp  word [si+1],'Fd'
  je   .get_info_fd
  cmp  word [si+1],'fD'
  je   .get_info_fd

  jmp  err_file_not_found
.get_info_fd:
  cmp  byte [si+3],'/'
  jne  err_file_not_found
  add  si,3
  call fd_get_file_info
  mov  word [gs:TASK_CHANGE],1
  ret

err_file_not_found:
  mov  ax,FS_FILE_NOT_FOUND
  mov  word [gs:TASK_CHANGE],1
  ret