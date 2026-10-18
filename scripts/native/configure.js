import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const repositoryRoot = path.resolve(
  path.dirname(fileURLToPath(import.meta.url)),
  "../..",
);

console.log("Installing native Conan dependencies");
execFileSync(
  "conan",
  [
    "install",
    ".",
    "--output-folder=build/conan",
    "--profile:host=default",
    "--profile:build=default",
    "--settings:host=build_type=Debug",
    "--settings:host=compiler.cppstd=20",
    "--settings:build=compiler.cppstd=20",
    "--build=missing",
    "-c",
    "tools.cmake.cmaketoolchain:generator=Ninja",
    "-c",
    "tools.build:compiler_executables={'cpp':'clang-cl'}",
  ],
  { cwd: repositoryRoot, stdio: "inherit" },
);

console.log("Configuring the native preset");
execFileSync("cmake", ["--preset", "default"], {
  cwd: repositoryRoot,
  stdio: "inherit",
});
console.log("Native preset configured");