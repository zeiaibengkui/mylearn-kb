<script setup lang="ts">
// Relation graph (../graph.data.ts) drawn with Cytoscape.
//
// Cytoscape is fetched from the CDN on mount (./vendor.ts), so it never runs
// during SSR and never enters any bundle — only the /graph page fetches it,
// once the container is on screen. Wheel zoom is off on purpose: the canvas
// sits in a scrolling document, so zooming is on the buttons instead.
import { onBeforeUnmount, onMounted, ref, watch } from "vue";
import { useData, withBase } from "vitepress";
import type { Core, ElementDefinition, Stylesheet } from "cytoscape";
import { data } from "../graph.data";
import { CYTOSCAPE_URL } from "./vendor.ts";

const host = ref<HTMLElement | null>(null);
const { isDark } = useData();
const showExternal = ref(true);
const failed = ref(false);

let cy: Core | undefined;
let observer: ResizeObserver | undefined;
let styles: Stylesheet[] = [];

/** the site palette, read live so the graph follows the theme + dark mode */
function palette() {
    const css = getComputedStyle(document.documentElement);
    const read = (name: string, fallback: string) => css.getPropertyValue(name).trim() || fallback;
    return {
        problem: read("--vp-c-brand-1", "#3451b2"),
        note: read("--vp-c-purple-1", "#7c3aed"),
        category: read("--vp-c-text-3", "#8a8a8a"),
        external: read("--vp-c-red-1", "#b8272c"),
        text: read("--vp-c-text-1", "#1a1a1a"),
        muted: read("--vp-c-text-3", "#8a8a8a"),
        bg: read("--vp-c-bg", "#ffffff"),
    };
}

function buildStyles(c: ReturnType<typeof palette>): Stylesheet[] {
    return [
        {
            selector: "node",
            style: {
                "background-color": "data(color)",
                label: "data(label)",
                color: c.text,
                "font-size": 10,
                "text-wrap": "wrap",
                "text-max-width": "110px",
                "text-valign": "bottom",
                "text-margin-y": 5,
                width: "data(size)",
                height: "data(size)",
                "border-width": 2,
                "border-color": c.bg,
            },
        },
        {
            // hubs carry their label inside, so it needs the page colour —
            // muted-on-muted would vanish into the hub's own fill — and the
            // box is sized to the text (see hubWidth)
            selector: 'node[kind = "category"]',
            style: {
                shape: "round-rectangle",
                "font-size": 11,
                "font-weight": "bold",
                color: c.bg,
                width: "data(w)",
                height: 26,
                "text-valign": "center",
                "text-margin-y": 0,
                "text-outline-width": 0,
            },
        },
        {
            selector: 'node[kind = "external"]',
            style: { shape: "diamond", "font-size": 9, color: c.muted },
        },
        {
            selector: "edge",
            style: {
                width: 1.4,
                "line-color": c.muted,
                "curve-style": "bezier",
                "target-arrow-shape": "triangle",
                "target-arrow-color": c.muted,
                "arrow-scale": 0.8,
                opacity: 0.5,
            },
        },
        {
            // reference edges are the interesting ones — draw them heavier
            selector: 'edge[kind = "ref"]',
            style: { width: 2, "line-color": c.problem, "target-arrow-color": c.problem, opacity: 0.9 },
        },
        { selector: ".dim", style: { opacity: 0.12 } },
        { selector: ".hidden", style: { display: "none" } },
        { selector: "node:selected", style: { "border-width": 3, "border-color": c.problem } },
    ];
}

/** rough text width in px — CJK glyphs are about twice an ASCII one at 11px */
function hubWidth(label: string): number {
    const wide = /[⺀-鿿＀-￯]/;
    return [...label].reduce((w, ch) => w + (wide.test(ch) ? 12 : 7), 0) + 22;
}

function elements(): ElementDefinition[] {
    const colors = palette();
    const nodes: ElementDefinition[] = data.nodes.map((node) => ({
        data: {
            id: node.id,
            label: node.label,
            kind: node.kind,
            color: colors[node.kind],
            w: hubWidth(node.label),
            size: node.kind === "category" ? 34 : node.kind === "external" ? 14 : 16 + node.degree * 2.5,
            url:
                node.kind === "external"
                    ? node.url // absolute Luogu link for pids the KB does not hold
                    : node.url
                      ? withBase(node.url)
                      : undefined,
        },
    }));
    const edges: ElementDefinition[] = data.edges.map((edge, i) => ({
        data: { id: `e${i}`, source: edge.source, target: edge.target, kind: edge.kind },
    }));
    return [...nodes, ...edges];
}

async function mount() {
    if (!host.value) return;
    let cytoscape;
    try {
        cytoscape = (await import(/* @vite-ignore */ CYTOSCAPE_URL)).default;
    } catch {
        failed.value = true; // CDN unreachable — the page still renders
        return;
    }
    styles = buildStyles(palette());
    cy = cytoscape({
        container: host.value,
        elements: elements(),
        style: styles,
        // wheel zoom stays off: the canvas lives in a scrolling document, so
        // zoom is on the buttons and pan on drag
        userZoomingEnabled: false,
        minZoom: 0.15,
        maxZoom: 4,
        layout: {
            name: "cose",
            animate: false,
            randomize: false,
            padding: 40,
            nodeRepulsion: 20_000,
            idealEdgeLength: 115,
            edgeElasticity: 120,
            nestingFactor: 1.1,
            gravity: 0.35,
        },
    });

    cy.on("tap", "node", (event) => {
        const url = event.target.data("url");
        if (url) window.location.href = url;
    });
    cy.on("mouseover", "node", (event) => {
        const node = event.target;
        cy!.elements().addClass("dim");
        node.removeClass("dim").connectedEdges().removeClass("dim");
        node.neighborhood().nodes().removeClass("dim");
        if (host.value) host.value.style.cursor = "pointer";
    });
    cy.on("mouseout", "node", () => {
        cy!.elements().removeClass("dim");
        if (host.value) host.value.style.cursor = "";
    });

    setExternal(showExternal.value);
    observer = new ResizeObserver(() => cy?.resize());
    observer.observe(host.value);
}

function setExternal(show: boolean) {
    showExternal.value = show;
    if (!cy) return;
    const external = cy.$('node[kind = "external"]');
    // the edges into them would otherwise dangle
    const touching = cy
        .edges()
        .filter((edge) => edge.source().data("kind") === "external" || edge.target().data("kind") === "external");
    if (show) {
        external.removeClass("hidden");
        touching.removeClass("hidden");
    } else {
        external.addClass("hidden");
        touching.addClass("hidden");
    }
}

function zoomBy(factor: number) {
    if (!cy) return;
    const level = Math.min(4, Math.max(0.15, cy.zoom() * factor));
    cy.zoom({ level, renderedPosition: { x: cy.width() / 2, y: cy.height() / 2 } });
}

const externalCount = data.nodes.filter((n) => n.kind === "external").length;

onMounted(mount);
onBeforeUnmount(() => {
    observer?.disconnect();
    cy?.destroy();
});

// the canvas is painted from the palette, so it must be repainted on theme
// flip — in the frame *after* it: VitePress swaps the `dark` class on <html>
// in its own watcher, so reading the variables synchronously here still sees
// the outgoing theme's colours and the labels end up dark-on-dark
watch(isDark, () => {
    requestAnimationFrame(() => {
        if (!cy) return;
        const colors = palette();
        for (const node of data.nodes) {
            cy.getElementById(node.id).data("color", colors[node.kind]);
        }
        cy.style(buildStyles(colors));
    });
});
</script>

<template>
    <div class="rg">
        <div class="rg-bar">
            <span class="rg-legend">
                <span class="rg-key" style="--c: var(--vp-c-brand-1)">problem</span>
                <span class="rg-key" style="--c: var(--vp-c-purple-1)">note</span>
                <span class="rg-key" style="--c: var(--vp-c-text-3)">category</span>
                <span v-if="externalCount" class="rg-key" style="--c: var(--vp-c-red-1)">
                    referenced elsewhere
                </span>
            </span>
            <span class="rg-actions">
                <button v-if="externalCount" class="rg-btn" @click="setExternal(!showExternal)">
                    {{ showExternal ? "Hide" : "Show" }} external ({{ externalCount }})
                </button>
                <button class="rg-btn" title="Zoom in" @click="zoomBy(1.25)">＋</button>
                <button class="rg-btn" title="Zoom out" @click="zoomBy(1 / 1.25)">－</button>
                <button class="rg-btn" @click="cy?.layout({ name: 'cose', animate: false, randomize: false, padding: 40 }).run()">
                    Re-layout
                </button>
                <button class="rg-btn" @click="cy?.fit(undefined, 40)">Fit</button>
            </span>
        </div>
        <div ref="host" class="rg-canvas" :class="{ 'rg-failed': failed }">
            <p v-if="failed" class="rg-note">
                The graph library could not be loaded — the note pages themselves are unaffected.
            </p>
        </div>
        <p class="rg-hint">
            Click a node to open its note · hover to highlight what it points at · the graph is
            generated at build time from links inside the notes.
        </p>
    </div>
</template>

<style scoped>
.rg {
    margin: 1.5rem 0;
}

.rg-bar {
    display: flex;
    flex-wrap: wrap;
    gap: 0.75rem;
    align-items: center;
    justify-content: space-between;
    margin-bottom: 0.75rem;
}

.rg-legend,
.rg-actions {
    display: flex;
    flex-wrap: wrap;
    gap: 0.5rem;
    align-items: center;
}

.rg-key {
    display: inline-flex;
    align-items: center;
    gap: 0.4rem;
    font-size: 0.78rem;
    color: var(--vp-c-text-2);
}

.rg-key::before {
    content: "";
    width: 0.65rem;
    height: 0.65rem;
    border-radius: 50%;
    background: var(--c);
}

.rg-btn {
    padding: 0.25rem 0.7rem;
    border: 1px solid var(--vp-c-divider);
    border-radius: 999px;
    background: var(--vp-c-bg-soft);
    color: var(--vp-c-text-2);
    font-size: 0.78rem;
    cursor: pointer;
    transition: color 0.2s, border-color 0.2s;
}

.rg-btn:hover {
    border-color: var(--vp-c-brand-1);
    color: var(--vp-c-brand-1);
}

.rg-canvas {
    height: min(72vh, 720px);
    border: 1px solid var(--vp-c-divider);
    border-radius: 10px;
    background: var(--vp-c-bg-alt);
    overflow: hidden;
}

.rg-failed {
    display: flex;
    align-items: center;
    justify-content: center;
}

.rg-note {
    color: var(--vp-c-text-3);
    font-size: 0.85rem;
}

.rg-hint {
    margin-top: 0.75rem;
    color: var(--vp-c-text-3);
    font-size: 0.8rem;
}
</style>
