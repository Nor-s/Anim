@ECHO OFF
SET mypath=%~dp0
echo -- PATH: %mypath:~0,-1%

SET cpython_path=%mypath%\..\external\cpython\
SET install_path=%mypath%\..\python\


if exist %mypath%\..\python\python.exe (
     echo -- exist python
) else (
     echo -- START::Python build 
    call %cpython_path%\PCBuild\clean.bat 
    call %cpython_path%\PCBuild\build.bat -e -p x64
    start %cpython_path%\PCBuild\amd64\python.exe %cpython_path%PC\layout\main.py -b %cpython_path%\PCBuild\amd64  -s %cpython_path% -t %cpython_path%\temp\ --preset-default --copy %install_path%
     echo -- END::Python build 

     echo -- START::LAYOUT 
    %cpython_path%\PCBuild\amd64\python.exe %cpython_path%PC\layout\main.py -b %cpython_path%\PCBuild\amd64  -s %cpython_path% -t %cpython_path%\temp\ --preset-default --copy %install_path%
     echo -- END::LAYOUT 

     echo -- START::PIP INSTALL 
    curl https://bootstrap.pypa.io/get-pip.py -o %mypath%\get-pip.py 
    %install_path%\python.exe %mypath%\get-pip.py
    %install_path%\Scripts\pip.exe install -r %mypath%\requirements.txt
    ${CPYTHON_PIP} install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
     echo -- END::PIP INSTALL 
)