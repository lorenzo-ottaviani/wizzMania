@echo off
REM Set Qt path
set PATH=C:\Qt\6.10.1\mingw_64\bin;C:\Qt\Tools\mingw1120_64\bin;%PATH%
set QT_QPA_PLATFORM_PLUGIN_PATH=%CD%\platforms

REM Launch client
WizzClient.exe
pause