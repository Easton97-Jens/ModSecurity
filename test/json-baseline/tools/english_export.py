#!/usr/bin/env python3
"""Localize only a fresh companion export; never edit the source checkout.

The full translations already exist under doc/en. This helper relocates them,
removes German-language navigation/documents from the English package, and
records the immutable origin of the historical manifest. It does not translate
or rewrite test data, fixtures, assertions, IDs, source SHAs or reference names.
The fork-only exporter calls localize_english before initializing the new repo.
"""
import hashlib
import json
import os
from pathlib import Path
import re
import shutil

BASELINE = '2bfb2180d8c0fa35ae7a3f2665e999600607d896'
BASELINE_URL = 'https://github.com/Easton97-Jens/ModSecurity/blob/' + BASELINE + '/'
DOCUMENTS = ('JSON-YAJL-BASELINE.md', 'json-yajl-baseline-v3.md',
             'json-yajl-code-analysis.md')
PACKAGING_TOOLS = ('test/json-baseline/tools/export_repositories.py',
                   'test/json-baseline/tools/english_export.py')


def _sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _relative_links(text, old_file, new_file, relocations):
    """Resolve local Markdown links at their old location, then relocate them."""
    pattern = re.compile(r'(\[[^\]\n]+\]\()([^\s)]+)(\))')

    def replace(match):
        target = match.group(2)
        if target.startswith(('#', '/', 'http:', 'https:', 'mailto:', 'data:')):
            return match.group(0)
        path, marker, fragment = target.partition('#')
        old_target = (old_file.parent / path).resolve()
        new_target = relocations.get(old_target, old_target)
        new_path = os.path.relpath(new_target, new_file.parent).replace(os.sep, '/')
        return match.group(1) + new_path + (marker + fragment if marker else '') + match.group(3)

    return pattern.sub(replace, text)


def _english_placeholders(text):
    # Explanatory command placeholders, not literal fixture/reference values.
    return text.replace('<vollständiger Original-SHA>', '<FULL-ORIGINAL-SHA>').replace(
        '<VERIFIZIERTE-AUFGABEN-SHA>', '<VERIFIED-TASK-SHA>')


def _english_navigation(text):
    """Remove links to the deliberately excluded German documents."""
    text = re.sub(r'\[[^\]\n]+\]\((?:[^)\n]*\.de\.md(?:#[^)\n]*)?|[^)\n]*/de/[^)\n]*)\)',
                  '', text)
    # Remove the separator left by a language switch without changing body data.
    text = re.sub(r'(?m)^\s*·\s*', '', text)
    text = re.sub(r'(?m)\s*·\s*$', '', text)
    text = re.sub(r'(?m)^\.\s*$', '', text)
    return text


def _repository_readme(text):
    text = text.replace(
        'and **all explanatory documents in German and English**. Both language '
        'repositories contain identical tests and documentation; only the primary entry language differs.',
        'and **all explanatory documents in English**. The separately prepared '
        'German companion retains both language versions; this package contains '
        'English documentation only. The executable tests are identical.')
    text = text.replace(' The  is included too.', '')
    text = text.replace(
        ' The [complete German documentation](doc/json-pr-extension/README.de.md#vollständige-dokumentation) is included too.',
        '')
    return text


def _extension_readme(text):
    old = ('Each prepared repository contains the same tests and the complete German and\n'
           'English documentation; its entry page selects the main language. This is\n'
           'separate from any publication of the task branch in the existing ModSecurity fork.')
    new = ('The English companion contains the same executable tests and complete English\n'
           'documentation. The separately prepared German companion retains German and\n'
           'English documentation. This is separate from any publication of the task\n'
           'branch in the existing ModSecurity fork.')
    text = text.replace(old, new)
    heading = '## Complete documentation in both languages'
    if heading in text:
        start = text.index(heading)
        end = text.find('\n## ', start + len(heading))
        if end < 0:
            end = len(text)
        replacement = '''## Complete documentation in English

This package contains the complete English translations of the prior baseline,
including every original section, table, code/source reference, historical count
and caveat. Historical reports are not silently rewritten with extension results.
The separate German companion retains both language versions.

| Document | Complete English version |
|---|---|
| Original baseline overview | [Complete report](../JSON-YAJL-BASELINE.md) |
| How requests work, technical analysis and fzipi statement checks | [Complete report](../json-yajl-code-analysis.md) |
| Original executable v3 matrix, environment and reproduction | [Complete report](../json-yajl-baseline-v3.md) |
| Original ModSecurity project README | [Complete historical original](../reference/en/ModSecurity-README.md) |
| Extension and execution | [This report](README.en.md) |
| Source-to-case coverage matrix | [Complete matrix](matrix.en.md) |

The [historical provenance manifest](../json-yajl-provenance.json), new
[provenance](provenance.json), [catalog](catalog.json) and
[result summary](results-summary.json) are shared machine-readable evidence.
The historical manifest records its original source URL and SHA-256; literal
reference names, paths, SHAs and observations remain unchanged. Original Doxygen
configuration and logo remain language-neutral archival references, requiring
the full original source tree to generate API docs.
'''
        text = text[:start] + replacement + text[end:]
    text = text.replace(
        '`export_repositories.py` requires the full original\n'
        'fork history and is a packaging helper, not the standalone build entry point.',
        'The packaging helpers belong to the [full ModSecurity fork]'
        '(https://github.com/Easton97-Jens/ModSecurity), require its original history, '
        'and are excluded from this English companion. Use the standalone build '
        'and run helpers included here.')
    return text


def _matrix(text):
    text = text.replace('The v2 analysis remains in the\nbilingual archive;',
                        'The v2 analysis remains in the\nhistorical English documentation;')
    text = text.replace(
        'The [German original analysis](../json-yajl-code-analysis.md) and its\n'
        '[complete English translation](../en/json-yajl-code-analysis.md) provide\n'
        'commit-pinned code evidence for these distinctions.',
        'The [complete English analysis](../json-yajl-code-analysis.md) provides\n'
        'commit-pinned code evidence for these distinctions.')
    return text


def localize_english(root: Path) -> dict:
    """Return an audit of changes made only to an uninitialized fresh export.

    The destination must be outside the source checkout, contain the complete
    copy selected by the fork-only exporter, and have no Git metadata or links.
    Existing repositories and the canonical checkout are rejected before edits.
    """
    root = Path(root)
    source = Path(__file__).resolve().parents[3]
    if root.is_symlink():
        raise ValueError('English export root must not be a symbolic link')
    root = root.resolve()
    if root == source or root.is_relative_to(source):
        raise ValueError('Refusing to localize the canonical source checkout')
    if not root.is_dir() or (root / '.git').exists():
        raise ValueError('English localization requires a fresh export before git init')
    if any(path.is_symlink() for path in root.rglob('*')):
        raise ValueError('English export must not contain symbolic links')
    required = [root / 'doc/en' / name for name in DOCUMENTS]
    required += [root / 'doc' / name for name in DOCUMENTS]
    required += [root / 'doc/json-yajl-provenance.json',
                 root / 'doc/json-pr-extension/README.en.md',
                 root / 'doc/json-pr-extension/matrix.en.md',
                 root / 'test/json-baseline/run.py',
                 root / 'test/json-baseline/tools/export_repositories.py']
    if any(not path.is_file() for path in required):
        raise ValueError('English localization requires the complete fresh companion copy')

    # The runnable tests and their synthetic Unicode bytes are never localized.
    runtime = {str(path.relative_to(root)): _sha256(path)
               for path in (root / 'test').rglob('*')
               if path.is_file() and str(path.relative_to(root)) not in PACKAGING_TOOLS}
    original_manifest = root / 'doc/json-yajl-provenance.json'
    original_manifest_hash = _sha256(original_manifest)
    original_manifest_data = json.loads(original_manifest.read_text())
    metadata = {'schema': 1, 'language': 'English only',
                'canonical_source_modified': False,
                'translated_documents': [], 'removed_paths': [],
                'historical_provenance': {
                    'source_commit': BASELINE,
                    'source_url': BASELINE_URL + 'doc/json-yajl-provenance.json',
                    'original_sha256': original_manifest_hash,
                    'human_prose_translations': [],
                    'note': 'The original manifest human-readable prose is already English. '
                            'All historical SHAs, references, paths, fixtures and observations '
                            'are preserved, including literal branch names in other languages.'}}
    relocations = {(root / 'doc/en' / name).resolve(): (root / 'doc' / name).resolve()
                   for name in DOCUMENTS}
    for name in DOCUMENTS:
        old = root / 'doc/en' / name
        new = root / 'doc' / name
        translation = _english_placeholders(old.read_text())
        relocated = _relative_links(translation, old, new, relocations)
        new.write_text(relocated)
        old.write_text(translation)
        metadata['translated_documents'].append({
            'path': str(new.relative_to(root)),
            'full_translation_source': str(old.relative_to(root)),
            'translation_source_sha256': _sha256(old),
            'source_original_url': BASELINE_URL + 'doc/' + name,
            'note': 'Complete existing English translation; only local links and '
                    'human-readable command placeholders adapted for the export.'})
    manifest_data = dict(original_manifest_data)
    manifest_data['english_export'] = metadata['historical_provenance']
    original_manifest.write_text(json.dumps(manifest_data, indent=2, ensure_ascii=False) + '\n')
    # Historical machine data must be identical after removing the new origin note.
    check_manifest = json.loads(original_manifest.read_text())
    check_manifest.pop('english_export')
    if check_manifest != original_manifest_data:
        raise RuntimeError('English export changed historical provenance values')

    # Source statements about a bilingual package must match the English export.
    for path in sorted(root.rglob('*.md')):
        relative = path.relative_to(root).as_posix()
        if '.de.md' in path.name or '/de/' in '/' + relative:
            continue
        text = path.read_text()
        if path.name in ('README.md', 'README.en.md') or path.name == 'repository-README.en.md':
            text = _repository_readme(text)
        if relative == 'doc/json-pr-extension/README.en.md':
            text = _extension_readme(text)
        if relative == 'doc/json-pr-extension/matrix.en.md':
            text = _matrix(text)
        if relative == 'doc/json-pr-extension/repository-README.en.md':
            # This template was authored for the repository root. Its retained
            # documentary copy needs working links at its nested location.
            text = _relative_links(text, root / 'README.en.md', path, relocations)
        text = _english_placeholders(_english_navigation(text))
        path.write_text(text)

    # This only removes copies in the validated fresh export, never source docs.
    remove = [path for path in root.rglob('*.de.md') if path.is_file()]
    german_reference = root / 'doc/reference/de'
    if german_reference.exists():
        metadata['removed_paths'].extend(str(path.relative_to(root))
                                         for path in german_reference.rglob('*') if path.is_file())
        shutil.rmtree(german_reference)
    for relative in PACKAGING_TOOLS:
        path = root / relative
        if path.exists():
            remove.append(path)
    for path in sorted(remove):
        metadata['removed_paths'].append(str(path.relative_to(root)))
        path.unlink()
    for relative, expected in runtime.items():
        if _sha256(root / relative) != expected:
            raise RuntimeError('English localization changed runnable tests: ' + relative)
    metadata['runtime_files_verified_identical'] = len(runtime)
    if any(root.rglob('*.de.md')) or german_reference.exists():
        raise RuntimeError('German-language documentation remains in English export')
    for path in root.rglob('*.md'):
        text = path.read_text()
        if re.search(r'\[[^\]\n]+\]\([^\n)]*(?:\.de\.md|/de/)', text):
            raise RuntimeError('English export contains a removed-language link: ' + str(path))
        if re.search(r'\b(?:Deutsche Fassung|vollständiger Original|VERIFIZIERTE-AUFGABEN|vollständige Dokumentation)\b', text):
            raise RuntimeError('English documentation retains untranslated navigation: ' + str(path))
    metadata['removed_paths'] = sorted(set(metadata['removed_paths']))
    return metadata
