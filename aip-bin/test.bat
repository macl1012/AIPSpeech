@echo off
set var=0
rem ************循环开始了
:continue
set /a var+=1
echo 第%var%次循环
echo 第%var%次循环 >> 1.txt
call tts.exe -t 二十三点六七元 -f audio.mp3 -v 15 >> 1.txt
if %var% lss 100 goto continue
rem ************循环结束了
echo 循环执行完毕
pause
