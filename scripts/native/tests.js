import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const repositoryRoot = path.resolve(
  path.dirname(fileURLToPath(import.meta.url)),
  "../..",
);

console.log("Building native tests");
execFileSync("cmake", ["--build", "--preset", "default"], {
  cwd: repositoryRoot,
  stdio: "inherit",
});
console.log("Running native tests");
execFileSync("ctest", ["--preset", "default"], {
  cwd: repositoryRoot,
  stdio: "inherit",
});
console.log("Native tests complete");