@ECHO OFF
echo start---------------------------------------------------
call conda activate mp_gui_build
pyinstaller %cd%/mp2mm.spec  --noconfirm
echo end---------------------------------------------------

@REM if exist exsample.bat (
@REM     echo exsample.bat가 현재 디렉토리에 존재한다
@REM ) else (
@REM     echo exsample.bat가 현재 디렉토리에 존재하지 않는다
@REM )