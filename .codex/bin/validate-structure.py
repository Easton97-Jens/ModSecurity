#!/usr/bin/env python3
"""Den zugeordneten Kontextaufbau dieses Forks prüfen, ohne Dateien zu ändern."""

import json
import sys
from pathlib import Path

try:
    import tomllib
except ImportError:
    print("FEHLER: Für die TOML-Prüfung wird Python 3.11 oder neuer benötigt.", file=sys.stderr)
    sys.exit(2)

ROOT = Path(__file__).resolve().parents[2]
GROUPS = (
    "technical_guides", "primary_policies", "profiles", "components",
    "security_checks", "session_context", "supporting_context", "tools",
)


def load(relative):
    with (ROOT / relative).open("rb") as handle:
        return tomllib.load(handle)


def local_path(relative):
    if not isinstance(relative, str) or not relative or Path(relative).is_absolute():
        raise ValueError(f"Ein nicht leerer repository-relativer Pfad wird erwartet: {relative!r}")
    path = (ROOT / relative).resolve()
    if not path.is_relative_to(ROOT):
        raise ValueError(f"Der Pfad verlässt das Repository: {relative}")
    return path


def main():
    manifest = load(".codex/context/manifest.toml")
    structure = load(".codex/structure-manifest.toml")
    errors = []
    for name, data in (("context", manifest), ("structure", structure)):
        if data.get("schema_version") != 1:
            errors.append(f"{name}: nicht unterstützte schema_version")
    if structure.get("repository") != "Easton97-Jens/ModSecurity":
        errors.append("Unerwartete Repository-Identität im Strukturmanifest")
    if structure.get("children") != []:
        errors.append("Dieses Schema unterstützt keine aktive Vererbung an untergeordnete Repositories")
    for relative in structure.get("required_files", []):
        if not local_path(relative).is_file():
            errors.append(f"Erforderliche Datei fehlt: {relative}")
    for relative in structure.get("required_directories", []):
        if not local_path(relative).is_dir():
            errors.append(f"Erforderliches Verzeichnis fehlt: {relative}")
    routed = set()
    for group in GROUPS:
        rows = manifest.get(group)
        if not isinstance(rows, dict) or not rows:
            errors.append(f"Fehlende oder leere Zuordnungsgruppe: {group}")
            continue
        for key, relative in rows.items():
            path = local_path(relative)
            if not path.is_file():
                errors.append(f"Fehlende Zuordnung {group}.{key}: {relative}")
            if relative in routed:
                errors.append(f"Doppeltes Zuordnungsziel: {relative}")
            routed.add(relative)
    for relative in structure.get("inventory_roots", []):
        actual = {
            p.relative_to(ROOT).as_posix()
            for p in local_path(relative).rglob("*.md") if p.is_file()
        }
        for missing in sorted(actual - routed):
            errors.append(f"Markdown-Datei hat keine Manifestzuordnung: {missing}")
    agents_path = local_path(manifest["root_instruction"])
    agents = agents_path.read_text("utf-8")
    marker = structure["routing_marker"]
    if marker not in agents:
        errors.append(f"AGENTS.md fehlt die Zuordnungsmarkierung: {marker}")
    if len(agents.splitlines()) > 150:
        errors.append("AGENTS.md überschreitet die Grenze von 150 Zeilen für einen knappen Einstieg")
    if manifest["index"] != ".codex/context/index.md":
        errors.append("Unerwarteter Pfad zum Kontextverzeichnis")
    lock = json.loads((ROOT / ".codex/context/upstream-lock.json").read_text("utf-8"))
    template = lock.get("structure_template", {})
    if template.get("repository") != structure.get("template_repository"):
        errors.append("Vorlagenrepository unterscheidet sich zwischen Manifesten und Lockdatei")
    if template.get("commit") != structure.get("template_commit"):
        errors.append("Vorlagenrevision unterscheidet sich zwischen Manifesten und Lockdatei")
    for error in errors:
        print("FEHLER: " + error, file=sys.stderr)
    if errors:
        print(f"ERGEBNIS: {len(errors)} Strukturfehler", file=sys.stderr)
        return 1
    print(f"OK: Kontextstruktur und {len(routed)} zugeordnete Dateien sind konsistent.")
    print("GRENZE: Geprüft wird die Dateizuordnung, nicht die inhaltliche Richtigkeit oder das Laufzeitverhalten.")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, TypeError) as exc:
        print("FEHLER: " + str(exc), file=sys.stderr)
        sys.exit(2)
