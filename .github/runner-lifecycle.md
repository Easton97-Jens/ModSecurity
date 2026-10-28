# Runner lifecycle

Generated from GitHub's runner catalog and retirement announcements. Review runner changes before merging; existing runners are retained.

| Runner | Status | Retirement date | Source |
| --- | --- | --- | --- |
| `macos-15` | active | not announced | [GitHub](https://raw.githubusercontent.com/actions/runner-images/main/README.md) |
| `macos-26` | active | not announced | [GitHub](https://raw.githubusercontent.com/actions/runner-images/main/README.md) |
| `ubuntu-22.04` | active | not announced | [GitHub](https://raw.githubusercontent.com/actions/runner-images/main/README.md) |
| `ubuntu-24.04` | active | not announced | [GitHub](https://raw.githubusercontent.com/actions/runner-images/main/README.md) |
| `ubuntu-26.04` | active | not announced | [GitHub](https://raw.githubusercontent.com/actions/runner-images/main/README.md) |
| `windows-2022` | active | not announced | [GitHub](https://raw.githubusercontent.com/actions/runner-images/main/README.md) |

## Pinned coverage

- `ubuntu-22.04`: Legacy Linux x32/x64 coverage in ci.yml; keep this runner until a replacement for the 32-bit build is reviewed.

## Excluded additions

- `windows-2025`: Requires the Windows toolchain migration tracked in #3715. Remove this exclusion after that migration is integrated.
