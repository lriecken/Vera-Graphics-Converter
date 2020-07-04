win_build.bat
rmdir /S /Q deploy_win
mkdir deploy_win
cd deploy_win
copy ..\build\release\VeraGraphicsConverter.exe VeraGraphicsConverter.exe
windeployqt VeraGraphicsConverter.exe
