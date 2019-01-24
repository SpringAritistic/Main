@echo off
echo 当前文件夹的所有文件有：
set file=文件名.txt
set show=———————

echo %show%
dir *.h /s /b /o:e >%file%

echo "="ren "& """" &A1 &"""" & " "& """"&B1 &"""" ">>%file%
type %file%

echo %show%

pause
