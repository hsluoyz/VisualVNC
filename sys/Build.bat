if "%1" == "" goto InvalidParameter

if not exist %1\bin\setenv.bat goto SetenvNotFound

set disk_path=%~d0
set dir_path=%cd%
call %1\bin\setenv.bat %1 %2
%disk_path%
cd %dir_path%
build.exe -cZ
if "%2" == "checked" goto CopyChecked
if "%2" == "free" goto CopyFree
goto exit

:CopyChecked
copy .\objchk_win7_x86\i386\passthru.sys ..\data\passthru.sys
goto exit

:CopyFree
copy .\objfre_win7_x86\i386\passthru.sys ..\data\passthru.sys
goto exit

:InvalidParameter
echo Invalid Parameter.
goto exit

:SetenvNotFound
echo Can't found Setenv.bat.
goto exit

:exit