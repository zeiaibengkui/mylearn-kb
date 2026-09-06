# myLearn new project

## Markdown Style

### Latex

```bash
find . -name "*.md" -type f -exec perl -i -0pe 's/\\\((.+?)\\\)/\$\1\$/gs; s/\\\[(.+?)\\\]/\$\$\1\$\$/gs' {} \;
```