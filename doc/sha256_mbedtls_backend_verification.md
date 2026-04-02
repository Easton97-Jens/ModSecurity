# SHA-256 Backend Verification (ModSecurity v3)

Date: 2026-04-02

## A. Wird Mbed TLS verwendet?
**Ja.**

Die SHA-256-Transformation nutzt `Utils::Sha256`, das auf `DigestImpl<MBEDTLS_MD_SHA256, 32>` basiert. `DigestImpl` inkludiert `mbedtls/md.h` und ruft `mbedtls_md_info_from_type(...)` sowie `mbedtls_md(...)` auf.

## B. Wo genau wird Mbed TLS verwendet?
- `src/utils/sha1.h`
  - `#include "mbedtls/md.h"`
  - `mbedtls_md_info_from_type(DigestType)`
  - `mbedtls_md(...)`
- `src/utils/sha256.h`
  - `class Sha256 : public DigestImpl<MBEDTLS_MD_SHA256, 32>`
- `src/actions/transformations/sha256.cc`
  - `Utils::Sha256::digest(value)`

## C. Wird Mbed TLS speziell für SHA-256 genutzt?
**Ja, indirekt über den generischen Wrapper.**

Es gibt keine direkte `mbedtls_sha256_*`-Aufrufstelle im Transformationscode; stattdessen erfolgt SHA-256 über den gemeinsamen Wrapper `DigestImpl` mit `MBEDTLS_MD_SHA256`.

## D. Ist die Implementierung Mbed TLS v4-kompatibel?
**Codebasiert: kompatibel ausgelegt.**

Belege:
- Build bindet Mbed TLS TF-PSA-Crypto-Pfade (`tf-psa-crypto/.../md.c`, `sha256.c`) ein.
- Includes zeigen `mbedtls/md.h` aus dieser Struktur.

Das spricht für eine Nutzung der in dieser Codebasis vorgesehenen Mbed-TLS/TF-PSA-MD-API statt veralteter projektexterner Backends.

## E. Alternative Backends
**Im SHA-256-Pfad keine belegt.**

Keine OpenSSL-`EVP_*`- oder `openssl/...`-Nutzung im SHA-256-Digestpfad gefunden.

## F. Vertrauensniveau
**Hoch.**

Die Aussage basiert auf direkten Includes, Typen (`MBEDTLS_MD_SHA256`) und konkreten API-Aufrufen im Digest-Wrapper plus Build-Einbindung von Mbed TLS TF-PSA-Crypto-Quellen.
