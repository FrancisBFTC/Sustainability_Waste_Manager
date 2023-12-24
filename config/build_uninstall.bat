cls
ECHO OFF
g++.exe -c uninstall.cpp -o uninstall.o
g++.exe  uninstall.o -o uninstall.exe
del *.o
pause