# **Build**

Note: The path must be in English, no spaces.

requirements

-   CMake (version >= 3.21)
-   git

## **Windows(VS2022 amd64)**

-   MSVC 19.33.31517.0 (VS2022 amd64) 
    -   workload
         -  Desktop Development with C++
         -  Universal Windows Platform Development
         -   C++ CMake Tools for Windows
         -    Python Development

    -   Visual Studio must satisfy the conditions described in [this document](https://github.com/python/cpython/tree/main/PCbuild) (for cpython)

        > Install Microsoft Visual Studio with Python workload and Python native development component.

    -   and, check c++ cmake tools

        ![](https://docs.microsoft.com/ko-kr/cpp/build/media/cmake-install-2019.png?view=msvc-170)
        
### **Visual Studio**

see [/docs/windows_build.pdf](/docs/windows_build.pdf)

### **Terminal**

```
git clone --recursive https://github.com/Nor-s/Anim.git
cd Anim
mkdir build
cd build
```

#### **Cmake**

```
cmake ..   -DCMAKE_CXX_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Preview/VC/Tools/MSVC/14.33.31517/bin/Hostx64/x64/cl.exe"
cmake --build . --config Release
```

or

```
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

output files path: build/bin/Release/

You need to move the files to this folder: build/bin/

#### **Cmake with Ninja**


```
cmake .. -G Ninja -DCMAKE_CXX_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Preview/VC/Tools/MSVC/14.33.31517/bin/Hostx64/x64/cl.exe"

ninja
```

or

```
cmake  .. -G Ninja
ninja
```

## **Linux**

-   ~~gcc 9.4.0 x86_64-linux-gnu~~

## **Mac OS (silicon)**

-   Clang 13.0.0 arm64-apple-darwin21.2.0
-   Python 3.10 and newer
-   Homebrew 3.5.9

```bash
xcode-select --install
brew install pkg-config openssl@1.1 xz gdbm tcl-tk
```

```bash
git clone --recursive https://github.com/Nor-s/Anim.git
cd Anim

mkdir build
cd build
cmake ..
make
```


## **Error handling**

-   when building cpython (https error)
    - https://stackoverflow.com/questions/54175042/python-3-7-anaconda-environment-import-ssl-dll-load-fail-error
