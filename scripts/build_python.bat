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

     echo -- END::Python build 

     echo -- START::LAYOUT    
     
           %cpython_path%\PCBuild\amd64\python.exe %cpython_path%PC\layout\main.py -b %cpython_path%\PCBuild\amd64  -s %cpython_path% -t %cpython_path%\temp\  --preset-default --include-underpth --include-stable --include-pip --include-dev --include-launchers  --include-venv --include-symbols --copy %install_path%

     echo -- END::LAYOUT 
     echo -- START::PIP INSTALL 
          %install_path%\python.exe -m pip install --upgrade pip
          @REM %install_path%\python.exe -m pip install torch torchvision torchaudio --extra-index-url https://download.pytorch.org/whl/cu116
          %install_path%\python.exe -m pip install --upgrade setuptools numpy cython
          %install_path%\python.exe -m pip install --upgrade simple-romp
          %install_path%\python.exe -m pip install onnxruntime 
          @REM TODO: bpy /2.82 (버전)폴더를 python에 옮겨야함
          @REM %install_path%\python.exe -m pip install future-fstrings mathutils==2.81.2
          @REM %install_path%\python.exe -m pip install /path/to/downloaded/bpy-2.91a0-cp37-cp37m-manylinux2014_x86_64.whl && bpy_post_install       
          @REM ../../../python/scripts/romp --mode=video -t -sc=2  --show_largest --onnx -i=C:/Users/No/Downloads/cxk.mp4 -o=C:/Users/No/Downloads/temp1
          @REM ../../../python/python.exe tools/convert2fbx.py --input=C:/Users/No/Downloads/temp1/video_results.npz --output=C:/Users/No/Downloads/temp1/video_results3.fbx --gender=female
     echo -- END::PIP INSTALL 
)