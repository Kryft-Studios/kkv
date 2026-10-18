@echo off
setlocal

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo Could not find vswhere.exe to locate Visual Studio Build Tools.
  exit /b 1
)

for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
if not defined VSINSTALL (
  echo Could not find a Visual Studio installation with x64 C++ build tools.
  exit /b 1
)

where code >nul 2>nul
if errorlevel 1 (
  echo The VS Code command-line launcher, code, is not on PATH.
  exit /b 1
)

powershell -NoProfile -Command "if (Get-Process -Name Code -ErrorAction SilentlyContinue) { exit 1 }"
if not errorlevel 1 goto :launch
echo Close all VS Code windows before using this launcher so clangd inherits the compiler environment.
exit /b 1

:launch
call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
  echo Failed to initialize the x64 Visual Studio C++ environment.
  exit /b 1
)

code --new-window "%~dp0.."
