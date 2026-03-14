
noinst_LTLIBRARIES = libinjection.la libmbedtls.la

libinjection_la_SOURCES = \
	libinjection/src/libinjection_html5.c \
	libinjection/src/libinjection_sqli.c \
	libinjection/src/libinjection_xss.c

libinjection_la_CFLAGS = -D LIBINJECTION_VERSION=\"${LIBINJECTION_VERSION}\"
libinjection_la_LIBADD =

noinst_HEADERS = \
	libinjection/src/libinjection.h \
	libinjection/src/libinjection_html5.h \
	libinjection/src/libinjection_sqli.h \
	libinjection/src/libinjection_sqli_data.h \
	libinjection/src/libinjection_xss.h \
	mbedtls/tf-psa-crypto/include/mbedtls/base64.h \
	mbedtls/tf-psa-crypto/drivers/builtin/src/check_crypto_config.h \
	mbedtls/include/mbedtls/mbedtls_config.h \
	mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/md5.h \
	mbedtls/tf-psa-crypto/include/mbedtls/platform.h \
	mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/sha1.h

MBED_PSA_CORE_SOURCES = \
        $(filter-out $(srcdir)/mbedtls/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c, \
                $(wildcard $(srcdir)/mbedtls/tf-psa-crypto/core/*.c)) \
        $(MBED_GENERATED_PSA_WRAPPERS_C)

MBED_BUILTIN_SOURCES = \
        $(wildcard $(srcdir)/mbedtls/tf-psa-crypto/drivers/builtin/src/*.c)

MBED_PYTHON = $(shell if command -v python3 >/dev/null 2>&1; then echo python3; else echo python; fi)
MBED_GENERATED_PSA_WRAPPERS_H = $(builddir)/mbedtls/tf-psa-crypto/core/psa_crypto_driver_wrappers.h
MBED_GENERATED_PSA_WRAPPERS_C = $(builddir)/mbedtls/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c

BUILT_SOURCES = \
        $(MBED_GENERATED_PSA_WRAPPERS_H) \
        $(MBED_GENERATED_PSA_WRAPPERS_C)

CLEANFILES = \
        $(MBED_GENERATED_PSA_WRAPPERS_H) \
        $(MBED_GENERATED_PSA_WRAPPERS_C)

$(MBED_GENERATED_PSA_WRAPPERS_H) $(MBED_GENERATED_PSA_WRAPPERS_C):
	$(AM_V_at)mkdir -p $(builddir)/mbedtls/tf-psa-crypto/core
	$(AM_V_GEN)$(MBED_PYTHON) $(srcdir)/mbedtls/tf-psa-crypto/scripts/generate_driver_wrappers.py $(builddir)/mbedtls/tf-psa-crypto/core

libmbedtls_la_SOURCES = \
        $(MBED_PSA_CORE_SOURCES) \
        $(MBED_BUILTIN_SOURCES)

libmbedtls_la_CPPFLAGS = \
        -DMBEDTLS_CONFIG_FILE=\"mbedtls/mbedtls_config.h\" \
        -I$(top_srcdir)/others/mbedtls/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/core \
        -I$(top_builddir)/others/mbedtls/tf-psa-crypto/core \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/src

#libmbedtls_la_CPPFLAGS =
libmbedtls_la_LIBADD =
