// Build-time note index — the data behind the generated /timeline and /graph
// pages. Walks the knowledge base the same way sidebar.ts does (problems/
// categories → note dirs, notes/ recursed) and adds what those pages need:
// a route, a category/group and a date. Node-only: imported by the `.data.ts`
// loaders, never by the browser.
//
// Dates are file mtimes, repaired from the committed `.mylearn/index/latest.json`
// snapshot: a fresh `git clone` (what CI does) stamps every file with the
// checkout time, so a bare stat would collapse the whole timeline onto one day.
// The snapshot holds the mtime + sha256 the CLI recorded, so when the disk mtime
// disagrees with it and the content still hashes the same, the snapshot wins —
// a real edit changes the hash and keeps its (fresh) disk mtime.

import fs from "node:fs";
import path from "node:path";
import crypto from "node:crypto";

export interface NoteEntry {
    /** absolute path on disk */
    file: string;
    /** knowledge-base relative, posix style */
    rel: string;
    kind: "problem" | "note";
    /** problem category, or the notes/ top-level dir ("notes" for loose files) */
    group: string;
    title: string;
    /** site route, base-less ("/problems/luogu/P2748 …/") */
    url: string;
}

interface SnapshotFile {
    mtimeMs: number;
    hash: string;
}

/** snapshot key for a file: problems/ entries are stored without their prefix
 *  (legacy, see src/maintain/watch.ts) and notes/ entries with it */
function snapshotKey(entry: NoteEntry): string {
    return entry.rel.startsWith("problems/") ? entry.rel.slice("problems/".length) : entry.rel;
}

/**
 * Date of a note, in ms. Disk mtime, unless it looks rewritten by a checkout
 * and the recorded snapshot mtime still matches the file's content hash.
 */
export function noteDate(root: string, entry: NoteEntry, snapshot: Snapshot): number {
    const disk = fs.statSync(entry.file).mtimeMs;
    const recorded = snapshot.files[snapshotKey(entry)];
    if (!recorded) return disk; // never watched — nothing better to go on
    if (Math.abs(disk - recorded.mtimeMs) < 1000) return disk; // agree
    const hash = crypto.createHash("sha256").update(fs.readFileSync(entry.file)).digest("hex");
    return hash === recorded.hash ? recorded.mtimeMs : disk;
}

export interface Snapshot {
    updatedAt?: number;
    files: Record<string, SnapshotFile>;
}

export function loadSnapshot(root: string): Snapshot {
    const file = path.join(root, ".mylearn", "index", "latest.json");
    try {
        const parsed = JSON.parse(fs.readFileSync(file, "utf-8"));
        return { updatedAt: parsed.updatedAt, files: parsed.files ?? {} };
    } catch {
        // no snapshot (or unreadable) — dates fall back to the disk
        return { files: {} };
    }
}

/** source file → route: index.md/problem.md resolve to their dir ("/x/y/") */
export function toRoute(root: string, file: string): string {
    let rel = path
        .relative(root, file)
        .replaceAll(path.sep, "/")
        .replace(/\.md$/i, "");
    let isDir = false;
    for (const suffix of ["/index", "/problem"]) {
        if (rel.endsWith(suffix)) {
            rel = rel.slice(0, -suffix.length);
            isDir = true;
            break;
        }
    }
    return "/" + rel + (isDir ? "/" : "");
}

/** frontmatter `title:` (regex, not a parser — same as the sidebar walker) */
export function readTitle(file: string, fallback: string): string {
    try {
        const head = fs.readFileSync(file, "utf-8").slice(0, 400);
        const m = /^title:\s*(.+)$/m.exec(head);
        if (m) return m[1].trim().replace(/^["']|["']$/g, "");
    } catch {
        // unreadable file — fall back to the name
    }
    return fallback;
}

/** markdown body without the frontmatter block */
export function readBody(file: string): string {
    const text = fs.readFileSync(file, "utf-8");
    const m = /^---\r?\n[\s\S]*?\r?\n---\r?\n?/.exec(text);
    return m ? text.slice(m[0].length) : text;
}

function mdFiles(dir: string): string[] {
    return fs
        .readdirSync(dir)
        .filter((e) => e.toLowerCase().endsWith(".md"))
        .sort();
}

/** a problem dir's note: problem.md, else the first .md (mirrors the sidebar) */
function problemNote(dir: string): string | undefined {
    const mds = mdFiles(dir);
    return mds.find((e) => e.toLowerCase() === "problem.md") ?? mds[0];
}

function walk(dir: string, rel: string, out: string[]): void {
    let entries: fs.Dirent[];
    try {
        entries = fs.readdirSync(dir, { withFileTypes: true });
    } catch {
        return;
    }
    for (const entry of entries) {
        // dot dirs (.remember/ and friends) are scaffolding, not notes
        if (entry.name.startsWith(".") || entry.name === "node_modules") continue;
        const abs = path.join(dir, entry.name);
        const childRel = rel ? `${rel}/${entry.name}` : entry.name;
        if (entry.isDirectory()) walk(abs, childRel, out);
        else if (entry.name.toLowerCase().endsWith(".md")) out.push(abs);
    }
}

/** every note in the knowledge base — problems first, then notes/, both sorted */
export function collectNotes(root: string): NoteEntry[] {
    const out: NoteEntry[] = [];

    const problems = path.join(root, "problems");
    if (fs.existsSync(problems)) {
        for (const category of fs.readdirSync(problems).sort()) {
            const catDir = path.join(problems, category);
            if (!fs.statSync(catDir).isDirectory()) continue;
            for (const name of fs.readdirSync(catDir).sort()) {
                const dir = path.join(catDir, name);
                if (!fs.statSync(dir).isDirectory()) continue;
                const note = problemNote(dir);
                if (!note) continue;
                const file = path.join(dir, note);
                out.push({
                    file,
                    rel: path.relative(root, file).replaceAll(path.sep, "/"),
                    kind: "problem",
                    group: category,
                    title: readTitle(file, name),
                    url: toRoute(root, file),
                });
            }
        }
    }

    const notes = path.join(root, "notes");
    if (fs.existsSync(notes)) {
        const files: string[] = [];
        walk(notes, "notes", files);
        for (const file of files.sort()) {
            const rel = path.relative(root, file).replaceAll(path.sep, "/");
            const rest = rel.slice("notes/".length).split("/");
            // an index.md stands for its directory, so it falls back to its name
            const name = path.basename(file, ".md");
            out.push({
                file,
                rel,
                kind: "note",
                group: rest.length > 1 ? rest[0] : "notes",
                title: readTitle(file, name === "index" ? path.basename(path.dirname(file)) : name),
                url: toRoute(root, file),
            });
        }
    }

    return out;
}
