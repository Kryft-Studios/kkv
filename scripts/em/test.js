import { globSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import path from "node:path";

const thisDir = path.dirname(fileURLToPath(import.meta.url));
const repositoryRoot = path.resolve(thisDir, "../..");
const testFiles = globSync(path.join(repositoryRoot, "tests-js", "*.js"));

console.log("Running JavaScript tests");
let failed = false;
for (const file of testFiles) {
  console.log(`Running test: ${path.relative(repositoryRoot, file)}`);
  const result = spawnSync(process.execPath, [file], {
    cwd: repositoryRoot,
    stdio: "inherit",
  });

  if (result.error) {
    console.error(`Could not start ${file}: ${result.error.message}`);
    failed = true;
  } else if (result.status !== 0) {
    failed = true;
  }
}

if (failed) {
  process.exitCode = 1;
}