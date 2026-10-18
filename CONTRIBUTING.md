# Contributing to KKV

KKV (Kryft Key-Value) is a C++20 library with an Emscripten/WebAssembly build. Contributions are welcome!

Please also read the [Kryft Studios Universal Contributing Guide](https://github.com/Kryft-Studios/.github/blob/main/CONTRIBUTING.md).
# A quick start, no yap.
```sh
cd scripts
native\vscode
native\configure
em\configure
py\configure
```
## Repository structure

| Path | Purpose |
|---|---|
| `src/` | C++ implementation |
| `include/kkv/` | Public C++ headers |
| `private/kkv/` | Internal implementation headers |
| `examples/` | Native C++ examples |
| `tests/` | Native doctest test suite |
| `emscripten/` | Emscripten bindings |
| `tests-js/` | JavaScript tests for the generated WebAssembly module |
| `scripts/`  | Scripts for helping you do stuff quick without memorising |
| `toolchain/` | Toolchain related stuff, like conan profiles |
| `.github/workflows/` | CI, formatting, and documentation workflows |

Important root-level files include `CMakeLists.txt`, `CMakePresets.json`, `conanfile.py`, and `.clang-format`.

## Prerequisites

Install the following tools:

- CMake 3.21 or newer
- Ninja
- A C++20-capable compiler, including `clang-cl` for the configured native preset
- Conan 2
- Doxygen
- `clang-format`
- Node.js for JavaScript tests
- An installed and activated Emscripten SDK for WebAssembly builds

Use a terminal where the required compilers and tools are available on `PATH`.
For native `clang-cl` builds, use an x64 Visual Studio Developer PowerShell/Command Prompt so the MSVC and Windows SDK headers and libraries are configured.

## Configure dependencies

The CMake presets expect Conan-generated files in `build/conan/`, `build-wasm/conan/`, and `build-python/conan/`. Install dependencies into the matching output folder before configuring that preset.

The detected Conan `default` profile is the native Windows MSVC profile. Use C++20 settings to satisfy the recipe while configuring the native build with `clang-cl`:

```sh
conan install . --output-folder=build/conan --profile:host=default --profile:build=default --settings:host=build_type=Debug --settings:host=compiler.cppstd=20 --settings:build=compiler.cppstd=20 --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja -c "tools.build:compiler_executables={'cpp':'clang-cl'}"
```

Install dependencies for WebAssembly using the repository's Emscripten host profile:

```sh
conan install . --output-folder=build-wasm/conan --profile:host=toolchain/profiles/wasm --profile:build=default --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja
```

The WebAssembly profile must reference a valid Emscripten installation. Its toolchain configuration currently requires attention if you are using a different machine or SDK location.

## Native development

Configure and build the native target:

```sh
cmake --preset default
cmake --build --preset default
```

Run the native tests:

```sh
ctest --preset default
```

Build the Doxygen documentation:

```sh
cmake --build --preset default --target documentation
```

Native test dependencies include doctest, which is fetched through CMake FetchContent.

## WebAssembly development

Ensure the Emscripten SDK is installed and activated, then configure and build:

```sh
cmake --preset wasm
cmake --build --preset wasm
```

The generated module is written under `em/`.

To run the existing JavaScript test script after building the module:

```sh
node tests-js/tryReadAsNum.js
```

This command runs that particular test script; run any additional JavaScript tests relevant to your changes as well.

Native CTest is disabled for Emscripten builds.

## Tests and formatting

- Add native test cases to `tests/test.cpp` using doctest.
- Add JavaScript checks under `tests-js/` when changing or extending the generated WebAssembly API.
- Run the relevant build and tests for every change.
- Rebuild the documentation when modifying public API documentation.

The project uses `.clang-format` and `.editorconfig` to define formatting conventions.

Run the configured CMake formatting target:

```sh
cmake --build --preset default --target format
```

CI checks C++ formatting in `src/` and `include/` using `clang-format --dry-run --Werror`.

Follow the repository's existing naming, documentation, and implementation conventions. Avoid unrelated changes in pull requests.

## Python bindings

The Python preset is a native Windows build. Install pybind11 in the same interpreter passed to CMake, then generate its dedicated Conan files:

```powershell
$python = (Get-Command python).Source
& $python -m pip install pybind11
conan install . --output-folder=build-python/conan --profile:host=default --profile:build=default --settings:host=compiler.cppstd=20 --settings:build=compiler.cppstd=20 --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja -c "tools.build:compiler_executables={'cpp':'clang-cl'}"
cmake --preset python "-DPython_EXECUTABLE=$python"
cmake --build --preset python
```

On Windows, from the repository root, run the helper with `.\py\configure.cmd` (or `node py/configure.js`); `py/configure` invokes the Python launcher and is not a valid way to run this JavaScript helper. Set `KKV_PYTHON` to choose a specific Python executable. The helper verifies pybind11, installs Conan dependencies first, then configures CMake with that interpreter.

The configure step uses that interpreter to locate pybind11's CMake package directory with `python -m pybind11 --cmakedir`. The `_kkv` extension is emitted as a native `.pyd` module under `build-python/`. To smoke-test it:

```powershell
& $python -c "import sys; sys.path.insert(0, 'build-python'); import _kkv; assert _kkv.tryParseAsNum() == 'ok'; print(_kkv.tryParseAsNum())"
```

clangd uses `build/compile_commands.json` for native sources, `build-wasm/compile_commands.json` for `emscripten/`, and `build-python/compile_commands.json` for `python/`. The Python entry and its Python/pybind11 include paths are available after the one-time Python Conan install and preset configure above; Python bindings remain disabled in the ordinary native and WebAssembly presets.

On Windows, clangd must inherit the Visual Studio compiler environment to discover the MSVC standard library and Windows SDK headers. If `clangd` reports `Failed to get an entry for resolved path '' from include <vector>` or other missing standard headers, it is almost always because VS Code was launched without `vcvars64.bat` in the process environment. Close all VS Code windows first, then start the workspace from the repository root with `.\native\vscode.cmd`. This launcher initializes the x64 Visual Studio environment before launching VS Code and keeps the native CMake preset aligned with the proper compiler include paths.

Do not start the repo from an already-open VS Code instance or from a regular shell that has not run the Visual Studio developer environment. The recommended VS Code extensions provide clangd diagnostics and CMake preset/configure/build/test integration; `.vscode/settings.json` selects the native preset by default. Select `wasm` or `python` in the CMake status bar when working with those build configurations.

## Build helper commands

The JavaScript helpers and their `.cmd`, `.ps1`, and `.sh` wrappers resolve paths from their own locations, so invoke them from the repository root or another working directory:

```powershell
.\native\configure.cmd
.\native\tests.cmd
.\em\configure.cmd
.\em\build.cmd
.\em\test.cmd
.\em\ci.cmd
.\py\configure.cmd
.\py\build.cmd
```

Run native commands from an x64 Visual Studio Developer shell. Set `KKV_PYTHON` to select the Python interpreter for the Python helper. Emscripten commands require an activated SDK with `EMSDK` set. `native\package.cmd` additionally requires vcpkg.

## Generated files

Do not commit build products, generated dependencies, or other generated output unless the project explicitly tracks them.

Existing ignore rules cover build directories such as `build/` and `build-wasm/`, `node_modules/`, `dist/`, compiler outputs, CTest output under `tests/Testing/`, and generated Emscripten files including `em/kkv.*` and `em/generated.d.ts`.

Check `.gitignore` before adding new generated files or changing existing ignore rules.

## Issues and pull requests

### Reporting an issue

Include:

- A clear description of the problem
- Expected and actual behavior
- Steps to reproduce the issue
- The affected platform and build preset
- Relevant compiler, CMake, or test output

### Submitting a pull request

Include:

- A summary of the change and its motivation
- A related issue, if applicable
- The builds, tests, and formatting checks you ran
- Documentation or tests accompanying changes to behavior or public APIs

Keep changes focused, and avoid committing unrelated generated files or local build configuration.

## Known setup caveats

- Each preset must use its own Conan output directory (`build/conan/`, `build-wasm/conan/`, or `build-python/conan/`), and `rang_DIR` must resolve from that same directory.
- `toolchain/profiles/wasm` resolves the Emscripten toolchain through `EMSDK`; activate the SDK so that environment variable is set before installing dependencies.
- CI should install Conan dependencies before configuring presets that require Conan-generated files and the `rang` package.

These caveats should be removed or updated as the setup is made fully reproducible.