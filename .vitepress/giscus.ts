// giscus (GitHub Discussions comments) config — built from the environment so
// the scaffolded config.ts stays project-agnostic, mirroring the SEO knobs:
//   MYLEARN_GISCUS="<owner/repo>,<repoId>,<category>,<categoryId>"
//   MYLEARN_GISCUS_MAPPING  discussion key (default "pathname"; "og:title" also
//                           works — every page carries og:title)
//   MYLEARN_GISCUS_LANG     widget language (defaults to MYLEARN_LANG)
// The four ids come from https://giscus.app (the repo needs Discussions enabled
// and the giscus app installed). Absent/malformed env → no comments.

export interface GiscusConfig {
    repo: string;
    repoId: string;
    category: string;
    categoryId: string;
    mapping: string;
    lang: string;
}

/** giscus language codes: zh maps to its region variants, everything else uses
 *  the base tag (`en-US` → `en`, which is what giscus expects) */
function giscusLang(lang: string): string {
    const lower = lang.toLowerCase();
    if (lower.startsWith("zh")) return /tw|hk|mo|hant/.test(lower) ? "zh-TW" : "zh-CN";
    return lower.split("-")[0];
}

export function giscusConfig(env: NodeJS.ProcessEnv = process.env): GiscusConfig | undefined {
    const raw = env.MYLEARN_GISCUS ?? "";
    const fields = raw.split(",").map((s) => s.trim()).filter(Boolean);
    if (!fields.length) return undefined;
    if (fields.length !== 4) {
        console.warn(
            "[mylearn] MYLEARN_GISCUS needs 4 comma-separated fields " +
                "(owner/repo, repoId, category, categoryId) — comments are disabled"
        );
        return undefined;
    }
    const [repo, repoId, category, categoryId] = fields;
    return {
        repo,
        repoId,
        category,
        categoryId,
        mapping: env.MYLEARN_GISCUS_MAPPING?.trim() || "pathname",
        lang: env.MYLEARN_GISCUS_LANG?.trim() || giscusLang(env.MYLEARN_LANG ?? "en-US"),
    };
}
