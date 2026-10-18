@echo off
setlocal

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
)

if defined VSINSTALL (
  call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul
)

set "CLANGD=%ProgramFiles%\LLVM\bin\clangd.exe"
if not exist "%CLANGD%" (
  echo clangd.exe was not found at "%CLANGD%".
  exit /b 1
)

"%CLANGD%" %*
