
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
	mbedtls/include/mbedtls/base64.h \
	mbedtls/include/mbedtls/check_config.h \
	mbedtls/include/mbedtls/mbedtls_config.h \
	mbedtls/include/mbedtls/md5.h \
	mbedtls/include/mbedtls/platform.h \
	mbedtls/include/mbedtls/sha1.h

libmbedtls_la_SOURCES = \
	mbedtls/library/base64.c \
	mbedtls/library/md5.c \
	mbedtls/library/sha1.c \
	mbedtls/library/platform_util.c

libmbedtls_la_CFLAGS = -DMBEDTLS_CONFIG_FILE=\"mbedtls/mbedtls_config.h\" -I$(top_srcdir)/others/mbedtls/include
libmbedtls_la_CPPFLAGS =
libmbedtls_la_LIBADD =

if BLAZE_ENABLED
BLAZE_BUILD_DIR = $(abs_builddir)/blaze-build

all-local:
	$(MKDIR_P) $(BLAZE_BUILD_DIR)
	$(CMAKE) -S $(abs_srcdir)/blaze-wrapper -B $(BLAZE_BUILD_DIR) \
		-DBLAZE_COMPILER=ON \
		-DBLAZE_EVALUATOR=ON \
		-DBLAZE_INSTALL=OFF \
		-DBLAZE_TESTS=OFF \
		-DBLAZE_BENCHMARK=OFF \
		-DBLAZE_CONTRIB=OFF \
		-DBLAZE_DOCS=OFF \
		-DSOURCEMETA_CORE_JSON=ON \
		-DSOURCEMETA_CORE_JSONPOINTER=ON \
		-DSOURCEMETA_CORE_JSONSCHEMA=ON \
		-DSOURCEMETA_CORE_URI=ON \
		-DSOURCEMETA_CORE_REGEX=ON \
		-DSOURCEMETA_CORE_JSONL=OFF \
		-DSOURCEMETA_CORE_YAML=OFF \
		-DSOURCEMETA_CORE_EXTENSION_ALTERSCHEMA=OFF \
		-DSOURCEMETA_CORE_TESTS=OFF \
		-DSOURCEMETA_CORE_BENCHMARK=OFF \
		-DSOURCEMETA_CORE_DOCS=OFF \
		-DSOURCEMETA_CORE_INSTALL=OFF \
		-DSOURCEMETA_CORE_CONTRIB_GOOGLETEST=OFF \
		-DSOURCEMETA_CORE_CONTRIB_GOOGLEBENCHMARK=OFF
	$(CMAKE) --build $(BLAZE_BUILD_DIR) --target \
		sourcemeta_blaze_evaluator \
		sourcemeta_blaze_compiler \
		modsecurity_blaze_bridge
endif

clean-local:
	rm -rf $(abs_builddir)/blaze-build
