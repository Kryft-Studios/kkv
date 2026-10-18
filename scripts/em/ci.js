import { spawnSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const emDirectory = path.dirname(fileURLToPath(import.meta.url));
const repositoryRoot = path.resolve(emDirectory, "../..");

for (const script of ["configure.js", "build.js", "test.js"]) {
  console.log(`Running ${script}`);
  const result = spawnSync(
    process.execPath,
    [path.join(emDirectory, script)],
    { cwd: repositoryRoot, stdio: "inherit" },
  );
  if (result.error) {
    throw new Error(`Could not start ${script}: ${result.error.message}`);
  }
  if (result.status !== 0) {
    process.exit(result.status ?? 1);
  }
}

console.log("WASM CI complete");