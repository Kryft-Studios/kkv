# KKV

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-00599C?logo=c%2B%2B&logoColor=white" alt="C++20" />
  <img src="https://img.shields.io/badge/CMake-3.21%2B-064F8C?logo=cmake&logoColor=white" alt="CMake 3.21+" />
  <img src="https://img.shields.io/badge/Conan-2-CA4A1D?logo=conan&logoColor=white" alt="Conan 2" />
  <img src="https://img.shields.io/badge/Windows-MSVC%20%2B%20clang-cl-0078D4?logo=microsoft&logoColor=white" alt="Windows" />
  <img src="https://img.shields.io/badge/Emscripten-WASM-6E4AAB?logo=webassembly&logoColor=white" alt="Emscripten" />
</p>

### MADE WITH LOVE BY KRYFT STUDIOS
###### and many sacrificed nights.

Kryft Key-Value is a C++20 key-value parsing and serialization library with native, WebAssembly, and optional Python bindings.

Version: 0.1.0

## Highlights

- Native static library target with CMake and Conan 2 integration
- Windows-first native toolchain using `clang-cl`
- Emscripten/WASM build output under `em/`
- Optional Python extension with `pybind11`
- Native and JS test coverage, formatting, and documentation targets

## Quick start

### Native build

```powershell
# from the repository root
conan install . --output-folder=build/conan --profile:host=default --profile:build=default --settings:host=build_type=Debug --settings:host=compiler.cppstd=20 --settings:build=compiler.cppstd=20 --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja -c "tools.build:compiler_executables={'cpp':'clang-cl'}"
cmake --preset default
cmake --build --preset default
ctest --preset default
```

### WebAssembly build

```powershell
conan install . --output-folder=build-wasm/conan --profile:host=profiles/wasm --profile:build=default --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja
cmake --preset wasm
cmake --build --preset wasm
```

### Python extension

```powershell
$python = (Get-Command python).Source
& $python -m pip install pybind11
.
\py\configure.cmd
.
\py\build.cmd
```

## Project layout

- `src/` — native C++ implementation
- `include/` — public headers
- `private/` — internal/private headers
- `examples/` — native example programs
- `tests/` — native test suite
- `emscripten/` — Emscripten bindings
- `python/` — Python extension source
- `build/`, `build-wasm/`, `build-python/` — generated build outputs
- `em/` — generated WebAssembly module outputs

## Developer workflow

The project uses CMake presets and Conan output folders per build configuration. For the complete setup, dependency commands, wrapper scripts, formatting, packaging, and Python build notes, see:

- [CONTRIBUTING.md](CONTRIBUTING.md)
- [docs/IDE.md](docs/IDE.md)

## Editor and clangd setup

Windows users should launch VS Code through the repository helper so clangd inherits the Visual Studio developer environment:

```powershell
.\native\vscode.cmd
```

That helper initializes the x64 Visual Studio environment before launching VS Code, which prevents missing standard library errors such as `<vector>`, `<stdexcept>`, and `<iostream>` in clangd.

## Documentation and tooling

- Doxygen documentation is generated through the native preset
- Formatting uses `.clang-format` and `.editorconfig`
- Sanitizers include address and undefined behaviour checks
- JavaScript tests live under `tests-js/`

## License

The repository is distributed under the project license declared in `LICENSE`.

If you are preparing a release or package distribution, make sure the license text is complete and matches your intended distribution terms before publishing.
