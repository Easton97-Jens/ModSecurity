#!/usr/bin/env python3
"""Prepare one English-only repo and one bilingual repo; never create remotes.

Requires the full fork history for immutable baseline-document comparisons.
Use prepare_source/build/run_all, not this exporter, in a standalone test repo.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

from english_export import localize_english

ROOT = Path(__file__).resolve().parents[3]
BASELINE = '2bfb2180d8c0fa35ae7a3f2665e999600607d896'


def git(root, *args, **kw):
    return subprocess.check_output(['git', '-C', str(root), *args], **kw)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--destination', type=Path, required=True,
                        help='absent directory outside the source checkout')
    parser.add_argument('--identity-source', type=Path, default=ROOT,
                        help='repository with an already configured, permitted Git identity')
    options = parser.parse_args()
    destination = options.destination.resolve()
    if options.destination.is_symlink() or destination.exists() or destination.is_relative_to(ROOT):
        parser.error('--destination must be absent and outside this checkout')
    identity = {}
    for field in ('name', 'email'):
        value = git(options.identity_source, 'config', '--get', 'user.' + field).decode().strip()
        if not value or '\n' in value:
            parser.error('existing Git user.' + field + ' is required')
        identity[field] = value
    destination.mkdir(parents=True)
    artifacts = destination / 'artifacts'
    artifacts.mkdir()
    selected = [ROOT / 'LICENSE']
    for path in (ROOT / 'doc').rglob('*'):
        relative = path.relative_to(ROOT)
        if path.is_file() and not path.is_symlink() and 'prepared-repositories' not in relative.parts:
            # Generated Doxygen/build products are never deliverables.
            if path.suffix in ('.md', '.json', '.png', '.cfg') or path.name in ('Makefile.am', '.empty'):
                selected.append(path)
    for path in (ROOT / 'test/json-baseline').rglob('*'):
        if not path.is_file() or path.is_symlink() or '__pycache__' in path.parts:
            continue
        if path.suffix in ('.cc', '.py', '.sh', '.json', '.yml'):
            selected.append(path)
    selected += [ROOT / 'test/test-cases/regression/request-body-parser-json.json']
    selected = sorted(set(selected))
    if len(selected) != len({str(path.relative_to(ROOT)) for path in selected}):
        raise RuntimeError('Duplicate export paths')
    # Historical baseline documents must remain present and byte-identical.
    original_documents = git(ROOT, 'ls-tree', '-r', '--name-only', BASELINE, 'doc').decode().splitlines()
    for relative in original_documents:
        expected = git(ROOT, 'show', BASELINE + ':' + relative)
        if (ROOT / relative).read_bytes() != expected or ROOT / relative not in selected:
            raise RuntimeError('Baseline document changed or omitted: ' + relative)
    manifest = {'schema': 1, 'created_utc': datetime.now(timezone.utc).isoformat(),
                'baseline_fork_commit': BASELINE,
                'remote_publication': 'not-created; prepared local repositories only',
                'repositories': {}, 'artifacts': {}}
    for language in ('DE', 'EN'):
        name = 'ModSecurity-JSON-Tests-' + language
        root = destination / name
        root.mkdir()
        for path in selected:
            target = root / path.relative_to(ROOT)
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(path, target)
        for code in ('de', 'en'):
            shutil.copyfile(ROOT / ('doc/json-pr-extension/repository-README.' + code + '.md'),
                            root / ('README.' + code + '.md'))
        shutil.copyfile(root / ('README.' + language.lower() + '.md'), root / 'README.md')
        workflow = root / '.github/workflows/json-tests.yml'
        workflow.parent.mkdir(parents=True)
        shutil.copyfile(root / 'test/json-baseline/repository-template/.github/workflows/json-tests.yml', workflow)
        (root / '.gitignore').write_text('__pycache__/\n*.pyc\n')
        patch = root / 'patches/native-regressions.patch'
        patch.parent.mkdir()
        patch.write_bytes(git(ROOT, 'diff', '--binary',
            '8271c9d3bc0abcbd1189f5414c196c336c959047', '--',
            'test/Makefile.am', 'test/test-cases/regression/request-body-parser-json.json'))
        localization = localize_english(root) if language == 'EN' else {
            'documentation_languages': ['de', 'en'],
            'baseline_documents_retained_byte_identical': original_documents}
        git(root, 'init', '-b', 'main')
        git(root, 'add', '--', '.')
        git(root, '-c', 'user.name=' + identity['name'], '-c', 'user.email=' + identity['email'],
            'commit', '-m', 'test: prepare ' +
            ('English-only' if language == 'EN' else 'bilingual') +
            ' ModSecurity JSON behavior contracts')
        commit = git(root, 'rev-parse', 'HEAD').decode().strip()
        files = git(root, 'ls-tree', '-r', '--name-only', 'HEAD').decode().splitlines()
        inventory = {relative: digest(root / relative) for relative in files}
        manifest['repositories'][name] = {'local_path': str(root), 'branch': 'main',
            'commit': commit, 'primary_language': language,
            'file_count': len(files), 'files_sha256': inventory,
            'remote': None, 'documentation_languages': ['en'] if language == 'EN' else ['de', 'en'],
            'localization': localization}
        zip_path = artifacts / (name + '.zip')
        git(root, 'archive', '--format=zip', '--prefix=' + name + '/', 'HEAD', '-o', str(zip_path))
        bundle = artifacts / (name + '.bundle')
        git(root, 'bundle', 'create', str(bundle), 'main', 'HEAD')
        git(root, 'bundle', 'verify', str(bundle), stderr=subprocess.STDOUT)
        restored = destination / ('restore-check-' + language)
        subprocess.check_output(['git', 'clone', str(bundle), str(restored)],
                                stderr=subprocess.STDOUT)
        if git(restored, 'rev-parse', 'HEAD').decode().strip() != commit:
            raise RuntimeError('Bundle clone did not restore the expected commit')
        if git(restored, 'branch', '--show-current').decode().strip() != 'main':
            raise RuntimeError('Bundle clone did not select the main branch')
        if any(digest(restored / path) != value for path, value in inventory.items()):
            raise RuntimeError('Bundle clone lost tracked files')
        manifest['repositories'][name]['default_bundle_clone_verified'] = True
        for artifact in (zip_path, bundle):
            manifest['artifacts'][artifact.name] = {'sha256': digest(artifact),
                'bytes': artifact.stat().st_size, 'repository_commit': commit}
    left, right = list(manifest['repositories'].values())
    packaging = {'test/json-baseline/tools/export_repositories.py',
                 'test/json-baseline/tools/english_export.py'}
    def test_files(record):
        return {path: value for path, value in record['files_sha256'].items()
                if path.startswith(('test/', '.github/', 'patches/')) and path not in packaging}
    if not test_files(left) or test_files(left) != test_files(right):
        raise RuntimeError('Executable tests, fixtures, reports or workflows differ between profiles')
    differing = sorted(path for path in left['files_sha256']
                       if path in right['files_sha256'] and
                       left['files_sha256'][path] != right['files_sha256'][path])
    manifest['parity'] = {'differing_common_files': differing,
        'only_bilingual_profile': sorted(set(left['files_sha256']) - set(right['files_sha256'])),
        'only_english_profile': sorted(set(right['files_sha256']) - set(left['files_sha256'])),
        'executable_tests_fixtures_reports_workflows_identical': True,
        'compared_test_files': sorted(test_files(left)),
        'documentation_profiles': {'DE': ['de', 'en'], 'EN': ['en']},
        'baseline_documents_verified_in_source_against_commit': original_documents}
    (artifacts / 'export-manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(json.dumps({name: info['commit'] for name, info in manifest['repositories'].items()}))


if __name__ == '__main__':
    main()
