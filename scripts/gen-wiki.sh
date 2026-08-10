#!/usr/bin/env bash
# Build a flat GitHub-wiki tree from docs-site/ into build/wiki/ (gitignored).
#
# GitHub wikis have no subdirectories, so every docs-site/**/*.md path is
# flattened to a single wiki page name. See docs-site/README.md ("Wiki
# flattening rule") for the exact rule and worked examples; the logic
# lives inline below (python3, stdlib-only) so there is one place to
# read it.
#
# This script only builds build/wiki/ locally — it does not push
# anywhere. See docs-site/README.md "Publishing to the wiki" for the
# manual steps, and .github/workflows/docs.yml's publish-wiki job for
# the CI path (gated on a WIKI_TOKEN secret).
#
# Usage: ./scripts/gen-wiki.sh

set -euo pipefail

cd "$(dirname "$0")/.."

SRC=docs-site
OUT=build/wiki

echo "== Cleaning $OUT =="
rm -rf "$OUT"
mkdir -p "$OUT"

echo "== Flattening $SRC -> $OUT =="
python3 - "$SRC" "$OUT" <<'PYEOF'
import re
import sys
from pathlib import Path

src_root = Path(sys.argv[1])
out_root = Path(sys.argv[2])

STOPWORDS = {
    "a", "an", "and", "as", "at", "but", "by", "for", "in", "nor",
    "of", "on", "or", "so", "the", "to", "up", "vs", "with", "yet",
}


def cap_word(word: str, is_edge: bool) -> str:
    if not word.isalpha():
        return word
    lower = word.lower()
    if lower in STOPWORDS and not is_edge:
        return lower
    return lower.capitalize()


def flatten_name(rel_path: Path) -> str:
    """docs-site-relative path -> flat wiki page filename (with .md)."""
    if rel_path == Path("index.md"):
        return "Home.md"
    stem = str(rel_path.with_suffix(""))
    words = stem.replace("/", "-").split("-")
    words = [
        cap_word(w, is_edge=(i == 0 or i == len(words) - 1))
        for i, w in enumerate(words)
    ]
    return "-".join(words) + ".md"


# Only pages actually linked from SUMMARY.md get published to the wiki —
# that file is the nav source of truth, so it also doubles as the
# publish manifest. This deliberately excludes docs-site/README.md and
# docs-site/SUMMARY.md itself (pipeline-internal, not reader content).
summary_text = (src_root / "SUMMARY.md").read_text()
link_target_re = re.compile(r"\[[^\]]+\]\(([^)]+\.md)(?:#[^)]*)?\)")
summary_rels = []
for m in link_target_re.finditer(summary_text):
    rel = Path(m.group(1))
    if rel not in summary_rels:
        summary_rels.append(rel)

md_files = [src_root / rel for rel in summary_rels]
missing = [p for p in md_files if not p.is_file()]
if missing:
    print("ERROR: SUMMARY.md links to missing file(s):", file=sys.stderr)
    for p in missing:
        print(f"  {p}", file=sys.stderr)
    sys.exit(1)

# Map every published source path (relative to src_root) to its
# flattened name, so link rewriting can look targets up regardless of
# which file is doing the linking.
flat_map = {}
for p in md_files:
    rel = p.relative_to(src_root)
    flat_map[rel] = flatten_name(rel)

link_re = re.compile(r"(\[[^\]]*\]\()([^)]+)(\))")


def rewrite_links(text: str, from_rel: Path) -> str:
    def repl(m):
        prefix, target, suffix = m.group(1), m.group(2), m.group(3)
        if re.match(r"^[a-zA-Z][a-zA-Z0-9+.-]*://", target) or target.startswith(
            ("#", "mailto:")
        ):
            return m.group(0)
        # Split off an anchor fragment, if any.
        path_part, _, anchor = target.partition("#")
        if not path_part:
            return m.group(0)
        if not path_part.endswith(".md"):
            return m.group(0)
        # Resolve relative to src_root (the doc tree root), not the
        # process's cwd — from_rel is itself relative to src_root.
        resolved = (src_root / from_rel.parent / path_part).resolve()
        try:
            rel_target = resolved.relative_to(src_root.resolve())
        except ValueError:
            return m.group(0)
        flat = flat_map.get(rel_target)
        if flat is None:
            return m.group(0)
        # GitHub wikis (Gollum) serve pages at extension-less URLs
        # (/wiki/Page-Name). A link that keeps ".md" doesn't resolve to
        # the rendered page — it hits the raw file blob instead. Strip
        # it here; flat_map values keep ".md" because that's still the
        # correct on-disk filename for the wiki git repo.
        link_target = flat.removesuffix(".md")
        new_target = link_target + (f"#{anchor}" if anchor else "")
        return f"{prefix}{new_target}{suffix}"

    return link_re.sub(repl, text)


for p in md_files:
    rel = p.relative_to(src_root)
    text = p.read_text()
    text = rewrite_links(text, rel)
    out_path = out_root / flat_map[rel]
    out_path.write_text(text)
    print(f"  {rel} -> {flat_map[rel]}")

# --- _Sidebar.md, generated from SUMMARY.md's nested link list ---------
summary = (src_root / "SUMMARY.md").read_text()
sidebar_lines = ["# Navigation", ""]
item_re = re.compile(r"^(\s*)-\s*(?:\[([^\]]+)\]\(([^)]+)\)|(.+))$")
for line in summary.splitlines():
    m = item_re.match(line)
    if not m:
        continue
    indent, text, target, plain = m.groups()
    depth = len(indent) // 2
    bullet = "  " * depth + "-"
    if target:
        path_part, _, anchor = target.partition("#")
        try:
            rel_target = Path(path_part)
            flat = flat_map.get(rel_target)
        except Exception:
            flat = None
        if flat:
            sidebar_lines.append(f"{bullet} [{text}]({flat.removesuffix('.md')})")
        else:
            sidebar_lines.append(f"{bullet} {text}")
    elif plain:
        sidebar_lines.append(f"{bullet} {plain.strip()}")

(out_root / "_Sidebar.md").write_text("\n".join(sidebar_lines) + "\n")
print("  SUMMARY.md -> _Sidebar.md")
PYEOF

echo
echo "== Done: $(find "$OUT" -name '*.md' | wc -l | tr -d ' ') pages in $OUT =="
echo
echo "To publish to the GitHub wiki:"
echo "  git clone git@github.com:moodoki/graphite_picocalc_gc.wiki.git /tmp/gpc-wiki"
echo "  rsync -a --delete --exclude .git $OUT/ /tmp/gpc-wiki/"
echo "  cd /tmp/gpc-wiki && git add -A && git commit -m 'Sync wiki from docs-site/' && git push"
echo
echo "See docs-site/README.md for the CI-driven alternative (needs a WIKI_TOKEN secret)."
