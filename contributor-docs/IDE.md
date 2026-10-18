# IDE and clangd setup

This project is configured around CMake presets and generated compile databases. The goal is for native sources to use native Windows compiler flags, for Python sources to use Python/pybind11 include paths, and for Emscripten sources to use the WebAssembly toolchain.

## Recommended editor setup

Use VS Code with:

- `llvm-vs-code-extensions.vscode-clangd`
- `ms-vscode.cmake-tools`
- `twxs.cmake`

The repository already contains editor config in `.vscode/settings.json` and `.vscode/extensions.json`.

## Native Windows setup for clangd

On Windows, clangd must run with the same MSVC/Windows SDK environment as the native compiler. If it reports errors such as:

- `Failed to get an entry for resolved path '' from include <vector>`
- `Failed to get an entry for resolved path '' from include <stdexcept>`
- `Failed to get an entry for resolved path '' from include <iostream>`

then VS Code was probably launched without the Visual Studio developer environment.

To fix it:

1. Fully close all VS Code windows.
2. Open a PowerShell or Command Prompt in the repository root.
3. Run:

```powershell
.\native\vscode.cmd
```

This script finds the installed Visual Studio Build Tools, initializes `vcvars64.bat`, and launches VS Code in the correct environment. After that, clangd can discover the standard library headers provided by MSVC and the Windows SDK.

## CMake presets and compile databases

The CMake presets write compile databases to these folders:

- `build/compile_commands.json` — native sources
- `build-wasm/compile_commands.json` — Emscripten sources
- `build-python/compile_commands.json` — Python extension sources

The project also includes a `.clangd` file that maps source paths to the correct compilation database. Keep the build directories in place so clangd picks up the right flags automatically.

## Switching presets in VS Code

Use the CMake Tools UI to choose the active preset:

- `default` for native Windows builds
- `wasm` for Emscripten builds
- `python` for the native Python extension build

The default VS Code configuration targets `default`, but you can switch to `wasm` or `python` when working in those trees.

## Troubleshooting checklist

- Make sure the repository root is the active workspace folder
- Open the project from the repo root, not from a subfolder
- Ensure the Visual Studio C++ tools are installed
- Ensure `clang-cl` is present and on `PATH`
- Ensure `EMSDK` is exported before using the WebAssembly preset
- Ensure `KKV_PYTHON` is set when using the Python helper if a non-default interpreter is desired

## Related docs

- [../CONTRIBUTING.md](../CONTRIBUTING.md)
- [../README.md](../README.md)
