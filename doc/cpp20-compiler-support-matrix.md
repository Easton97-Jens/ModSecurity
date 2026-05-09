# C++20 Compiler Support Matrix Across Operating Systems

## 1. Introduction
C++20 is the ISO C++ standard published as **ISO/IEC 14882:2020**; compiler/library implementation status directly affects whether production code can use features such as concepts, ranges, coroutines, modules, and calendar/time-zone support.  
Source: https://isocpp.org/std/the-standard and https://en.cppreference.com/w/cpp/compiler_support/20

Because support is feature-by-feature (not a single boolean), teams should track **compiler version**, **standard library version**, and **distribution packaging policy** together.  
Source: https://gcc.gnu.org/projects/cxx-status.html and https://clang.llvm.org/cxx_status.html and https://learn.microsoft.com/cpp/overview/visual-cpp-language-conformance

## 2. Official C++20 Compiler Support References

| Topic | Official reference |
|---|---|
| GCC C++ status | https://gcc.gnu.org/projects/cxx-status.html |
| LLVM Clang C++ status | https://clang.llvm.org/cxx_status.html |
| Microsoft MSVC conformance | https://learn.microsoft.com/cpp/overview/visual-cpp-language-conformance |
| cppreference compiler support matrix | https://en.cppreference.com/w/cpp/compiler_support/20 |

## 3. Linux Distributions

> Audit note: package versions below are distribution-repository values and can change with updates/backports. Re-check using the linked package pages and the verification commands in Section 8.

| Distribution | Release line | Default GCC package (repo) | Approximate C++20 support quality | Official source |
|---|---:|---|---|---|
| Ubuntu | 24.04 LTS (Noble) | `gcc-13` package in Noble archive | Good for most mainstream C++20; feature-level verification still required | https://packages.ubuntu.com/noble/gcc-13 |
| Debian | 12 (Bookworm) | `gcc-12` in stable | Good baseline; some late C++20/C++23-adjacent items require newer compilers | https://packages.debian.org/bookworm/gcc-12 |
| Fedora | 40 | GCC from Fedora 40 gcc package (GNU Toolchain F40 targets GCC 14) | Very strong/modern | https://packages.fedoraproject.org/pkgs/gcc/gcc/fedora-40.html and https://fedoraproject.org/wiki/Changes/GNUToolchainF40 |
| CentOS Linux | 8 (EOL) | GCC from CentOS Linux 8 repos (historical) | Historical only (EOL platform) | https://www.centos.org/centos-linux-eol/ |
| CentOS Stream | 9 | GCC from Stream 9 repos (tracks just ahead of RHEL) | Strong enterprise-forward baseline; track stream updates | https://www.centos.org/centos-stream/ |
| Rocky Linux | 9 | GCC aligned with RHEL 9 toolchain baseline | Enterprise-stable baseline; newer standards often via optional toolsets | https://docs.rockylinux.org/ |
| AlmaLinux | 9 | GCC aligned with RHEL 9 toolchain baseline | Enterprise-stable baseline; newer standards often via optional toolsets | https://wiki.almalinux.org/ |
| openSUSE Leap | 15.6 | GCC from Leap 15.6 OSS repo | Stable baseline (not as fast-moving as Tumbleweed) | https://software.opensuse.org/package/gcc |
| Arch Linux | Rolling | `gcc` in core (rolling latest in Arch repos) | Very current (rolling) | https://archlinux.org/packages/core/x86_64/gcc/ |
| Alpine Linux | 3.20 | `gcc` in main repository | Good; musl-based ecosystem differences may matter for some builds | https://pkgs.alpinelinux.org/package/v3.20/main/x86_64/gcc |
| Gentoo | Rolling | `sys-devel/gcc` ebuild selected by profile/user | Very flexible; exact version depends on profile + user selection | https://packages.gentoo.org/packages/sys-devel/gcc |
| NixOS | 24.05 | GCC via `stdenv`/package set in channel | Reproducible; exact compiler depends on pinned nixpkgs revision | https://search.nixos.org/packages?query=gcc |

## 4. Historical Compiler Support (GCC Focus)

GCC’s official C++ status page is the authoritative per-feature matrix. It documents, per language feature, which GCC version first implemented support and whether implementation is complete/partial.  
Source: https://gcc.gnu.org/projects/cxx-status.html

### Version-oriented summary (use with feature matrix)

| GCC line | C++20 support characterization | Source |
|---|---|---|
| GCC 4.x | Predates C++20 standardization; no C++20 implementation baseline | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 5 | Predates C++20 publication; no meaningful C++20 baseline | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 6 | Early experimental implementation period for some proposals that later became C++20 | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 7 | Additional proposal implementations landed pre-standard | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 8 | Expanded pre-standard implementation coverage | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 9 | Substantial pre-C++20 feature coverage; still incomplete overall | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 10 | First major wave after C++20 ratification timeframe | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 11 | Broad implementation progress across core and library features | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 12 | Near-complete for many practical workloads; verify remaining feature gaps | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 13 | Very mature C++20 usability in practice | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 14 | Continued completion/bug-fix maturation and C++23 overlap | https://gcc.gnu.org/projects/cxx-status.html |
| GCC 15 | Current line; confirm exact per-feature status from matrix | https://gcc.gnu.org/projects/cxx-status.html |

### Milestone framing (auditable)
- **Experimental start**: pre-ratification implementation of individual proposals in GCC 6–9, visible in feature rows with those first-supported versions. Source: https://gcc.gnu.org/projects/cxx-status.html
- **Major stability phase**: GCC 10–12 delivered large post-ratification implementation waves. Source: https://gcc.gnu.org/projects/cxx-status.html
- **Nearly complete practical support**: GCC 12+ is commonly used for production C++20, while remaining edge cases should be checked feature-by-feature. Source: https://gcc.gnu.org/projects/cxx-status.html and https://en.cppreference.com/w/cpp/compiler_support/20

## 5. macOS

- Apple ships **Apple Clang** with Xcode/Command Line Tools; this is Clang-based but versioned and patched by Apple, so mapping to upstream LLVM Clang requires checking Apple release notes + compiler `--version`.  
  Sources: https://developer.apple.com/xcode/ and https://clang.llvm.org/cxx_status.html
- Upstream LLVM C++20 feature status is tracked at LLVM’s C++ status page.  
  Source: https://clang.llvm.org/cxx_status.html
- GCC on macOS is typically installed via package managers (e.g., Homebrew) rather than being the system compiler.  
  Source: https://brew.sh/ and https://formulae.brew.sh/formula/gcc

## 6. Windows

- **MSVC (native)**: Microsoft’s cl.exe toolchain with official conformance tracking.  
  Source: https://learn.microsoft.com/cpp/overview/visual-cpp-language-conformance
- **MinGW-w64 (native GNU-style on Windows)**: GCC-based toolchain targeting Win32/Win64 ABI directly.  
  Source: https://www.mingw-w64.org/
- **MSYS2 (distribution/environment)**: package-managed environment that can provide MinGW-w64 GCC/Clang toolchains.  
  Source: https://www.msys2.org/
- **Cygwin (POSIX compatibility layer)**: GNU toolchains targeting Cygwin runtime ABI (not native MSVC ABI).  
  Source: https://www.cygwin.com/
- **WSL (Linux compatibility VM layer)**: runs real Linux distributions/userspace on Windows; compiler behavior follows the chosen Linux distro toolchain.  
  Source: https://learn.microsoft.com/windows/wsl/

### Native vs compatibility environments
| Environment type | Examples | ABI/runtime implication | Source |
|---|---|---|---|
| Native Windows toolchains | MSVC, MinGW-w64 | Produce Windows-native binaries; ABI differs between MSVC and GNU ecosystems | https://learn.microsoft.com/cpp/build/x64-software-conventions and https://www.mingw-w64.org/ |
| Compatibility / Linux-userland environments | Cygwin, WSL | Cygwin targets Cygwin runtime ABI; WSL uses Linux ABI in Linux environment | https://www.cygwin.com/ and https://learn.microsoft.com/windows/wsl/about |

## 7. CentOS Clarification

- **CentOS Linux is end-of-life** (project announcement).  
  Source: https://www.centos.org/centos-linux-eol/
- **CentOS Stream is the continuing CentOS project line**, positioned between Fedora and RHEL.  
  Source: https://www.centos.org/centos-stream/
- **There is no CentOS Linux 9**; CentOS Stream 9 is the relevant “9” generation in CentOS branding.  
  Sources: https://www.centos.org/centos-linux-eol/ and https://www.centos.org/centos-stream/
- **Relationship to RHEL**: CentOS Stream is upstream of RHEL minor releases (preview/development stream), while RHEL is the downstream enterprise product.  
  Sources: https://www.redhat.com/en/topics/linux/what-is-centos-stream and https://www.centos.org/centos-stream/

## 8. Verification Commands

Use these commands to verify toolchain identity and effective C++ standard macro values on a target machine:

```bash
g++ --version
clang++ --version
cl

# GCC / Clang: report __cplusplus for -std=c++20
echo | g++ -std=c++20 -dM -E -x c++ - | grep __cplusplus
echo | clang++ -std=c++20 -dM -E -x c++ - | grep __cplusplus

# MSVC Developer Command Prompt / x64 Native Tools Prompt
cl /Bv
```

MSVC `__cplusplus` reporting behavior (including `/Zc:__cplusplus`) is documented by Microsoft.  
Source: https://learn.microsoft.com/cpp/build/reference/zc-cplusplus
