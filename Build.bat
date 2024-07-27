@echo off

:start
make clean
make
echo Press any key to rebuild.
pause
goto start 