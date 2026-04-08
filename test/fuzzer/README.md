# Fuzzer harnesses

This directory contains lightweight harnesses for high-risk libmodsecurity paths:

- `transaction_fuzzer`: Transaction API, URI/header/body parsing and rule execution.
- `rule_parser_fuzzer`: Rule parsing and evaluation with chunked request body handling.

## Local standalone run

```bash
./build.sh
CC=clang CXX=clang++ ./configure --enable-afl-fuzz --enable-assertions=yes --without-lua
make -j"$(nproc)"

cat test/fuzzer/corpus/transaction/basic.bin | ./test/fuzzer/transaction_fuzzer
cat test/fuzzer/corpus/rule_parser/basic_regex.bin | ./test/fuzzer/rule_parser_fuzzer
```

> Note: `--enable-afl-fuzz` enables ASan instrumentation and is expected to use clang-family compilers.
