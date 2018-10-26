@echo off

REM set fromelf.exe directory
set FROMELF=C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe

REM check fromelf.exe existence
if exist %FROMELF% (goto STEP2) else (goto STEP1)

:STEP1
echo fromelf.exe directory error
pause & exit

:STEP2
REM if .\..\Output\STM32F103_CLI.axf exit, then convert it to bin
if exist .\..\Output\STM32F103_CLI.axf (%FROMELF% ".\..\Output\STM32F103_CLI.axf" --bin --output ".\..\Output\STM32F103_CLI.bin")

REM if STM32F103_CLI.bin convert success, then copy the bin file to this dir.
if exist .\..\Output\STM32F103_CLI.bin (copy .\..\Output\STM32F103_CLI.bin .\)

pause & exit
