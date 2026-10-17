$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot
$pythonPath = Join-Path $PSScriptRoot '.venv\Scripts\python.exe'
if (-not (Test-Path -LiteralPath $pythonPath)) {
    Write-Host '正在建立此工具專用的 Python 環境。需已安裝 Python 3.10 或更新版。'
    & py -3 -m venv (Join-Path $PSScriptRoot '.venv')
    if ($LASTEXITCODE -ne 0) { throw '無法建立 Python 環境，請先安裝 Python 3.10 或更新版。' }
}
& $pythonPath -c 'import sys; assert sys.version_info >= (3,10), "Python 3.10+ required"'
if ($LASTEXITCODE -ne 0) { throw '需要 Python 3.10 或更新版。' }
& $pythonPath -c 'import usb, libusb_package, Cryptodome, yaml'
if ($LASTEXITCODE -ne 0) {
    Write-Host '第一次使用正在安裝必要元件，需網路連線。'
    & $pythonPath -m pip install --requirement (Join-Path $PSScriptRoot 'requirements.txt')
    if ($LASTEXITCODE -ne 0) { throw '元件安裝失敗，尚未連線相機。' }
}
& $pythonPath (Join-Path $PSScriptRoot 'app.py') check-package
if ($LASTEXITCODE -ne 0) { throw '套件檢查失敗，尚未連線相機。' }
& $pythonPath (Join-Path $PSScriptRoot 'gui.py')
