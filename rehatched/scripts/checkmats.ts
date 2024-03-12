import { readdirSync, readFileSync } from "fs";
import { extname } from "path";
import { resolve, join } from "path/posix";

const path = resolve("../../Build/Debug");
// const path = "/Applications/Games/Penumbra Black Plague.app/Contents/Resources/game";

let matCount = 0;
const matFiles: Record<string, string[]> = {};
const dep: Record<string, string[]> = {};
const fileTypes: Record<string, number> = {};
const listing: Record<string, { n: string, kb: number }[]> = {};

function doFunnyStuffWithFile(fileName: string, fullPath: string) {
	matCount++;
	const text = readFileSync(fullPath, { encoding: "utf-8" });
	const match = text.match(/Wrap="(\w*)"/);
	if (match) {
		const renderType = match[1].toLowerCase();
		if (! (renderType in matFiles)) {
			matFiles[renderType] = [];
		}
		matFiles[renderType].push(fileName);
	}
	else {
		console.error("  Can't match renderType in " + fileName);
	}
}

function scan(dir: string) {
	const files = readdirSync(dir, { withFileTypes: true });
	for (const f of files) {
		const fullPath = join(dir, f.name);
		if (f.isDirectory()) {
			// console.info("DIR", f.name);
			scan(join(dir, f.name));
		}
		else if(f.isFile()) {
			matCount++;
			const ext = extname(f.name).toLowerCase();
			if (ext.length == 0) continue;
			// if (! (ext in fileTypes)) { fileTypes[ext] = 0; listing[ext] = []; }
			// fileTypes[ext]++;
			// listing[ext].push({ n: f.name, kb: Bun.file(fullPath).size });
			
			if (ext === ".mat") {
				doFunnyStuffWithFile(f.name, fullPath);
			}
		}
	}
}

scan(path);

console.info("====================================================================");
console.info("Files: ", matCount);
// console.info(JSON.stringify(fileTypes, undefined, 4));
console.info("====================================================================");
// console.info("Listing");
// for (const k in listing) { listing[k].sort((a,b) => a.kb-b.kb); }
// console.info(JSON.stringify(listing, undefined, 4));
// console.info(JSON.stringify(dep, undefined, 4));
// console.info(dep.true.length + dep.false.length)
// console.info(Object.values(matFiles).reduce((a, b) => a + b.length, 0));
// console.info(Object.keys(matFiles).sort());
console.info(matFiles["clamptoedge"]);
for (const m in matFiles) {
	matFiles[m] = matFiles[m].length + "";
}
console.info(matFiles);
