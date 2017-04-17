@ECHO off

rem # Instructions
rem # Right-Click this file > Run as Administrator

net session >nul 2>&1
if NOT %errorLevel% == 0 (
	echo Error: Please run as Administrator
	pause
	exit
)

cd /D %~dp0
Powershell.exe -Command "Set-ExecutionPolicy RemoteSigned"
Powershell.exe -File .\setup.ps1
pause