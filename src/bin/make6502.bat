@echo off
rem 6502 CPU Emulator
rem Make M6502 source code for VC++ 
 make6502 M6502.asm -s -d -z
rem Asemble source code for VC++
 nasm -f win32 M6502.asm
rem Now clean up
erase m6502.asm


