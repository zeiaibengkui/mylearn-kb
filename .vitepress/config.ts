// myLearn VitePress site — copied into the project by `myLearn site setup`.
// The project root IS the docs root: problems/ + notes/ render in place, no
// generation step. Run vitepress from the project root (`myLearn site dev`,
// the package.json scripts, or the CI workflow) — the sidebar walker uses
// process.cwd().
//
// GitHub Pages: repo sites live under /<repo>/ — the scaffolded workflow
// sets MYLEARN_BASE to /<git repo name>/; user sites (<user>.github.io)
// serve from "/". Locally it stays "/" (vitepress dev/preview ignore base
// concerns for navigation).

import { defineConfig, type DefaultTheme, type HeadConfig } from "vitepress";
import fs from "node:fs";
import path from "node:path";
import { buildSidebar } from "./sidebar.ts";
import { giscusConfig, type GiscusConfig } from "./giscus.ts";

// SEO extras are opt-in: MYLEARN_SITE_URL (canonical origin, e.g.
// https://chunl.ai) enables canonical/og tags + sitemap + robots; the lang
// defaults to en-US and is set for the build with MYLEARN_LANG (e.g. zh-CN).
// Comments are opt-in too: MYLEARN_GISCUS (see giscus.ts) mounts giscus
// under every page.
const siteUrl = (process.env.MYLEARN_SITE_URL ?? "").replace(/\/+$/, "");
const siteName = "myLearn";
const siteDescription = "Personal competitive-programming knowledge base";

function base(): string {
    const raw = process.env.MYLEARN_BASE ?? "/";
    // normalize so the same build output works for a user-site repo
    return raw !== "/" && /\.github\.io\/?$/.test(raw) ? "/" : raw;
}

/** source file → served path (mirrors the config's rewrites); segments
 *  encoded so the URL is exactly what the browser requests */
function routeFor(rel: string): string {
    let p = rel.replace(/\.md$/i, "");
    // readme.md is rewritten to index.md → the home page
    if (p.toLowerCase() === "readme") p = "index";
    for (const suffix of ["/index", "/problem"]) {
        if (p.endsWith(suffix)) {
            p = p.slice(0, -suffix.length);
            break;
        }
    }
    // a bare index/problem (the home page) is the base itself (already "/"-terminated)
    if (p === "index" || p === "problem" || p === "") return base();
    return base() + p.split("/").map(encodeURIComponent).join("/") + "/";
}

/** copy every non-markdown file under problems/ into the output, so hosted
 *  pages keep the solution sources / PDFs downloadable (VitePress copies only
 *  .vitepress/public/, and md-only files are handled by the page scan) */
function mirrorSources(root: string, dir: string, outDir: string): void {
    for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
        const src = path.join(dir, entry.name);
        if (entry.isDirectory()) {
            mirrorSources(root, src, outDir);
            continue;
        }
        if (entry.name.startsWith(".") || entry.name.toLowerCase().endsWith(".md")) continue;
        const dest = path.join(outDir, path.relative(root, src));
        fs.mkdirSync(path.dirname(dest), { recursive: true });
        fs.copyFileSync(src, dest);
    }
}

const sitemapUrls: string[] = [];

// hoisted + annotated so the extra `giscus` key types cleanly alongside the
// default theme's config (background: VitePress's ThemeConfig is not augmentable)
const themeConfig: DefaultTheme.Config & { giscus?: GiscusConfig } = {
    nav: [{ text: "Home", link: "/" }],
    sidebar: buildSidebar(process.cwd()),
    outline: { level: [2, 3] },
    search: { provider: "local" },
    // sidebar entries auto-follow the notes tree; only set explicit
    // values if you want a fixed order or extra top-level links
    docFooter: { prev: "Previous", next: "Next" },
    // this KB's giscus ids (https://giscus.app); threads are keyed by og:title,
    // which every page carries. Set MYLEARN_GISCUS in the environment to
    // override (the generic template reads it and stays project-agnostic).
    giscus: giscusConfig({
        MYLEARN_GISCUS: "zeiaibengkui/mylearn-kb,R_kgDOUPPgIw,General,DIC_kwDOUPPgI84DFcWJ",
        MYLEARN_GISCUS_MAPPING: "og:title",
        MYLEARN_LANG: process.env.MYLEARN_LANG ?? "zh-CN",
    }),
};

export default defineConfig({
    base: base(),
    lang: process.env.MYLEARN_LANG ?? "en-US",
    title: siteName,
    description: siteDescription,
    markdown: {
        math: true, // $…$ / $$…$$ via markdown-it-mathjax3
    },
    cleanUrls: true,
    // .mylearn/ (project config + snapshot) is not content; the scaffolded
    // workflow/package.json dirs are not content either
    srcExclude: [".mylearn/**", "node_modules/**", ".github/**"],
    rewrites: {
        // every problem note is the index of its dir → /problems/<cat>/<title>/
        "problems/:category/:title/problem.md": "problems/:category/:title/index.md",
        "readme.md": "index.md",
    },
    themeConfig,
    transformHead: ({ page, title, description }) => {
        // og:title is emitted unconditionally — it is cheap and giscus's
        // og:title mapping reads it (see MYLEARN_GISCUS_MAPPING)
        const tags: HeadConfig[] = [["meta", { property: "og:title", content: title }]];
        // canonical + remaining Open Graph — with a custom domain both alias
        // hosts rewrite to one canonical origin, so the duplicates stay harmless
        if (!siteUrl) return tags;
        const url = siteUrl + routeFor(page);
        sitemapUrls.push(url);
        return [
            ...tags,
            ["link", { rel: "canonical", href: url }],
            ["meta", { property: "og:url", content: url }],
            ["meta", { property: "og:description", content: description }],
            ["meta", { property: "og:site_name", content: siteName }],
            ["meta", { name: "twitter:card", content: "summary" }],
        ];
    },
    buildEnd: (siteConfig) => {
        const root = process.cwd();
        const problems = path.join(root, "problems");
        if (fs.existsSync(problems)) mirrorSources(root, problems, siteConfig.outDir);
        if (siteUrl && sitemapUrls.length) {
            const urls = [...new Set(sitemapUrls)].filter((u) => !u.endsWith("/404/")).sort();
            const locs = urls.map((u) => `  <url><loc>${u}</loc></url>`).join("\n");
            fs.writeFileSync(
                path.join(siteConfig.outDir, "sitemap.xml"),
                `<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n${locs}\n</urlset>\n`
            );
            // the sitemap is served from the same base as the site itself
            fs.writeFileSync(
                path.join(siteConfig.outDir, "robots.txt"),
                `User-agent: *\nAllow: /\nSitemap: ${siteUrl}${base()}sitemap.xml\n`
            );
        }
    },
});
