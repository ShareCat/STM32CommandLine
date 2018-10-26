REM if .\..\Output\STM32F103_CLI.hex exit, then convert it to bin
if exist .\..\Output\STM32F103_CLI.hex (hex2bin.exe  .\..\Output\STM32F103_CLI.hex)

REM if STM32F103_CLI.bin convert success, then copy the bin file to this dir.
if exist .\..\Output\STM32F103_CLI.bin (copy .\..\Output\STM32F103_CLI.bin .\)
pause
