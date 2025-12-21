
noinst_LTLIBRARIES = libinjection.la libmbedtls.la libmbedcrypto.la

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

libmbedtls_la_SOURCES = \
	mbedtls/tf-psa-crypto/drivers/builtin/src/base64.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/md5.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/sha1.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/platform_util.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/bignum.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/ccm.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/cipher.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/ctr_drbg.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/ecp.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/entropy.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/gcm.c \
	mbedtls/tf-psa-crypto/drivers/builtin/src/rsa.c \
	mbedtls/tf-psa-crypto/core/psa_crypto.c \
	mbedtls/tf-psa-crypto/core/psa_crypto_client.c \
	mbedtls/tf-psa-crypto/core/psa_crypto_slot_management.c \
	mbedtls/tf-psa-crypto/core/psa_crypto_storage.c \
	mbedtls/tf-psa-crypto/core/psa_its_file.c \
	mbedtls/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c

libmbedcrypto_la_SOURCES = \
        mbedtls/library/mps_reader.c \
	mbedtls/library/mps_trace.c \
        mbedtls/library/net_sockets.c \
	mbedtls/library/pkcs7.c \
	mbedtls/library/ssl_cache.c \
	mbedtls/library/ssl_ciphersuites.c \
	mbedtls/library/ssl_client.c \
	mbedtls/library/ssl_cookie.c \
	mbedtls/library/ssl_msg.c \
	mbedtls/library/ssl_ticket.c \
	mbedtls/library/ssl_tls.c \
	mbedtls/library/ssl_tls12_client.c \
	mbedtls/library/ssl_tls12_server.c \
	mbedtls/library/ssl_tls13_client.c \
	mbedtls/library/ssl_tls13_generic.c \
	mbedtls/library/ssl_tls13_keys.c \
	mbedtls/library/ssl_tls13_server.c \
	mbedtls/library/timing.c \
	mbedtls/library/version.c \
	mbedtls/library/x509.c \
	mbedtls/library/x509_create.c \
	mbedtls/library/x509_crl.c \
	mbedtls/library/x509_crt.c \
	mbedtls/library/x509_csr.c \
	mbedtls/library/x509_oid.c \
	mbedtls/library/x509write.c \
	mbedtls/library/x509write_crt.c \
	mbedtls/library/x509write_csr.c



libmbedtls_la_CFLAGS = -DMBEDTLS_CONFIG_FILE=\"mbedtls/mbedtls_config.h\" -I$(top_srcdir)/others/mbedtls/include \
						-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/include \
						-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/include \
						-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/core \
						-I$(top_srcdir)/others/mbedtls/include \
						-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/src
libmbedcrypto_la_CPPFLAGS = \
	-DMBEDTLS_CONFIG_FILE=\"mbedtls/mbedtls_config.h\" \
	-I$(top_srcdir)/others/mbedtls/include \
	-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/include \
	-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/include \
	-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/src \
	-I$(top_srcdir)/others/mbedtls/tf-psa-crypto/core

libmbedtls_la_CPPFLAGS =
libmbedtls_la_LIBADD =

#libmbedcrypto_la_CPPFLAGS =
#libmbedcrypto_la_LIBADD =
