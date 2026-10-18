import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const repositoryRoot = path.resolve(
  path.dirname(fileURLToPath(import.meta.url)),
  "../..",
);

if (!process.env.EMSDK) {
  throw new Error(
    "EMSDK is not set. Activate the Emscripten SDK before configuring WASM.",
  );
}

console.log("Installing Emscripten Conan dependencies");
execFileSync(
  "conan",
  [
    "install",
    ".",
    "--output-folder=build-wasm/conan",
    "--profile:host=toolchain/profiles/wasm",
    "--profile:build=default",
    "--build=missing",
    "-c",
    "tools.cmake.cmaketoolchain:generator=Ninja",
  ],
  { cwd: repositoryRoot, stdio: "inherit" },
);

console.log("Configuring the Emscripten preset");
execFileSync("cmake", ["--preset", "wasm"], {
  cwd: repositoryRoot,
  stdio: "inherit",
});
console.log("WASM preset configured");
