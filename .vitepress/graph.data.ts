// Data for /graph: notes as nodes, references between them as edges.
//
//   group edges  — a category (or a notes/ top-level dir) points at its notes,
//                  so the graph reads as clusters instead of loose dots
//   ref edges    — one note mentions another: a Luogu pid in the statement
//                  ("见 [P3049](https://www.luogu.com.cn/problem/P3049)") or a
//                  relative markdown link. Pids with no note here become
//                  `external` nodes that link back to Luogu.
//
// Composed at build time (Node), rendered client-side by RelationGraph.vue.

import fs from "node:fs";
import path from "node:path";
import { defineLoader } from "vitepress";
import { collectNotes, readBody, type NoteEntry } from "./notes.ts";

export interface GraphNode {
    id: string;
    label: string;
    kind: "problem" | "note" | "category" | "external";
    /** hub name for categories, or the note's own group */
    group: string;
    /** site route (base-less); external nodes carry an absolute Luogu URL */
    url?: string;
    /** degree — drives node size in the layout */
    degree: number;
}

export interface GraphEdge {
    source: string;
    target: string;
    kind: "group" | "ref";
}

declare const data: { nodes: GraphNode[]; edges: GraphEdge[] };
export { data };

const luoguProblemRe = /luogu\.com\.cn\/problem\/([A-Za-z0-9_]+)/g;
const relativeLinkRe = /\]\((?!https?:|mailto:|#)([^)\s]+)\)/g;

/** the note's own pid, from its title ("P2748 [USACO16OPEN] …" → "P2748").
 *  No pid shape check: AT_dp_u has no digit, and only an exact match against a
 *  pid taken out of a Luogu URL can ever be looked up in this map anyway. */
function ownPid(entry: NoteEntry): string {
    return entry.title.split(/\s+/)[0];
}

/** every markdown file in the note's own directory: the explanation and the
 *  archived solutions refer to other problems just as often as the statement */
function noteText(file: string): string {
    const dir = path.dirname(file);
    return fs
        .readdirSync(dir)
        .filter((e) => e.toLowerCase().endsWith(".md"))
        .sort()
        .map((e) => readBody(path.join(dir, e)))
        .join("\n\n");
}

/** pids referenced in a body, plus its relative links; fenced blocks are
 *  sample data, not prose, so they never count as a reference */
function references(body: string): { pids: string[]; links: string[] } {
    const clean = body.replace(/```[\s\S]*?```/g, "");
    const pids = [...clean.matchAll(luoguProblemRe)].map((m) => m[1]);
    const links = [...clean.matchAll(relativeLinkRe)].map((m) => m[1]);
    return { pids: [...new Set(pids)], links: [...new Set(links)] };
}

/** absolute path a relative markdown link points at ("" when it cannot be
 *  read as a path — a broken link must not fail the build) */
function resolveLink(from: string, link: string): string {
    try {
        return path.resolve(path.dirname(from), decodeURIComponent(link.replace(/[?#].*$/, "")));
    } catch {
        return "";
    }
}

export default defineLoader({
    watch: ["problems/**/*.md", "notes/**/*.md"],
    load(): { nodes: GraphNode[]; edges: GraphEdge[] } {
        const root = process.cwd();
        const entries = collectNotes(root);
        const nodes = new Map<string, GraphNode>();
        const edges: GraphEdge[] = [];

        const nodeId = (entry: NoteEntry) => `note:${entry.rel}`;
        const addNode = (node: GraphNode) => {
            if (!nodes.has(node.id)) nodes.set(node.id, node);
        };
        const addEdge = (source: string, target: string, kind: GraphEdge["kind"]) => {
            if (source !== target) edges.push({ source, target, kind });
        };

        // every note, plus a hub per category / notes/ top-level dir
        for (const entry of entries) {
            addNode({
                id: nodeId(entry),
                label: entry.title,
                kind: entry.kind,
                group: entry.group,
                url: entry.url,
                degree: 0,
            });
            addNode({
                id: `group:${entry.group}`,
                label: entry.group,
                kind: "category",
                group: entry.group,
                degree: 0,
            });
            addEdge(`group:${entry.group}`, nodeId(entry), "group");
        }

        // pid → note, and every page a relative link can resolve to
        const byPid = new Map<string, NoteEntry>();
        const byFile = new Map<string, NoteEntry>();
        for (const entry of entries) {
            byPid.set(ownPid(entry).toLowerCase(), entry);
            byFile.set(entry.file, entry);
            // a note that *is* its directory (problem.md / index.md) owns the
            // dir route too, so ../foo/ and ../foo/index.md both resolve
            const base = path.basename(entry.file, ".md").toLowerCase();
            if (base === "problem" || base === "index") byFile.set(path.dirname(entry.file), entry);
        }

        for (const entry of entries) {
            const { pids, links } = references(noteText(entry.file));
            for (const pid of pids) {
                // the statement's own header link
                if (pid.toLowerCase() === ownPid(entry).toLowerCase()) continue;
                const target = byPid.get(pid.toLowerCase());
                if (target) {
                    addEdge(nodeId(entry), nodeId(target), "ref");
                } else {
                    const id = `ext:${pid}`;
                    addNode({
                        id,
                        label: pid,
                        kind: "external",
                        group: "external",
                        url: `https://www.luogu.com.cn/problem/${pid}`,
                        degree: 0,
                    });
                    addEdge(nodeId(entry), id, "ref");
                }
            }
            for (const link of links) {
                const target = byFile.get(resolveLink(entry.file, link));
                if (target) addEdge(nodeId(entry), nodeId(target), "ref");
            }
        }

        for (const edge of edges) {
            nodes.get(edge.source)!.degree++;
            nodes.get(edge.target)!.degree++;
        }

        return { nodes: [...nodes.values()], edges };
    },
});
