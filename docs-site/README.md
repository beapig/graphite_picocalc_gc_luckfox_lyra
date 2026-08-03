# docs-site — plain-markdown source, three outputs

This tree is the single source for all user-facing documentation. It is
**not** MkDocs-specific — no admonitions (`!!! note`), no `pymdownx`
extensions, no `{: attrs}` blocks. Plain CommonMark (plus `$...$` /
`$$...$$` math, which every consumer below already understands) so the
same files render acceptably in a GitHub wiki, through pandoc, and in a
future MkDocs Material site.

See `docs/notes/docs-site-plan.md` for the original planning note and its
2026-08-03 follow-up recording this shape.

## How the pipeline works

```
docs-site/SUMMARY.md   <- nav order + titles (the only source of truth)
docs-site/**/*.md      <- page content
        │
        ├── scripts/gen-doc-reference.py  → docs-site/reference/*.md (generated, committed)
        ├── scripts/gen-wiki.sh           → build/wiki/*.md (gitignored, flat GitHub-wiki tree)
        └── scripts/gen-offline.sh        → build/offline/*.md(+.html/.pdf) (gitignored bundle)
```

`SUMMARY.md` is a plain nested markdown list of relative links (mdBook
style). All three generators parse it to get page order and titles — it
is the only place page order is declared. Add a page by:

1. Creating the `.md` file under `docs-site/`.
2. Adding one link line to `SUMMARY.md` in the position you want it to
   appear.
3. Running `python3 scripts/validate_md.py docs-site/` to check it.

## Reference pages are generated — don't hand-edit

`docs-site/reference/function-catalog.md`, `key-reference.md`, and
`syntax-reference.md` carry a "do not edit by hand" header and are
regenerated from firmware source (`src/math/catalog.cpp`,
`src/apps/help_screen.cpp`) by `scripts/gen-doc-reference.py`. Firmware
stays the source of truth; these docs are derived from it, not written
independently. CI fails the build if the committed files differ from a
fresh run (see `.github/workflows/docs.yml`) — regenerate and commit
after any change to those source files.

## Wiki flattening rule

GitHub wikis are a flat namespace (no subdirectories). `gen-wiki.sh`
flattens `docs-site/**/*.md` with this deterministic rule:

- `index.md` → `Home.md` (GitHub's required landing-page name).
- Every other path has its directory separators and leading digits'
  separators turned into hyphens, and each path segment is Title-Cased
  word-by-word (splitting on `-`), e.g.:
  - `getting-started/build-and-flash.md` → `Getting-Started-Build-and-Flash.md`
  - `guide/01-basic-calculations.md` → `Guide-01-Basic-Calculations.md`
  - `reference/function-catalog.md` → `Reference-Function-Catalog.md`
- Relative markdown links between pages are rewritten to point at the
  flattened names (same rule applied to the link target).
- A `_Sidebar.md` is generated from `SUMMARY.md`'s structure, linking to
  the flattened names, so the wiki gets a persistent nav sidebar.

## Publishing to the wiki

`gen-wiki.sh` only builds `build/wiki/` locally — it does not push.
Publishing is either:

**Manual** (any time):

```bash
./scripts/gen-wiki.sh
git clone git@github.com:moodoki/graphite_picocalc_gc.wiki.git /tmp/gpc-wiki
rsync -a --delete --exclude .git build/wiki/ /tmp/gpc-wiki/
cd /tmp/gpc-wiki
git add -A
git commit -m "Sync wiki from docs-site/"
git push
```

**CI** (`.github/workflows/docs.yml`, `publish-wiki` job): runs the same
steps automatically, but only when a `WIKI_TOKEN` repository secret is
present (`GITHUB_TOKEN` cannot push to the wiki repo — wikis are a
separate git remote GitHub does not extend the default token's
permissions to). The step is gated with
`if: ${{ secrets.WIKI_TOKEN != '' }}` so CI stays green with the secret
absent; it simply skips wiki publishing until one is added.

### One-time PAT setup (do this to enable the CI wiki-publish step)

1. GitHub → Settings → Developer settings → Personal access tokens →
   Fine-grained tokens → **Generate new token**.
2. Resource owner: `moodoki`. Repository access: only
   `graphite_picocalc_gc`.
3. Permissions: **Contents: Read and write** (wiki pages are pushed over
   the repo's wiki git remote, which fine-grained PATs cover via the
   Contents permission plus wiki access — if the fine-grained token
   can't push to the `.wiki.git` remote in practice, fall back to a
   classic PAT with the `repo` scope instead).
4. Copy the token. Repo → Settings → Secrets and variables → Actions →
   **New repository secret** → name it `WIKI_TOKEN`, paste the value.
5. Re-run (or wait for) the `docs` workflow — the `publish-wiki` job
   will pick it up automatically.

Until step 4 is done, wiki publishing stays manual (see above) — this is
expected, not a bug.

## Offline bundle

`gen-offline.sh` always produces a single concatenated markdown file
(`build/offline/picocalc-gc-guide.md`, stdlib/shell only, no external
dependency — the guaranteed-available output). If `pandoc` is on `PATH`
it additionally produces a self-contained single-file HTML and a PDF
from the same concatenation. If pandoc is missing, the script prints
`brew install pandoc` and exits 0 (not a failure — the markdown bundle
alone is still useful).

## Validating

```bash
python3 scripts/validate_md.py docs-site/
```

Checks: balanced `$...$` math delimiters, no bare Unicode math glyphs
outside math mode (write `$\pi$` or `` `pi` ``, never a bare `π`),
balanced code fences, well-formed links, consistent markdown-table
column counts. CI runs this over `docs-site/ docs/ AGENTS.md README.md`
on every push/PR.
