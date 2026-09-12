<script setup lang="ts">
// giscus comments (GitHub Discussions) — injects the official client.js so the
// scaffold stays dependency-free. VitePress is a SPA: the iframe has to be
// rebuilt on every route change, and the comment theme follows the site's
// appearance toggle (giscus's preferred_color_scheme only tracks the OS).
import { onBeforeUnmount, onMounted, ref, watch } from "vue";
import { useData, useRoute } from "vitepress";

// mirrors GiscusConfig in ../giscus.ts, declared inline on purpose: Vue's SFC
// compiler has to resolve the type of defineProps<T>() and cannot read imported
// types during VitePress's client build ("No fs option provided to compileScript")
interface GiscusProps {
    repo: string;
    repoId: string;
    category: string;
    categoryId: string;
    mapping: string;
    lang: string;
}

const props = defineProps<GiscusProps>();

const { isDark } = useData();
const route = useRoute();
const host = ref<HTMLElement | null>(null);

function mount() {
    const el = host.value;
    if (!el) return;
    el.innerHTML = "";
    const script = document.createElement("script");
    script.src = "https://giscus.app/client.js";
    script.async = true;
    script.crossOrigin = "anonymous";
    const attrs: Record<string, string> = {
        "data-repo": props.repo,
        "data-repo-id": props.repoId,
        "data-category": props.category,
        "data-category-id": props.categoryId,
        "data-mapping": props.mapping,
        "data-strict": "0",
        "data-reactions-enabled": "1",
        "data-emit-metadata": "0",
        "data-input-position": "top",
        "data-theme": isDark.value ? "dark" : "light",
        "data-lang": props.lang,
        "data-loading": "lazy",
    };
    for (const [key, value] of Object.entries(attrs)) script.setAttribute(key, value);
    el.appendChild(script);
}

onMounted(mount);
watch([() => route.path, isDark], mount);
onBeforeUnmount(() => {
    if (host.value) host.value.innerHTML = "";
});
</script>

<template>
    <div ref="host" class="mylearn-giscus" />
</template>
