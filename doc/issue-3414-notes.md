# Issue 3414: Concurrent Audit Logging Memory Pressure

## Kurzfassung
Issue #3414 reports gradual RAM growth when `SecAuditLogType Concurrent` (mapped to Parallel) is enabled
with `SecAuditLogStorageDir`, and RAM drops after deleting audit logs. The likely pressure source is a
very large number of small files/directories, which increases OS dentry/inode/page cache and I/O overhead.
This document proposes upstream-safe options to reduce memory pressure while preserving security and
compatibility defaults.

## Optionen A/B/C (Vergleich: CPU/IO/Security)

### Option A: Configurable directory granularity (Day/Hour/Minute/Second)
**Idea:** Make directory layout configurable so users can reduce directory fan-out and creation rates.

**CPU/IO:**
- Lower `mkdir` calls if using coarser granularity (e.g., day/hour).
- Fewer directories => fewer inode/dentry entries.

**Security:**
- No new security risks if path sanitation and permissions remain unchanged.
- Keeps one file per transaction (current behavior) => no change in log integrity.

**Compatibility:**
- Default preserves current behavior (day + minute + second path).
- Backward compatible with existing tooling if default unchanged.

### Option B: Optional chunked/append mode (multiple transactions per time window)
**Idea:** Write multiple transactions into a single file per time window (e.g., 1 minute), reducing
file count dramatically.

**CPU/IO:**
- Fewer `open`/`close`/`mkdir` calls and fewer files.
- Potentially more lock contention on the shared chunk file.

**Security:**
- Must ensure safe locking (flock or per-file mutex) to avoid interleaving.
- Log integrity changes: multiple transactions per file; include explicit boundaries.
- Consider log tamper resistance: append mode can be made safe with fsync/flush policy.

**Compatibility:**
- Default stays one file per transaction (no change).
- Optional mode requires documentation and tooling updates for log parsing.

### Option C: Streaming output (avoid building giant strings)
**Idea:** Stream audit log parts to file rather than building a full string in memory.

**CPU/IO:**
- Potentially higher CPU (multiple writes) but reduces peak heap usage.
- Lower memory spikes for large bodies.

**Security:**
- Must preserve exact formatting and boundaries to avoid log corruption.
- No path or permission changes.

**Compatibility:**
- Default output remains identical; internal implementation change only.
- Requires careful refactor of `Transaction::toJSON` / `toOldAuditLogFormat`.

## Konfig-Design

### New directives (default preserves current behavior)
1) `SecAuditLogStorageDirMode`
   - **Values:** `Day`, `Hour`, `Minute`, `Second`
   - **Default:** `Second` (current behavior)
   - **Security impact:** none if permissions unchanged.

2) `SecAuditLogParallelChunkInterval`
   - **Values:** `0` (disabled), `1s`, `10s`, `1m`, `5m` (or integer seconds)
   - **Default:** `0` (disabled)
   - **Security impact:** multiple entries per file; use safe locking and explicit boundaries.

3) `SecAuditLogParallelChunkLock`
   - **Values:** `flock`, `mutex`
   - **Default:** `flock`
   - **Security impact:** ensures log integrity under concurrent writers.

### Security considerations
- Preserve `SecAuditLogFileMode`/`SecAuditLogDirMode` permissions.
- Prevent path traversal (keep existing sanitized path generation).
- Avoid TOCTOU: create directories with `mkdir` and handle `EEXIST` safely.
- Ensure chunked files are opened with append + lock, and flush boundaries.

## Code-Änderungen (files/functions + diff-like plan)

### Entry points and rationale
- `src/audit_log/writer/parallel.cc` (`Parallel::write`): main write path and file naming.
- `src/audit_log/audit_log.cc` (`AuditLog::init`, `saveIfRelevant`): writer init and write calls.
- `src/utils/shared_files.cc` and `src/utils/system.cc`: if new shared file handles or dir logic are added.

### Diff-like plan (English code only)
```diff
// headers/modsecurity/audit_log.h
+ enum AuditLogStorageDirMode {
+   StorageDirSecond,
+   StorageDirMinute,
+   StorageDirHour,
+   StorageDirDay
+ };
+
+ enum AuditLogParallelChunkLock {
+   ChunkLockFlock,
+   ChunkLockMutex
+ };
+
+ AuditLogStorageDirMode m_storageDirMode = StorageDirSecond;
+ int m_parallelChunkIntervalSeconds = 0;
+ AuditLogParallelChunkLock m_parallelChunkLock = ChunkLockFlock;
```

```diff
// src/parser/seclang-parser.yy
+ /* SecAuditLogStorageDirMode */
+ | CONFIG_DIR_AUDIT_DIR_MODE CONFIG_VALUE_DAY
+   { driver.m_auditLog->setStorageDirMode(StorageDirDay); }
+ | CONFIG_DIR_AUDIT_DIR_MODE CONFIG_VALUE_HOUR
+   { driver.m_auditLog->setStorageDirMode(StorageDirHour); }
+ | CONFIG_DIR_AUDIT_DIR_MODE CONFIG_VALUE_MINUTE
+   { driver.m_auditLog->setStorageDirMode(StorageDirMinute); }
+ | CONFIG_DIR_AUDIT_DIR_MODE CONFIG_VALUE_SECOND
+   { driver.m_auditLog->setStorageDirMode(StorageDirSecond); }
+
+ /* SecAuditLogParallelChunkInterval */
+ | CONFIG_DIR_AUDIT_PARALLEL_CHUNK_INTERVAL
+   { driver.m_auditLog->setParallelChunkInterval($1); }
+
+ /* SecAuditLogParallelChunkLock */
+ | CONFIG_DIR_AUDIT_PARALLEL_CHUNK_LOCK CONFIG_VALUE_FLOCK
+   { driver.m_auditLog->setParallelChunkLock(ChunkLockFlock); }
+ | CONFIG_DIR_AUDIT_PARALLEL_CHUNK_LOCK CONFIG_VALUE_MUTEX
+   { driver.m_auditLog->setParallelChunkLock(ChunkLockMutex); }
```

```diff
// src/audit_log/writer/parallel.cc
- std::string fileName = logFilePath(&transaction->m_timeStamp,
-   YearMonthDayDirectory | YearMonthDayAndTimeDirectory | YearMonthDayAndTimeFileName);
+ std::string fileName = logFilePath(&transaction->m_timeStamp,
+   mapStorageDirModeToPathParts(m_audit->m_storageDirMode));
+
+ if (m_audit->m_parallelChunkIntervalSeconds > 0) {
+   fileName = makeChunkFileName(transaction->m_timeStamp,
+     m_audit->m_parallelChunkIntervalSeconds, transaction->m_id);
+ }
+
+ if (m_audit->m_parallelChunkIntervalSeconds > 0) {
+   lockChunkFile(fileName, m_audit->m_parallelChunkLock);
+   appendChunk(fileName, log, boundary);
+   unlockChunkFile(fileName, m_audit->m_parallelChunkLock);
+ } else {
+   writeSingleFile(fileName, log);
+ }
```

### Error handling strategy
- Return `false` on any `open`/`write` failure and log via `ms_dbg_a`.
- Keep current behavior for non-chunked paths.
- Ensure locks are released on all error paths (RAII guard).

## Performance & IO analysis
- **File count:** Option A reduces directory fan-out (fewer dirs). Option B reduces file count drastically.
- **Syscalls:** Fewer `mkdir` and `open/close` with chunking; potential extra `flock` per write.
- **CPU:** Slight increase with locking and boundary writes; streaming (Option C) could increase CPU due to
  more write calls but lower heap usage.
- **Write pattern:** Many small files (current) vs fewer append writes (chunked). Fewer files generally
  reduce dentry/inode cache growth and IOPS.

## Tests/Benchmark-Plan

### CI-suitable tests
1) Path generation for storage mode:
```text
Test: storage_dir_mode_day_hour_minute_second
Expect: directory path matches selected granularity
```
2) Chunk window selection:
```text
Test: chunk_interval_60s
Expect: multiple transactions map to same file within interval
```
3) Basic write in parallel mode:
```text
Test: parallel_write_chunked
Expect: file exists and contains two entries with boundaries
```

### Optional benchmark plan (manual)
**Metrics:** req/s, CPU%, IOPS, created files/dirs, log size, RSS

**Commands (examples):**
```text
wrk -t4 -c200 -d60s http://localhost/
strace -c -p <nginx_worker_pid>
iostat -x 1
pidstat -p <pid> 1
slabtop
```

## PR-Beschreibung (zum Kopieren)

**Title:** Reduce memory pressure for Concurrent (Parallel) audit logging

**Summary:**
- Add optional directory granularity for parallel audit log storage paths.
- Add optional chunked append mode with safe locking for fewer files.
- Keep defaults unchanged to preserve compatibility.

**Details:**
- New directives:
  - `SecAuditLogStorageDirMode` (Day/Hour/Minute/Second, default: Second)
  - `SecAuditLogParallelChunkInterval` (0=disabled, default: 0)
  - `SecAuditLogParallelChunkLock` (flock/mutex, default: flock)
- Security considerations addressed: permissions preserved, append-mode locking, explicit boundaries.

**Config example (English code):**
```text
SecAuditLogType Concurrent
SecAuditLogStorageDir /var/log/modsec/audit
SecAuditLogStorageDirMode Minute
SecAuditLogParallelChunkInterval 60
SecAuditLogParallelChunkLock flock
```

**Testing:**
- Unit/integration tests for path generation and chunk window mapping.
- Manual benchmark plan included (wrk/strace/iostat/pidstat/slabtop).
