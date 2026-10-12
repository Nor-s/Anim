# Build Anim on Windows

## Requirements

- Visual Studio 2022 with the Desktop development with C++ workload and Windows SDK
- CMake 3.25 or newer
- Git
- Python 3.10 x64

The source tree contains GLFW, GLM, jsoncpp, Assimp, hiredis, ImGui, GLAD,
pybind11, and stb. CMake builds the native libraries that are needed by Anim;
GLM, pybind11, and stb are header/source-only dependencies. CPython itself is
not built from source. The local Python 3.10 environment is created by the setup
script, which avoids rebuilding CPython and unrelated extension modules.

## Configure and build

From the repository root in PowerShell:

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/setup.ps1
cmake --preset windows-release
cmake --build --preset windows-release
```

The executable is `build/bin/Anim.exe`. The build directory also contains the
Python environment and copied application resources. Release builds do not
generate PDB files.

To launch from the repository root:

```powershell
& .\build\bin\Anim.exe
```

The older motion-capture notebooks and their sample files are kept in
`playground/mocap`. Run Jupyter with that directory as its working directory
when using relative paths inside the notebooks.

## Dependencies

Third-party versions are pinned in the repository's submodule entries. Their
source builds are configured with tests, examples, documentation, and optional
command-line tools disabled. Runtime Python packages are pinned in
`scripts/requirements-runtime.txt` and installed into `build/bin/python`.

If CMake cannot locate Python, remove only the generated `build` directory and
rerun the setup and configure commands. Do not delete or move the source folders
under `external`.
