#!/usr/bin/env python3
"""Validate .github/ISSUE_TEMPLATE/*.yml against GitHub's issue-form schema.

Worth having because of how GitHub fails: a malformed issue form is not
reported anywhere in the UI: the template silently stops appearing in the
"New issue" chooser. You find out when someone tells you the form is missing,
which on a low-traffic repo could be months.

This checks the parts that actually break in practice — required top-level
keys, valid element types, ids that are unique and present where GitHub needs
them, and dropdowns that have options.

Usage:
    ./scripts/validate-issue-templates.py            # checks .github/ISSUE_TEMPLATE/
    ./scripts/validate-issue-templates.py <dir>

Needs PyYAML (see requirements-dev.txt).
"""

import sys
from pathlib import Path

try:
    import yaml
except ImportError:
    sys.exit(
        "PyYAML not installed. In the project venv:\n"
        "  python3 -m venv .venv && .venv/bin/pip install -r requirements-dev.txt"
    )

# https://docs.github.com/en/communities/using-templates-to-encourage-useful-
# issues-and-pull-requests/syntax-for-githubs-form-schema
VALID_TYPES = {"markdown", "input", "textarea", "dropdown", "checkboxes"}
NEEDS_ID = {"input", "textarea", "dropdown", "checkboxes"}


def check_form(doc):
    errs = []
    if not isinstance(doc, dict):
        return [f"top level is {type(doc).__name__}, expected a mapping"]

    for key in ("name", "description", "body"):
        if key not in doc:
            errs.append(f"missing required top-level key: {key}")
    if not isinstance(doc.get("body"), list):
        return errs + ["`body` must be a list"]

    seen = set()
    for i, el in enumerate(doc["body"]):
        where = f"body[{i}]"
        if not isinstance(el, dict):
            errs.append(f"{where}: not a mapping")
            continue

        t = el.get("type")
        if t not in VALID_TYPES:
            errs.append(f"{where}: type {t!r} not one of {sorted(VALID_TYPES)}")
            continue

        if t in NEEDS_ID:
            eid = el.get("id")
            if not eid:
                errs.append(f"{where}: type {t!r} needs an id")
            elif eid in seen:
                errs.append(f"{where}: duplicate id {eid!r}")
            else:
                seen.add(eid)

        attrs = el.get("attributes")
        if not isinstance(attrs, dict):
            errs.append(f"{where}: missing `attributes`")
            continue
        if t == "markdown":
            if not attrs.get("value"):
                errs.append(f"{where}: markdown needs attributes.value")
        elif not attrs.get("label"):
            errs.append(f"{where}: {t} needs attributes.label")

        if t == "dropdown" and not attrs.get("options"):
            errs.append(f"{where}: dropdown needs attributes.options")
        if t == "checkboxes" and not attrs.get("options"):
            errs.append(f"{where}: checkboxes needs attributes.options")

    return errs


def check_config(doc):
    errs = []
    if not isinstance(doc, dict):
        return ["config.yml top level must be a mapping"]
    for i, link in enumerate(doc.get("contact_links") or []):
        for key in ("name", "url", "about"):
            if not link.get(key):
                errs.append(f"contact_links[{i}]: missing {key}")
    return errs


def main():
    root = Path(sys.argv[1] if len(sys.argv) > 1 else ".github/ISSUE_TEMPLATE")
    if not root.is_dir():
        sys.exit(f"not a directory: {root}")

    files = sorted(p for p in root.iterdir() if p.suffix in (".yml", ".yaml"))
    if not files:
        sys.exit(f"no templates found in {root}")

    bad = 0
    for path in files:
        try:
            doc = yaml.safe_load(path.read_text())
        except yaml.YAMLError as e:
            print(f"{path}: FAIL — not valid YAML\n  {e}")
            bad += 1
            continue

        errs = check_config(doc) if path.name == "config.yml" else check_form(doc)
        if errs:
            bad += 1
            print(f"{path}: {len(errs)} issue(s)")
            for e in errs:
                print(f"  {e}")
        else:
            print(f"{path}: OK")

    print(f"\nValidated {len(files)} template(s), {bad} with issues.")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
