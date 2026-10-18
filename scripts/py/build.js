import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const repositoryRoot = path.resolve(
  path.dirname(fileURLToPath(import.meta.url)),
  "../..",
);

console.log("Building the native Python extension");
execFileSync("cmake", ["--build", "--preset", "python"], {
  cwd: repositoryRoot,
  stdio: "inherit",
});
console.log("Python build complete");