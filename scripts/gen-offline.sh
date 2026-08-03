#!/usr/bin/env bash
# Build an offline-readable bundle from docs-site/ into build/offline/
# (gitignored).
#
# Always produces a single concatenated markdown file, in SUMMARY.md
# order, using only python3/shell — that output is guaranteed available
# with no external dependency. If pandoc is on PATH, also produces a
# single-file self-contained HTML and a PDF from the same concatenation.
# If pandoc is absent, prints instructions and exits 0 (not a failure —
# the markdown bundle alone is still useful).
#
# Usage: ./scripts/gen-offline.sh

set -euo pipefail

cd "$(dirname "$0")/.."

SRC=docs-site
OUT=build/offline
BUNDLE_NAME=picocalc-gc-guide
MD_OUT="$OUT/$BUNDLE_NAME.md"

echo "== Cleaning $OUT =="
rm -rf "$OUT"
mkdir -p "$OUT"

echo "== Concatenating $SRC (SUMMARY.md order) -> $MD_OUT =="
python3 - "$SRC" "$MD_OUT" <<'PYEOF'
import re
import sys
from pathlib import Path

src_root = Path(sys.argv[1])
out_path = Path(sys.argv[2])

summary_text = (src_root / "SUMMARY.md").read_text()
link_re = re.compile(r"\[[^\]]+\]\(([^)]+\.md)(?:#[^)]*)?\)")
rels = []
for m in link_re.finditer(summary_text):
    rel = Path(m.group(1))
    if rel not in rels:
        rels.append(rel)

missing = [r for r in rels if not (src_root / r).is_file()]
if missing:
    print("ERROR: SUMMARY.md links to missing file(s):", file=sys.stderr)
    for r in missing:
        print(f"  {r}", file=sys.stderr)
    sys.exit(1)

# Local .md links get rewritten to same-document anchors, since the
# bundle is a single file. Anchors are derived the same way GitHub/most
# renderers derive them from headings: lowercase, spaces -> hyphens,
# strip anything that isn't alnum/space/hyphen.
def anchor_for(rel: Path) -> str:
    text = (src_root / rel).read_text()
    for line in text.splitlines():
        if line.startswith("# "):
            title = line[2:].strip()
            slug = title.lower()
            slug = re.sub(r"[^a-z0-9 \-]", "", slug)
            slug = slug.strip().replace(" ", "-")
            return slug
    return rel.stem

anchor_map = {rel: anchor_for(rel) for rel in rels}

local_link_re = re.compile(r"(\[[^\]]+\]\()([^)]+\.md)((?:#[^)]*)?\))")


def rewrite(text: str, from_rel: Path) -> str:
    def repl(m):
        prefix, target, suffix = m.groups()
        path_part = target
        resolved = (src_root / from_rel.parent / path_part).resolve()
        try:
            rel_target = resolved.relative_to(src_root.resolve())
        except ValueError:
            return m.group(0)
        anchor = anchor_map.get(rel_target)
        if anchor is None:
            return m.group(0)
        return f"{prefix}#{anchor}{suffix}"

    return local_link_re.sub(repl, text)


parts = []
parts.append("# PicoCalc GraphCalc — Offline Guide\n")
parts.append(
    "Concatenated from `docs-site/` in `SUMMARY.md` order by "
    "`scripts/gen-offline.sh`. Section headings below correspond to "
    "individual pages on the wiki/site.\n"
)
for rel in rels:
    text = (src_root / rel).read_text()
    text = rewrite(text, rel)
    parts.append(f"\n\n<!-- ---- source: docs-site/{rel} ---- -->\n\n")
    parts.append(text.rstrip() + "\n")

out_path.write_text("\n".join(parts))
print(f"  {len(rels)} pages concatenated -> {out_path}")
PYEOF

echo "== Markdown bundle: $MD_OUT =="
wc -l "$MD_OUT"

if ! command -v pandoc &>/dev/null; then
  echo
  echo "pandoc not found on PATH — skipping HTML/PDF output."
  echo "Install it with: brew install pandoc"
  echo "(the markdown bundle above is still complete and usable)"
  exit 0
fi

echo
echo "== pandoc found ($(pandoc --version | head -1)); building HTML + PDF =="

HTML_OUT="$OUT/$BUNDLE_NAME.html"
# --embed-resources is the current flag name; older pandocs only know
# --self-contained. Try the modern one first.
if pandoc --standalone --embed-resources -o "$HTML_OUT" "$MD_OUT" 2>/dev/null; then
  echo "  HTML (--embed-resources) -> $HTML_OUT"
elif pandoc --standalone --self-contained -o "$HTML_OUT" "$MD_OUT"; then
  echo "  HTML (--self-contained) -> $HTML_OUT"
else
  echo "WARNING: pandoc HTML build failed." >&2
fi

PDF_OUT="$OUT/$BUNDLE_NAME.pdf"
if pandoc -o "$PDF_OUT" "$MD_OUT" 2>/dev/null; then
  echo "  PDF -> $PDF_OUT"
else
  echo "WARNING: pandoc PDF build failed (needs a LaTeX engine, e.g. 'brew install --cask basictex')." >&2
fi

echo
echo "Done. Outputs in $OUT/"
