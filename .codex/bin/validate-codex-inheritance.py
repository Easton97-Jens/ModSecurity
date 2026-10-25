#!/usr/bin/env python3
"""Den lokalen Richtlinien-Prüfsummenkatalog prüfen oder bewusst aktualisieren.

Es sind keine untergeordneten Repositories aktiv. Dieses Hilfsprogramm lädt keine
Dateien herunter, ändert keinen Quellcode und lässt Codex keine Anweisungen laden.
Nur --refresh schreibt den lokalen Katalog.
"""

import argparse
import hashlib
import json
import sys
from pathlib import Path

try:
    import tomllib
except ImportError:
    print("FEHLER: Für die TOML-Prüfung wird Python 3.11 oder neuer benötigt.", file=sys.stderr)
    sys.exit(2)

ROOT = Path(__file__).resolve().parents[2]
CATALOG = ROOT / ".codex/inheritance-manifest.toml"


def load(path):
    with path.open("rb") as handle:
        return tomllib.load(handle)


def expected_rows():
    routes = load(ROOT / ".codex/context/manifest.toml")
    policies = routes["primary_policies"]
    if not isinstance(policies, dict) or not policies:
        raise ValueError("Keine primären Richtlinien sind zugeordnet")
    rows = []
    identifiers = set()
    sources = set()
    for key, relative in sorted(policies.items()):
        if not isinstance(key, str) or not key:
            raise ValueError(f"Ungültiger Schlüssel der Richtlinienzuordnung: {key!r}")
        identifier = "MODSECURITY-" + key.upper().replace("_", "-")
        if identifier in identifiers:
            raise ValueError(f"Doppelte abgeleitete Richtlinien-ID: {identifier}")
        identifiers.add(identifier)
        if not isinstance(relative, str) or Path(relative).is_absolute():
            raise ValueError(f"Ungültiger Richtlinienpfad: {relative!r}")
        path = (ROOT / relative).resolve()
        if not path.is_relative_to(ROOT) or not path.is_file():
            raise ValueError(f"Richtlinienpfad fehlt oder liegt außerhalb des Repositorys: {relative}")
        if path in sources:
            raise ValueError(f"Doppelte zugeordnete Richtlinienquelle: {relative}")
        sources.add(path)
        rows.append({
            "id": identifier,
            "source": relative,
            "sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
            "inheritable": True,
        })
    return rows


def validate_catalog_location():
    if CATALOG.is_symlink():
        raise ValueError("Der lokale Richtlinienkatalog darf kein symbolischer Link sein")
    if not CATALOG.resolve().is_relative_to(ROOT):
        raise ValueError("Der Pfad des lokalen Richtlinienkatalogs verlässt das Repository")


def refresh(rows):
    validate_catalog_location()
    lines = [
        "# Lokaler Richtlinienkatalog. Bewusst mit --refresh erzeugt.",
        "# Prüfsummen erfassen Inhalt, nicht Richtigkeit. Keine Vererbung an untergeordnete Repositories aktiv.",
        "schema_version = 1", 'repository = "Easton97-Jens/ModSecurity"',
        "children = []", "",
    ]
    for row in rows:
        lines.append("[[policies]]")
        for key in ("id", "source", "sha256"):
            lines.append(f"{key} = {json.dumps(row[key])}")
        lines.extend(["inheritable = true", ""])
    CATALOG.write_text("\n".join(lines), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--check", action="store_true", help="Den Katalog ausschließlich lesend prüfen.")
    mode.add_argument("--refresh", action="store_true", help="Nach Richtlinienprüfung ausschließlich den lokalen Katalog neu schreiben.")
    parser.add_argument("--json", action="store_true", help="Maschinenlesbare Ergebnisse ausgeben.")
    args = parser.parse_args()
    validate_catalog_location()
    expected = expected_rows()
    if args.refresh:
        refresh(expected)
    catalog = load(CATALOG)
    errors = []
    if catalog.get("schema_version") != 1:
        errors.append("Nicht unterstützte schema_version im Katalog")
    if catalog.get("repository") != "Easton97-Jens/ModSecurity":
        errors.append("Unerwartete Repository-Identität im Katalog")
    if catalog.get("children") != []:
        errors.append("Dieses Schema unterstützt keine aktive Vererbung an untergeordnete Repositories")
    actual = catalog.get("policies", [])
    if not isinstance(actual, list):
        raise ValueError("Die Richtlinien im Katalog müssen eine Liste bilden")
    observed = {}
    for row in actual:
        if not isinstance(row, dict) or not isinstance(row.get("id"), str):
            errors.append("Ungültiger Eintrag im Richtlinienkatalog")
            continue
        if row["id"] in observed:
            errors.append(f"Doppelte Richtlinien-ID: {row['id']}")
        observed[row["id"]] = row
    wanted = {row["id"]: row for row in expected}
    for identifier in sorted(wanted.keys() - observed.keys()):
        errors.append(f"Richtlinie fehlt im Katalog: {identifier}")
    for identifier in sorted(observed.keys() - wanted.keys()):
        errors.append(f"Nicht zugeordnete Richtlinie im Katalog: {identifier}")
    for identifier in sorted(wanted.keys() & observed.keys()):
        if wanted[identifier] != observed[identifier]:
            errors.append(f"Richtlinienpfad, Prüfsumme oder Metadaten sind veraltet: {identifier}")
    result = {
        "ok": not errors, "policies": len(expected),
        "active_child_repositories": 0, "refreshed": args.refresh, "errors": errors,
    }
    if args.json:
        print(json.dumps(result, indent=2))
    else:
        for error in errors:
            print("FEHLER: " + error, file=sys.stderr)
        if not errors:
            action = "Aktualisiert und geprüft:" if args.refresh else "Geprüft:"
            print(f"OK: {action} {len(expected)} lokale Richtlinien-Prüfsummen; keine aktive Vererbung an untergeordnete Repositories.")
    return 1 if errors else 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, TypeError) as exc:
        print("FEHLER: " + str(exc), file=sys.stderr)
        sys.exit(2)
