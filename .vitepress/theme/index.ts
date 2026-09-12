// Default theme + a breadcrumb above the doc content (doc-before) and giscus
// comments below it (doc-after) when the config carries a giscus block.
// Copied into the project's .vitepress/theme/ by `myLearn site setup`.

import { defineComponent, h } from "vue";
import DefaultTheme from "vitepress/theme";
import { useData } from "vitepress";
import Breadcrumb from "./Breadcrumb.vue";
import Giscus from "./Giscus.vue";
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
                    "doc-after": () => (giscus ? h(Giscus, giscus) : null),
                });
        },
    }),
};
