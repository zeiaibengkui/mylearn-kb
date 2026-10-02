// markdown-it tweaks the knowledge-base content needs. config.ts installs both
// through `markdown.config` — VitePress runs that after its own plugins (the
// math one included), so a rule can be replaced and a renderer wrapped from here.
//
//   mathSpaces   — the statements and notes write inline math with the
//                  delimiter padded (`$ n$`, `$x $`), which is legal TeX but
//                  rejected by markdown-it-mathjax3's delimiter test
//   mermaidFence — ```mermaid blocks become <pre class="mermaid">, drawn in the
//                  browser by theme/Mermaid.vue; without JavaScript they are
//                  still readable as the diagram source

import type { MarkdownOptions } from "vitepress";

type MarkdownIt = Parameters<NonNullable<MarkdownOptions["config"]>>[0];
type RuleInline = Parameters<MarkdownIt["inline"]["ruler"]["at"]>[1];

/** Inline math, with the whitespace conditions of the upstream rule dropped.
 *
 *  markdown-it-mathjax3 (like the markdown-it-katex it borrows from) refuses a
 *  `$` that is followed by a space and a `$` that is preceded by one, so
 *  `$ n$` and `$x $` — how most of this knowledge base is written — stay
 *  literal text, and worse, their stray `$` pairs are then free to pair up
 *  across the sentence and swallow prose. Everything else about the rule is
 *  kept: backslash escapes, the `$$` guard, and the "closer followed by a
 *  digit" test, which is what keeps prices ("$5 … $10") from turning into
 *  math — a `$` before a number is still not a closing delimiter. */
function mathSpaces(md: MarkdownIt): void {
    const mathInline: RuleInline = (state, silent) => {
        if (state.src[state.pos] !== "$") return false;
        const start = state.pos + 1;
        let match = start;
        while ((match = state.src.indexOf("$", match)) !== -1) {
            // an even number of backslashes escapes the `$` (odd ones do not)
            let escapes = 0;
            while (state.src[match - 1 - escapes] === "\\") escapes++;
            if (escapes % 2 === 0) break;
            match++;
        }
        if (match === -1 || match === start) return false; // unclosed, or "$$"
        const content = state.src.slice(start, match);
        if (!content.trim()) return false; // "$ $" is not math either
        const after = state.src.charCodeAt(match + 1);
        if (after >= 0x30 && after <= 0x39) return false; // "$1" is a price
        if (!silent) {
            const token = state.push("math_inline", "math", 0);
            token.markup = "$";
            token.content = content;
        }
        state.pos = match + 1;
        return true;
    };
    // at(), not after(): the plugin's rule is replaced in place, so its
    // renderer (which only reads the token's content) keeps doing the MathJax
    // conversion. Throws if math is not enabled — that is the point.
    md.inline.ruler.at("math_inline", mathInline);
}

/** ```mermaid fences → `<pre class="mermaid">`, mermaid's own convention: the
 *  element keeps the source as its text, which the client-side renderer reads
 *  and replaces with the SVG. */
function mermaidFence(md: MarkdownIt): void {
    // VitePress's wrapper (language class + copy button); every other language
    // still needs it
    const fence = md.renderer.rules.fence!;
    md.renderer.rules.fence = (tokens, idx, options, env, self) => {
        const token = tokens[idx];
        // fence info can carry extras ("mermaid title=…"), the language leads
        if (token.info.trim().split(/\s+/)[0].toLowerCase() === "mermaid") {
            return `<pre class="mermaid">${md.utils.escapeHtml(token.content)}</pre>\n`;
        }
        return fence(tokens, idx, options, env, self);
    };
}

export function configureMarkdown(md: MarkdownIt): void {
    mathSpaces(md);
    mermaidFence(md);
}
