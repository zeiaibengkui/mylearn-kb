// Data for /timeline: every note, newest first. Rendered server-side (the
// dates and titles are in the HTML), so the page works without JS.

import { defineLoader } from "vitepress";
import { collectNotes, loadSnapshot, noteDate } from "./notes.ts";

export interface TimelineEntry {
    title: string;
    url: string;
    kind: "problem" | "note";
    group: string;
    /** ms since epoch */
    date: number;
}

declare const data: { entries: TimelineEntry[] };
export { data };

export default defineLoader({
    watch: ["problems/**/*.md", "notes/**/*.md", ".mylearn/index/latest.json"],
    load(): { entries: TimelineEntry[] } {
        const root = process.cwd();
        const snapshot = loadSnapshot(root);
        const entries = collectNotes(root)
            .map((entry) => ({
                title: entry.title,
                url: entry.url,
                kind: entry.kind,
                group: entry.group,
                date: noteDate(root, entry, snapshot),
            }))
            .sort((a, b) => b.date - a.date || a.title.localeCompare(b.title));
        return { entries };
    },
});
