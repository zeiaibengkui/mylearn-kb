// The two heavy browser libraries load from a CDN instead of the bundle.
// Rollup had to pull all of mermaid's ~60 MB chunk graph through the
// tree-shaker on every build — ~17s (cytoscape ~4s) — for bytes that were
// always fetched lazily anyway. The pinned esm builds come from
// fastly.jsdelivr.net (jsdelivr's Fastly endpoint: CORS-enabled, cached
// immutably, reachable from mainland China); mermaid lazy-loads its
// per-diagram chunks relative to this URL, exactly as it did when bundled.
//
// Versions mirror the devDependencies in package.json — those stay installed
// for the TypeScript types the components annotate against (the browser never
// sees the installed copies). Bump both together.
export const MERMAID_URL = "https://fastly.jsdelivr.net/npm/mermaid@12.0.0/dist/mermaid.esm.min.mjs";
export const CYTOSCAPE_URL = "https://fastly.jsdelivr.net/npm/cytoscape@3.34.3/dist/cytoscape.esm.min.mjs";
