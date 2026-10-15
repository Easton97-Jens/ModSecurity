#!/bin/sh
set -eu
# Run from the configured v2 source root; all outputs belong outside source.
# APR_CFLAGS/APR_LIBS and EXTRA_CFLAGS permit an isolated dependency prefix.
# Normal system install: APR_CFLAGS="$(apr-1-config --includes)" \
# APR_LIBS="$(apr-1-config --link-ld --libs)" ./.../build-processor.sh /tmp/processor
output=$1
${CC:-cc} -std=gnu99 -g -O1 -ffunction-sections -fdata-sections \
  ${EXTRA_CFLAGS:-} ${APR_CFLAGS:-$(apr-1-config --includes)} \
  $(pkg-config --cflags yajl libxml-2.0) -Iapache2 -I"$(apxs -q INCLUDEDIR)" -DHAVE_CONFIG_H -DWITH_YAJL \
  tests/json-yajl-baseline/processor.c apache2/msc_json.c apache2/msc_util.c \
  -Wl,--gc-sections ${EXTRA_LDFLAGS:-} ${APR_LIBS:-$(apr-1-config --link-ld --libs)} \
  $(pkg-config --libs yajl libxml-2.0) -o "$output"
