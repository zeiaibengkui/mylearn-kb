// Default theme + a breadcrumb above the doc content (doc-before) and giscus
// comments below it (doc-after) when the config carries a giscus block.
//
// Timeline.vue / RelationGraph.vue are deliberately NOT registered here: each
// imports its own build-time data, which would then ride along in this shared
// chunk on every page (and grow with the knowledge base). The pages that want
// them import them directly — see /timeline.md and /graph.md. Mermaid *is*
// mounted here (every page may carry a diagram, and a page cannot import a
// component without a <script setup> block) but stays cheap: the component is
// tiny, and it fetches mermaid itself only when a page actually has a diagram.

import { defineComponent, h } from "vue";
import DefaultTheme from "vitepress/theme";
import { useData } from "vitepress";
import Breadcrumb from "./Breadcrumb.vue";
import Giscus from "./Giscus.vue";
import Mermaid from "./Mermaid.vue";
import type { GiscusConfig } from "../giscus.ts";
import "./style.css";

const { Layout } = DefaultTheme;

export default {
    extends: DefaultTheme,
    Layout: defineComponent({
        name: "MyLearnLayout",
        setup(_, { slots }) {
            // set from MYLEARN_GISCUS by config.ts; undefined → no comments
            const giscus = useData().theme.value.giscus as GiscusConfig | undefined;
            return () =>
                h(Layout, null, {
                    // keep the theme's other slots flowing through
                    ...slots,
                    "doc-before": () => h(Breadcrumb),
                    "doc-after": () => [
                        giscus ? h(Giscus, giscus) : null,
                        // renders no markup of its own: it draws the
                        // <pre class="mermaid"> blocks already in the page
                        h(Mermaid),
                    ],
                });
        },
    }),
};
