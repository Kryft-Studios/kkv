import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const scriptDirectory = path.dirname(fileURLToPath(import.meta.url));
const repositoryRoot = path.resolve(scriptDirectory, "../..");
const pythonCommand = process.env.KKV_PYTHON || process.env.PYTHON || "python";

function run(command, args) {
  execFileSync(command, args, {
    cwd: repositoryRoot,
    stdio: "inherit",
  });
}

let python;
try {
  python = execFileSync(
    pythonCommand,
    ["-c", "import sys; print(sys.executable)"],
    { cwd: repositoryRoot, encoding: "utf8" },
  ).trim();
} catch {
  console.error(`Unable to run the selected Python interpreter: ${pythonCommand}`);
  process.exit(1);
}

try {
  run(python, ["-c", "import pybind11"]);
} catch {
  console.error(
    `pybind11 is not installed for ${python}. Install it with: ` +
      `"${python}" -m pip install pybind11`,
  );
  process.exit(1);
}

console.log("Installing native Python-build Conan dependencies");
run("conan", [
  "install",
  ".",
  "--output-folder=build-python/conan",
  "--profile:host=default",
  "--profile:build=default",
  "--settings:host=compiler.cppstd=20",
  "--settings:build=compiler.cppstd=20",
  "--build=missing",
  "-c",
  "tools.cmake.cmaketoolchain:generator=Ninja",
  "-c",
  "tools.build:compiler_executables={'cpp':'clang-cl'}",
]);

console.log("Configuring the native Python preset");
run("cmake", [
  "--preset",
  "python",
  `-DPython_EXECUTABLE=${python}`,
]);

console.log("Python preset configured");
