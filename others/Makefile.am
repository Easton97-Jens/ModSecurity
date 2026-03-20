
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

include $(top_srcdir)/others/mbedtls_sources.mk

MBED_PSA_CORE_SOURCES += \
        $(MBED_GENERATED_PSA_WRAPPERS_C)

MBEDTLS_PY = @MBEDTLS_PYTHON@
MBED_GENERATED_CORE_DIR = $(builddir)/mbedtls-generated/core
MBED_GENERATED_PSA_WRAPPERS_H = $(MBED_GENERATED_CORE_DIR)/psa_crypto_driver_wrappers.h
MBED_GENERATED_PSA_WRAPPERS_C = $(MBED_GENERATED_CORE_DIR)/psa_crypto_driver_wrappers_no_static.c
MBED_GENERATED_TF_PSA_CONFIG_HDRS = \
        $(MBED_GENERATED_CORE_DIR)/tf_psa_crypto_config_check_before.h \
        $(MBED_GENERATED_CORE_DIR)/tf_psa_crypto_config_check_final.h \
        $(MBED_GENERATED_CORE_DIR)/tf_psa_crypto_config_check_user.h
MBED_GENERATED_STAMP = $(MBED_GENERATED_CORE_DIR)/.generated.stamp

BUILT_SOURCES = \
        $(MBED_GENERATED_PSA_WRAPPERS_H) \
        $(MBED_GENERATED_PSA_WRAPPERS_C) \
        $(MBED_GENERATED_TF_PSA_CONFIG_HDRS)

CLEANFILES = \
        $(MBED_GENERATED_PSA_WRAPPERS_H) \
        $(MBED_GENERATED_PSA_WRAPPERS_C) \
        $(MBED_GENERATED_TF_PSA_CONFIG_HDRS) \
        $(MBED_GENERATED_STAMP)

$(MBED_GENERATED_PSA_WRAPPERS_H) $(MBED_GENERATED_PSA_WRAPPERS_C) $(MBED_GENERATED_TF_PSA_CONFIG_HDRS): $(MBED_GENERATED_STAMP)

$(MBED_GENERATED_STAMP):
	$(AM_V_at)mkdir -p $(MBED_GENERATED_CORE_DIR)
	$(AM_V_GEN)$(MBEDTLS_PY) $(top_srcdir)/others/mbedtls/tf-psa-crypto/scripts/generate_driver_wrappers.py $(MBED_GENERATED_CORE_DIR)
	$(AM_V_GEN)cd $(top_srcdir)/others/mbedtls/tf-psa-crypto && \
		$(MBEDTLS_PY) scripts/generate_config_checks.py $(abspath $(MBED_GENERATED_CORE_DIR))
	$(AM_V_at)touch $(MBED_GENERATED_STAMP)

libmbedtls_la_SOURCES = \
        $(MBED_PSA_CORE_SOURCES) \
        $(MBED_BUILTIN_SOURCES)

libmbedtls_la_CPPFLAGS = \
        -DMBEDTLS_CONFIG_FILE=\"mbedtls/mbedtls_config.h\" \
        -I$(top_srcdir)/others/mbedtls/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/core \
        -I$(top_builddir)/others/mbedtls-generated/core \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/src

#libmbedtls_la_CPPFLAGS =
libmbedtls_la_LIBADD =
