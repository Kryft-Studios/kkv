#!/usr/bin/env node
import { build } from "esbuild";
import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import { readFile, writeFile } from "node:fs/promises";
import path from "node:path";
import { minify } from "terser";

const thisDir = path.dirname(fileURLToPath(import.meta.url));
const emDirectory = path.resolve(thisDir, "../../em")
const repositoryRoot = path.resolve(thisDir, "../..");

console.log("Building the Emscripten target");
execFileSync("cmake", ["--build", "--preset", "wasm"], {
  cwd: repositoryRoot,
  stdio: "inherit",
});

console.log("Bundling the generated WebAssembly module");
await build({
  platform: "neutral",
  entryPoints: ["kkv.js"],
  allowOverwrite: true,
  absWorkingDir: emDirectory,
  outfile: "kkv.js",
  format: "esm",
});

console.log("Minifying the generated JavaScript module");
const minified = await minify(
  await readFile(path.join(emDirectory, "kkv.js"), "utf8"),
);
if (!minified.code) {
  throw new Error("Terser did not produce JavaScript output for em/kkv.js");
}
await writeFile(path.join(emDirectory, "kkv.js"), minified.code);
console.log("WASM build complete");