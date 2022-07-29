import { readdirSync, readFileSync } from "fs";
import { extname } from "path";
import { resolve, join } from "path/posix";

const path = resolve("../../Build/Debug");

let matCount = 0;
const matFiles: Record<string, string[]> = {};

function scan(dir: string) {
	const files = readdirSync(dir, { withFileTypes: true });
	for (const f of files) {
		if (f.isDirectory()) {
			// console.info("DIR", f.name);
			scan(join(dir, f.name));
		}
		else if(f.isFile()) {
			const ext = extname(f.name).toLowerCase();
			if (ext === ".mat") {
				matCount++;
				// console.info("  F", f.name);
				const text = readFileSync(join(dir, f.name), { encoding: "utf-8" });
				const match = text.match(/<Main\s+Type="(\w+)"/);
				if (match) {
					const renderType = match[1].toLowerCase();
					if (! (renderType in matFiles)) {
						matFiles[renderType] = [];
					}
					matFiles[renderType].push(f.name);
				}
				else {
					console.error("  Can't match renderType");
				}
			}
		}
	}
}

scan(path);

console.info("====================================================================");
console.info("Mat files: ", matCount);
console.info(JSON.stringify(matFiles, undefined, 4));
console.info(Object.values(matFiles).reduce((a, b) => a + b.length, 0));
