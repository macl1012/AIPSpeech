@echo off
set var=0
rem ************ѭ����ʼ��
:continue
set /a var+=1
echo ��%var%��ѭ��
echo ��%var%��ѭ�� >> 1.txt
call tts.exe -t ��ʮ��������Ԫ -f audio.mp3 -v 15 >> 1.txt
if %var% lss 100 goto continue
rem ************ѭ��������
echo ѭ��ִ�����
pause
