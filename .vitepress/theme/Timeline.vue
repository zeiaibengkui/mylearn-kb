<script setup lang="ts">
// Vertical timeline of every note (see ../timeline.data.ts). Server-rendered:
// the entries are in the HTML, the filter chips are the only client state.
import { computed, ref } from "vue";
import { withBase } from "vitepress";
import { data } from "../timeline.data";

type Filter = "all" | "problem" | "note";

const filter = ref<Filter>("all");

const counts = computed(() => ({
    all: data.entries.length,
    problem: data.entries.filter((e) => e.kind === "problem").length,
    note: data.entries.filter((e) => e.kind === "note").length,
}));

const shown = computed(() =>
    filter.value === "all" ? data.entries : data.entries.filter((e) => e.kind === filter.value)
);

// UTC formatting keeps the prerendered HTML and the hydrated client identical
const day = (ms: number) => new Date(ms).toISOString().slice(0, 10);
const month = (ms: number) => new Date(ms).toISOString().slice(0, 7);

const months = computed(() => {
    const out: { month: string; items: typeof data.entries }[] = [];
    for (const entry of shown.value) {
        const key = month(entry.date);
        const last = out[out.length - 1];
        if (last?.month === key) last.items.push(entry);
        else out.push({ month: key, items: [entry] });
    }
    return out;
});
</script>

<template>
    <div class="tl">
        <div class="tl-filters">
            <button
                v-for="key in (['all', 'problem', 'note'] as Filter[])"
                :key="key"
                class="tl-chip"
                :class="{ active: filter === key }"
                @click="filter = key"
            >
                {{ key === "all" ? "All" : key === "problem" ? "Problems" : "Notes" }}
                <span class="tl-count">{{ counts[key] }}</span>
            </button>
        </div>

        <p v-if="!shown.length" class="tl-empty">Nothing here yet.</p>

        <section v-for="group in months" :key="group.month" class="tl-month">
            <h2 class="tl-month-head">{{ group.month }}</h2>
            <ol class="tl-list">
                <li v-for="entry in group.items" :key="entry.url" class="tl-item">
                    <a class="tl-link" :href="withBase(entry.url)">
                        <time class="tl-date" :datetime="day(entry.date)">{{ day(entry.date) }}</time>
                        <span class="tl-body">
                            <span class="tl-title">{{ entry.title }}</span>
                            <span class="tl-meta">
                                <span class="tl-kind" :data-kind="entry.kind">
                                    {{ entry.kind === "problem" ? "problem" : "note" }}
                                </span>
                                <span class="tl-group">{{ entry.group }}</span>
                            </span>
                        </span>
                    </a>
                </li>
            </ol>
        </section>
    </div>
</template>

<style scoped>
.tl {
    margin: 1.5rem 0;
}

.tl-filters {
    display: flex;
    gap: 0.5rem;
    margin-bottom: 1.5rem;
    flex-wrap: wrap;
}

.tl-chip {
    display: inline-flex;
    align-items: center;
    gap: 0.4rem;
    padding: 0.3rem 0.75rem;
    border: 1px solid var(--vp-c-divider);
    border-radius: 999px;
    background: var(--vp-c-bg-soft);
    color: var(--vp-c-text-2);
    font-size: 0.85rem;
    cursor: pointer;
    transition: color 0.2s, border-color 0.2s, background-color 0.2s;
}

.tl-chip:hover {
    border-color: var(--vp-c-brand-1);
    color: var(--vp-c-text-1);
}

.tl-chip.active {
    border-color: var(--vp-c-brand-1);
    background: var(--vp-c-brand-soft);
    color: var(--vp-c-brand-1);
}

.tl-count {
    font-variant-numeric: tabular-nums;
    opacity: 0.7;
}

.tl-empty {
    color: var(--vp-c-text-3);
}

.tl-month-head {
    position: sticky;
    /* park under the fixed nav bar while the month scrolls by */
    top: var(--vp-nav-height);
    z-index: 1;
    margin: 0;
    padding: 0.5rem 0 0.5rem 2.25rem;
    border: none;
    background: var(--vp-c-bg);
    color: var(--vp-c-text-3);
    font-size: 0.8rem;
    font-weight: 600;
    letter-spacing: 0.08em;
}

.tl-list {
    margin: 0;
    padding: 0 0 0 2.25rem;
    list-style: none;
    /* the rail the dots sit on */
    border-left: 2px solid var(--vp-c-divider);
}

.tl-item {
    position: relative;
    margin: 0 0 0.35rem;
}

/* dot on the rail, hollow ring so it reads as a node in a timeline */
.tl-item::before {
    content: "";
    position: absolute;
    left: -1.55rem;
    top: 0.85rem;
    width: 0.6rem;
    height: 0.6rem;
    border: 2px solid var(--vp-c-divider);
    border-radius: 50%;
    background: var(--vp-c-bg);
    transition: border-color 0.2s, background-color 0.2s;
}

.tl-item:has(.tl-link:hover)::before {
    border-color: var(--vp-c-brand-1);
    background: var(--vp-c-brand-1);
}

.tl-link {
    display: flex;
    gap: 1rem;
    padding: 0.6rem 0.8rem;
    border-radius: 8px;
    /* the doc theme underlines every link — a whole list of underlined dates
       and titles reads as noise, so the underline comes back on hover */
    text-decoration: none;
    transition: background-color 0.2s;
}

.tl-link:hover .tl-title {
    text-decoration: underline;
}

.tl-link:hover {
    background: var(--vp-c-bg-soft);
}

.tl-date {
    flex: none;
    padding-top: 0.15rem;
    color: var(--vp-c-text-3);
    font-size: 0.8rem;
    font-variant-numeric: tabular-nums;
}

.tl-body {
    display: flex;
    flex-direction: column;
    gap: 0.2rem;
    min-width: 0;
}

.tl-title {
    color: var(--vp-c-text-1);
    font-weight: 500;
    line-height: 1.4;
}

.tl-link:hover .tl-title {
    color: var(--vp-c-brand-1);
}

.tl-meta {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    font-size: 0.75rem;
    color: var(--vp-c-text-3);
}

.tl-kind {
    padding: 0.05rem 0.45rem;
    border-radius: 999px;
    font-size: 0.7rem;
}

.tl-kind[data-kind="problem"] {
    background: var(--vp-c-brand-soft);
    color: var(--vp-c-brand-1);
}

.tl-kind[data-kind="note"] {
    background: var(--vp-c-default-soft);
    color: var(--vp-c-text-2);
}
</style>
