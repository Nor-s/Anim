@ECHO OFF
echo start bat---------------------------------------------------
call conda activate mp_gui_build
if exist %cd%\dist\mp2mm\mp2mm.exe (
    echo exist mp2mm
) else (
    pyinstaller %cd%\mp2mm.spec  --noconfirm
)
echo end bat---------------------------------------------------