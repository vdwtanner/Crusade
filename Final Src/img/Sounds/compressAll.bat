@echo off
for /r %%i in (*.wav) do call "AdpcmEncode.exe" "%%i" "%%~ni.adpcm"
mkdir UncompressedWAVs
for /r %%i in (*.wav) do move "%%i" "UncompressedWAVs\"
echo "Finished!"
pause >nul