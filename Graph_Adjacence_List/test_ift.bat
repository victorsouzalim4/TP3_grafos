@echo off
echo === Testando IFT Image Processor ===
echo.

echo Configurando PATH do OpenCV...
set PATH=%PATH%;%~dp0third_party\opencv\x64\mingw\bin

echo PATH configurado: %PATH%
echo.

echo Tentando executar sem argumentos (deve mostrar usage):
ift_image_processor.exe
echo Exit code: %ERRORLEVEL%
echo.

echo Tentando executar com --help:
ift_image_processor.exe --help
echo Exit code: %ERRORLEVEL%
echo.

echo Testando com imagem inexistente:
ift_image_processor.exe teste.jpg
echo Exit code: %ERRORLEVEL%
echo.

echo === Fim dos testes ===
pause 