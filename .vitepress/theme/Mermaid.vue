<script setup lang="ts">
// Draws the ```mermaid blocks (../markdown.ts ships them as
// `<pre class="mermaid">`). mermaid is a multi-MB library, so it is fetched
// from the CDN on demand (./vendor.ts) — and only by pages that actually carry
// a diagram, the same trick as the relation graph's cytoscape. Until it arrives
// — and if the CDN is unreachable — the reader sees the diagram source, which
// is the graceful half of the trade.
//
// Re-drawing (dark mode, or a second visit to the page) starts from that
// source: mermaid replaces the element's content with its SVG and marks it
// `data-processed`, so the original text is stashed in `data-src` on the first
// pass and restored before every later one.
import { nextTick, onMounted, watch } from "vue";
import { useData, useRouter } from "vitepress";
import { MERMAID_URL } from "./vendor.ts";

const { isDark } = useData();
const router = useRouter();

let mermaid: typeof import("mermaid").default | undefined;
let unavailable = false;

async function draw(): Promise<void> {
    const blocks = [...document.querySelectorAll<HTMLElement>("pre.mermaid")];
    if (!blocks.length) return;
    try {
        mermaid ??= (await import(/* @vite-ignore */ MERMAID_URL)).default;
    } catch {
        unavailable = true; // CDN unreachable: the source stays on the page
    }
    if (unavailable || !mermaid) return;

    for (const block of blocks) {
        if (block.dataset.src === undefined) block.dataset.src = block.textContent ?? "";
        else block.textContent = block.dataset.src; // back from the SVG
        block.removeAttribute("data-processed");
        delete block.dataset.error;
    }
    mermaid.initialize({
        startOnLoad: false,
        securityLevel: "strict",
        theme: isDark.value ? "dark" : "default",
    });
    try {
        await mermaid.run({ nodes: blocks });
    } catch (error) {
        // the ones that did make it are marked; say so on the rest
        for (const block of blocks) {
            if (block.dataset.processed === undefined) {
                block.dataset.error = `Mermaid could not draw this diagram: ${error}`;
            }
        }
    }
}

onMounted(() => {
    nextTick(draw);
    // the router reuses this component across pages, so navigation is a watch
    // of its own (the new page's DOM is in place when this fires)
    router.onAfterRouteChanged = () => {
        nextTick(draw);
    };
});

watch(isDark, () => {
    nextTick(draw);
});
</script>

<template>
    <span class="mermaid-mount" hidden />
</template>
