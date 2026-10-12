$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$venvPython = Join-Path $repoRoot 'build\bin\python\Scripts\python.exe'

if (-not (Test-Path $venvPython)) {
    $pythonLauncher = Get-Command py -ErrorAction SilentlyContinue
    if ($pythonLauncher) {
        & py -3.10 -m venv (Join-Path $repoRoot 'build\bin\python')
    } else {
        $python310 = Get-Command python3.10, python -ErrorAction SilentlyContinue |
            Where-Object { (& $_.Source --version 2>&1) -match '^Python 3\.10\.' } |
            Select-Object -First 1
        if (-not $python310) {
            throw 'Python 3.10 is required. Install Python 3.10 x64 and rerun scripts/setup.ps1.'
        }
        & $python310.Source -m venv (Join-Path $repoRoot 'build\bin\python')
    }
}

if (-not (Test-Path $venvPython)) {
    throw "Could not create the project virtual environment at $venvPython"
}

foreach ($proxyVariable in @('HTTP_PROXY', 'HTTPS_PROXY', 'ALL_PROXY', 'GIT_HTTP_PROXY', 'GIT_HTTPS_PROXY')) {
    Remove-Item "Env:$proxyVariable" -ErrorAction SilentlyContinue
}

& $venvPython -m pip install --upgrade pip
& $venvPython -m pip install --no-deps mediapipe==0.10.14
& $venvPython -m pip install -r (Join-Path $repoRoot 'scripts\requirements-runtime.txt')
& $venvPython -c "import cv2, mediapipe, matplotlib, glm, redis; print('Runtime dependencies are ready.')"
