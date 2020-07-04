rmdir /S /Q build
mkdir build
cd build
qmake ..\src\VeraGraphicsConverter.pro
nmake
cd ..
