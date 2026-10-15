# ModSecurity v3: JSON/YAJL tests and English documentation

This repository contains the **complete ModSecurity v3 source tree**, together
with additional JSON regression tests, test helpers and the complete English
documentation from the earlier investigation. The official project
[README](README.md) remains unchanged. Start there for the library, connectors,
dependencies, normal build process and contribution guidance; use this index for
the JSON investigation.

YAJL remains the JSON library. This work does not replace the parser, convert
between SAX and DOM, or change production behavior to make a test pass. The
additional cases exercise the real ModSecurity request-processing and
JSON-generating interfaces.

## Current full-source result

The new official `v3/master` reference is
[`dd1d0f82672ecf7d0f014a0c3fc4fd0ab40b3bdd`](https://github.com/owasp-modsecurity/ModSecurity/commit/dd1d0f82672ecf7d0f014a0c3fc4fd0ab40b3bdd),
fetched on October 15, 2026 at 15:24 UTC (17:24 CEST) and then pinned. This is
newer than the intermediate `2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641`
comparison described in the historical extension. The production reference
will not be silently advanced again during this investigation.

The [current v3/master report](doc/json-pr-extension/full-v3-master.md) records
the exact official source commit, task branch, build configuration, executed
commands, individual results and publication status for this full-source
delivery. The official source SHA and verified fork result SHA are different
references. The report records the source SHA; the final handover and GitHub
commit metadata identify the verified published result without a recursive self-SHA.

The new local task branch is `test/json-yajl-v3-master-english-20261015` in the
`Easton97-Jens/ModSecurity` fork clone. Its final verified commit is given in the handover; publication details
are explained in the current report. A local branch name is not evidence
that a push succeeded.

**Use the current report for current test counts.** The complete source tree
does not by itself demonstrate that it was built or tested. The current source
requires a fresh engine build: the earlier compiled library cannot establish
the new reference or its changed public-header layout. This entry page
does not transfer a PASS from an older source commit to the new original
reference.

Clone the full source branch into a new directory:

```sh
git clone --single-branch --no-tags --branch test/json-yajl-v3-master-english-20261015 https://github.com/Easton97-Jens/ModSecurity.git modsecurity-v3-json-en
cd modsecurity-v3-json-en
git submodule update --init --recursive
```

## Complete documentation index

| Document | Scope |
|---|---|
| [Official project README](README.md) | Complete upstream project instructions, retained unchanged |
| [Current full-source report](doc/json-pr-extension/full-v3-master.md) | New pinned v3/master source, actual execution and verified handover |
| [Original results overview](doc/JSON-YAJL-BASELINE.md) | Complete English historical baseline overview |
| [How ModSecurity processes a JSON request](doc/json-yajl-code-analysis.md) | Complete English processing-path analysis, v2/v3 distinctions and verification of discussion statements |
| [Original v3 characterization](doc/json-yajl-baseline-v3.md) | Complete English historical matrix, expectations, environment, commands and limitations |
| [PR/comment test extension](doc/json-pr-extension/README.en.md) | Additional cases derived from the two jsonbench PRs and ModSecurity comments |
| [Coverage and open decisions](doc/json-pr-extension/matrix.en.md) | Stable case IDs, coverage of groups A–J and capabilities outside the task |
| [Original provenance](doc/json-yajl-provenance.json) | Immutable historical original references and environment |
| [Extension provenance](doc/json-pr-extension/provenance.json) | Historical extension source inventory and execution evidence |
| [Case catalog](doc/json-pr-extension/catalog.json) | Deterministic inputs, fixed expectations and links to individual cases |

The historical baseline investigated official production commit
[`8271c9d3bc0abcbd1189f5414c196c336c959047`](https://github.com/owasp-modsecurity/ModSecurity/commit/8271c9d3bc0abcbd1189f5414c196c336c959047)
and was published in fork commit
[`2bfb2180d8c0fa35ae7a3f2665e999600607d896`](https://github.com/Easton97-Jens/ModSecurity/commit/2bfb2180d8c0fa35ae7a3f2665e999600607d896).
Its counts, limitations, paths and publication statements describe that earlier
run. The complete English translations preserve those historical findings;
the current report documents any new observations separately.

## Tests included with the engine

The native JSON rules live in
[`test/test-cases/regression/request-body-parser-json.json`](test/test-cases/regression/request-body-parser-json.json).
The additional byte-level, parser-chunk, lifecycle and writer tests live under
[`test/json-baseline/`](test/json-baseline/). They include:

- Exact names and values in ARGS, duplicate multiplicity, query arguments and
  collection presence or absence.
- Numeric-looking keys and strings, original number text, long tokens, path
  collisions, embedded NUL and bounded long keys and values.
- UTF-8 bytes, Unicode escapes, surrogate edge cases and malformed syntax.
- Real calls to the internal ModSecurity JSON processor at controlled chunk
  boundaries, including completion-only errors and selected 16 KiB splits.
- Argument/depth limits and separate transactions after an earlier failure.
- Audit JSON structure, field types, log-part selection, duplicate headers,
  repeated serialization and the separate public offset JSON writer.

Characterization tests state what the examined original does. Requirement tests
state additional output guarantees or explicitly labelled policy proposals. A
characterization PASS does not make questionable behavior a desired contract.
Unmet requirements remain ordinary FAIL results with a nonzero exit, rather
than XFAIL or a relaxed comparison. Crashes, timeouts and missing dependencies
remain distinguishable from deliberate parser rejection.

## Build and reproduce

Build this **complete source checkout** using the dependency and submodule
instructions in the unchanged [official README](README.md). Keep required
submodules at the commits recorded by this source tree; do not update them to
their latest branch tips. Keep build products, generated reports and raw logs
outside versioned documentation and fixtures where the runner supports it.

The [current full-source report](doc/json-pr-extension/full-v3-master.md) provides
the exact tested commands for this source reference. Follow those commands for
the native tests, JSON helpers and separately reported requirement profile.
Helpers must verify this delivery's pinned source and actual loaded library;
older helper commands that deliberately require `8271c9d3` must not silently be
used to validate a different production tree. Commands in the historical
reports reproduce their own original reference.

After publication, recover this delivery from the **verified task commit**
recorded in the handover, including its complete tree and new fixtures.
A moving `v3/master` tip, an older fork branch or a copied documentation folder
does not reconstruct that tested state. No automatic fork synchronization,
production merge or replacement of an existing user branch is part of this work.

## Sources and limits

The extension independently implements test ideas from
[jsonbench PR #4](https://github.com/digitalwave/jsonbench/pull/4),
[PR #5](https://github.com/digitalwave/jsonbench/pull/5),
[fzipi's comment](https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6166461842)
and [airween's comment](https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6142615388).
Their statements are checked against ModSecurity rather than treated as an
already adopted specification. No jsonbench adapter implementation is copied;
original licensing and attribution remain in force.

The tests do not choose a replacement library, demonstrate equal SAX/DOM memory
consumption, establish benchmark rankings or prove complete external connector
or thread behavior. Bounded fixtures are not an allocation-exhaustion test.
Any sanitizer evidence must identify its actual instrumented scope. Validation
status and publication status remain separate in the current report.
