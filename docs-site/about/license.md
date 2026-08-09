# License

This is a personal project for fun, education, and sharing.

- **The project's own code** (application source, tests, scripts, docs)
  is **MIT-licensed** — see the repository's `LICENSE` file. Reuse the
  math engine, graph subsystem, or renderer freely.
- **The combined firmware binary** links GPL-2.0 vendored drivers
  (display and keyboard drivers, and their bitmap font, from Coyote OS),
  so the firmware as distributed — the flashable `.uf2` — is under
  **GPL-2.0** as a whole. See the repository's `NOTICE.md` for the full
  per-component license table.

In short: if you're reading or reusing the source code, it's MIT. If
you're flashing and running the built firmware, the combined binary is
GPL-2.0 because of the vendored drivers it links.
