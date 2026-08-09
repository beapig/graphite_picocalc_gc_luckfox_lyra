#!/usr/bin/env python3
"""Generate docs-site/reference/*.md from firmware source.

Firmware stays the source of truth for the function catalog and the
built-in HELP screen's key/syntax reference text; these markdown pages
are derived from it so they can't silently drift out of sync.

Sources:
  - src/math/catalog.cpp        -> docs-site/reference/function-catalog.md
  - src/apps/help_screen.cpp    -> docs-site/reference/key-reference.md
                                   docs-site/reference/syntax-reference.md

Idempotent: running this twice produces byte-identical output (verify
with `git diff --exit-code docs-site/reference/`). Stdlib-only.

Usage:
    python3 scripts/gen-doc-reference.py
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CATALOG_SRC = ROOT / "src" / "math" / "catalog.cpp"
HELP_SRC = ROOT / "src" / "apps" / "help_screen.cpp"
OUT_DIR = ROOT / "docs-site" / "reference"

GENERATED_NOTICE = (
    "> **Generated file — do not edit by hand.** Regenerate with\n"
    "> `python3 scripts/gen-doc-reference.py` (source: `{src}`).\n"
)


def escape_cell(text: str) -> str:
    """Escape a value for use inside a markdown table cell."""
    return text.replace("\\", "\\\\").replace("|", "\\|")


def backtick(text: str) -> str:
    """Wrap in backticks for inline code, escaping any literal backtick."""
    if "`" in text:
        # Extremely unlikely in this source, but stay safe rather than
        # emit a malformed code span.
        text = text.replace("`", "'")
    return f"`{text}`"


# ---------------------------------------------------------------------------
# 1. Function catalog
# ---------------------------------------------------------------------------

ENTRY_RE = re.compile(
    r'\{\s*"(?P<name>[^"]*)"\s*,\s*"(?P<sig>[^"]*)"\s*,\s*"(?P<desc>[^"]*)"\s*,'
    r"\s*(?P<fnexpr>.*?)\s*,\s*(?P<arity>\d+)\s*\}\s*,?"
)


def parse_catalog(src_path: Path) -> list[dict[str, str]]:
    text = src_path.read_text()

    start = text.index("kCatalog[]")
    array_start = text.index("{", start)
    # Find the matching closing brace for the array initializer by
    # brace-depth counting from array_start.
    depth = 0
    i = array_start
    end = None
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                end = i
                break
        i += 1
    if end is None:
        raise ValueError(f"unbalanced braces parsing kCatalog in {src_path}")

    body = text[array_start + 1 : end]

    entries = []
    for line in body.split("\n"):
        stripped = line.strip()
        if not stripped or stripped.startswith("//"):
            continue
        m = ENTRY_RE.search(stripped)
        if not m:
            continue
        entries.append(
            {
                "name": m.group("name"),
                "sig": m.group("sig"),
                "desc": m.group("desc"),
                "arity": m.group("arity"),
            }
        )
    return entries


def render_function_catalog(entries: list[dict[str, str]]) -> str:
    lines = [
        "# Function Catalog",
        "",
        GENERATED_NOTICE.format(src="src/math/catalog.cpp").rstrip(),
        "",
        "Every function the expression parser recognizes, in the same order",
        "as the on-device `F5` HELP screen's FUNC tab (it reads this exact",
        "table).",
        "",
        "| Name | Signature | Description | Arity |",
        "|---|---|---|---|",
    ]
    for e in entries:
        lines.append(
            "| {name} | {sig} | {desc} | {arity} |".format(
                name=backtick(escape_cell(e["name"])),
                sig=backtick(escape_cell(e["sig"])),
                desc=escape_cell(e["desc"]),
                arity=e["arity"],
            )
        )
    lines.append("")
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# 2. Key reference / syntax reference (help_screen.cpp curated line arrays)
# ---------------------------------------------------------------------------

STRING_RE = re.compile(r'"((?:[^"\\]|\\.)*)"')


def parse_line_array(text: str, array_name: str) -> list[str]:
    start = text.index(f"{array_name}[]")
    array_start = text.index("{", start)
    depth = 0
    i = array_start
    end = None
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                end = i
                break
        i += 1
    if end is None:
        raise ValueError(f"unbalanced braces parsing {array_name} in help_screen.cpp")

    body = text[array_start + 1 : end]

    out = []
    for raw_line in body.split("\n"):
        stripped = raw_line.strip()
        if not stripped or stripped.startswith("//"):
            continue
        m = STRING_RE.search(raw_line)
        if not m:
            continue
        literal = unescape_c(m.group(1))
        out.append(literal)
    return out


# The firmware's 8x16 font puts math symbols in slots 127..141, above ASCII,
# and the help text reaches them with octal escapes ("2\\2142" is 2-radical-2).
# Decoding those to the real Unicode characters is the difference between a
# docs page reading "2\\2142" and reading "2√2". Source of truth for the slot
# map is src/gfx/font.hpp — keep these in step if it changes.
GLYPH_SLOTS = {
    0x7F: "\u03c0",  # pi
    0x80: "\u2220",  # angle sign (polar phasor)
    0x81: "\u03b8",  # theta
    0x82: "\u03c3",  # sigma
    0x83: "\u03a3",  # Sigma
    0x84: "\u03c7",  # chi
    0x85: "\u03bc",  # mu
    0x86: "\U0001d456",  # slanted imaginary-unit i
    0x87: "\u21d2",  # store arrow
    0x88: "\u03bb",  # lambda
    0x89: "\u2260",  # not-equal
    0x8A: "\u2026",  # horizontal ellipsis
    0x8B: "\u00b2",  # superscript two
    0x8C: "\u221a",  # square-root radical
    0x8D: "\u2093",  # subscript x
}

C_ESCAPE_RE = re.compile(r"\\([0-7]{1,3})|\\x([0-9a-fA-F]{1,2})|\\(.)")


def unescape_c(literal: str) -> str:
    """Decode the C-string escapes the help tables actually use.

    Octal and hex escapes are mapped through GLYPH_SLOTS so a font slot
    becomes the character it draws, rather than leaking as "\\214" into a
    page someone is meant to read.
    """

    def sub(m: "re.Match[str]") -> str:
        octal, hexa, simple = m.group(1), m.group(2), m.group(3)
        if octal is not None:
            code = int(octal, 8)
        elif hexa is not None:
            code = int(hexa, 16)
        else:
            # Exactly one of the three alternatives matched, so `simple` is a
            # str here; anything not in the table passes through as itself.
            return {"n": "\n", "t": "\t", '"': '"', "\\": "\\"}.get(simple, simple)
        return GLYPH_SLOTS.get(code, chr(code))

    return C_ESCAPE_RE.sub(sub, literal)


def render_line_reference(title: str, src_label: str, lines_in: list[str]) -> str:
    """Turn a curated, screen-wrapped line array into markdown.

    Lines starting with '#' become '## ' section headings (curated
    section names, kept verbatim). Within a section, a line that starts
    with a literal leading space in the source string is an unambiguous
    wrapped continuation of the previous line (the device wraps display
    text to ~26-30 columns) and is merged into it. Any other line is
    kept as its own list item verbatim — merging non-indented lines
    would mean guessing where the on-device wrapping was purely
    cosmetic vs. a real line break, which the source doesn't disambiguate.
    """
    out = [
        f"# {title}",
        "",
        GENERATED_NOTICE.format(src=src_label).rstrip(),
        "",
        "Mirrors the on-device `F5` HELP screen's tab of the same name.",
        "Lines are curated for a narrow on-device display column; wrapped",
        "continuations (marked by a leading space in the source) are",
        "rejoined below, everything else is kept as its own line verbatim.",
        "",
    ]

    items: list[str] = []
    for raw in lines_in:
        if raw.startswith("#"):
            if items:
                for it in items:
                    out.append(f"- {backtick(it)}")
                out.append("")
                items = []
            out.append(f"## {raw[1:].strip()}")
            out.append("")
        elif raw.startswith(" "):
            if items:
                items[-1] = items[-1] + " " + raw.strip()
            else:
                items.append(raw.strip())
        else:
            items.append(raw)
    if items:
        for it in items:
            out.append(f"- {backtick(it)}")
        out.append("")

    return "\n".join(out).rstrip() + "\n"


def main() -> int:
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    catalog_entries = parse_catalog(CATALOG_SRC)
    (OUT_DIR / "function-catalog.md").write_text(
        render_function_catalog(catalog_entries)
    )

    help_text = HELP_SRC.read_text()
    keys_lines = parse_line_array(help_text, "kKeysLines")
    syntax_lines = parse_line_array(help_text, "kSyntaxLines")

    (OUT_DIR / "key-reference.md").write_text(
        render_line_reference(
            "Key Reference", "src/apps/help_screen.cpp (kKeysLines)", keys_lines
        )
    )
    (OUT_DIR / "syntax-reference.md").write_text(
        render_line_reference(
            "Syntax Reference",
            "src/apps/help_screen.cpp (kSyntaxLines)",
            syntax_lines,
        )
    )

    print(f"Wrote {len(catalog_entries)} catalog entries -> function-catalog.md")
    print(f"Wrote {len(keys_lines)} key-reference lines -> key-reference.md")
    print(f"Wrote {len(syntax_lines)} syntax-reference lines -> syntax-reference.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
