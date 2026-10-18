import { readFileSync, writeFileSync } from "node:fs";
import { fileURLToPath } from "node:url";
import path from "node:path";

const portDirectory = path.dirname(fileURLToPath(import.meta.url));
const repositoryRoot = path.resolve(portDirectory, "../..");
const manifestPath = path.join(portDirectory, "vcpkg.json");
const configPath = path.join(repositoryRoot, "project-config.env");
const config = readFileSync(configPath, "utf8");
const version = config
  .split(/\r?\n/)
  .map((line) => line.match(/^\s*kkv_ver\s*=\s*(.*?)\s*$/))
  .find(Boolean)?.[1];

if (!version) {
  throw new Error(`kkv_ver is not defined in ${configPath}`);
}

const manifest = JSON.parse(readFileSync(manifestPath, "utf8"));
manifest["version-string"] = version;
writeFileSync(manifestPath, `${JSON.stringify(manifest, null, 2)}\n`);