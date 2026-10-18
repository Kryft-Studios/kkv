#!/usr/bin/env node
import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const repositoryRoot = path.resolve(
  path.dirname(fileURLToPath(import.meta.url)),
  "../..",
);

console.log("Building the native preset");
execFileSync("cmake", ["--build", "--preset", "default"], {
  cwd: repositoryRoot,
  stdio: "inherit",
});

console.log("Installing the native package with vcpkg");
execFileSync(
  "node",
  [path.join(repositoryRoot, "vcpkg-overlay", "kkv", "create-vcpkg.json.js")],
  { cwd: repositoryRoot, stdio: "inherit" },
);
execFileSync(
  "vcpkg",
  [
    "install",
    "kkv",
    `--overlay-ports=${path.join(repositoryRoot, "vcpkg-overlay")}`,
  ],
  { cwd: repositoryRoot, stdio: "inherit" },
);

console.log("Creating the native Conan package");
execFileSync(
  "conan",
  [
    "create",
    ".",
    "--profile:host=default",
    "--profile:build=default",
    "--settings:host=compiler.cppstd=20",
    "--settings:build=compiler.cppstd=20",
    "--build=missing",
    "-c",
    "tools.build:compiler_executables={'cpp':'clang-cl'}",
  ],
  { cwd: repositoryRoot, stdio: "inherit" },
);
console.log("Native packages created");