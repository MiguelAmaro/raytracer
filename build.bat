@echo off

if not exist build mkdir build

rem PROJECT INFO / SOURCE FILES
rem ============================================================
set PROJECT_DIR=%cd%
set PROJECT_NAME=main
set SOURCES= %PROJECT_DIR%\src\%PROJECT_NAME%.c

rem COMPILER(MSVC) OPTIONS
rem ============================================================
set MSVC_SEARCH_DIRS=-I%PROJECT_DIR%\thirdparty\pcg\

set MSVC_WARNINGS=-W4
set MSVC_FLAGS= %MSVC_WARNINGS% -std:c11 -nologo -O2 -Zi
rem -Od -Zi

rem LINKER(MSVC) OPTIONS
rem ============================================================
set MSVC_LINK_FLAGS=-subsystem:console
set MSVC_LIBS=advapi32.lib

rem START BUILD
rem ============================================================
set path=%PROJECT_DIR%\build;%path%
pushd build
cl %MSVC_FLAGS% %MSVC_SEARCH_DIRS% %SOURCES% /link %MSVC_LINK_FLAGS% %MSVC_LIBS%
popd

pause
