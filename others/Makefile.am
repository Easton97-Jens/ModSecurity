
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
        mbedtls/tf-psa-crypto/core/psa_crypto.c \
        mbedtls/tf-psa-crypto/core/psa_crypto_client.c \
        mbedtls/tf-psa-crypto/core/psa_crypto_slot_management.c \
        mbedtls/tf-psa-crypto/core/psa_crypto_storage.c \
        mbedtls/tf-psa-crypto/core/psa_its_file.c \
        mbedtls/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c

MBED_BUILTIN_SOURCES = \
        mbedtls/tf-psa-crypto/drivers/builtin/src/aes.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/aesce.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/aesni.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/aria.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/asn1parse.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/asn1write.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/base64.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/bignum.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_core.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_mod.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_mod_raw.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/block_cipher.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/camellia.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ccm.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/chacha20.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/chachapoly.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/cipher.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/cipher_wrap.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/cmac.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/constant_time.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ctr_drbg.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ecdh.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ecdsa.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ecjpake.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ecp.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ecp_curves.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ecp_curves_new.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/entropy.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/entropy_poll.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/gcm.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/hmac_drbg.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/md.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/md5.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/memory_buffer_alloc.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/nist_kw.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/oid.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pem.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pk.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pk_ecc.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pk_rsa.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pk_wrap.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pkcs5.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pkparse.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/pkwrite.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/platform.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/platform_util.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/poly1305.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_aead.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_cipher.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ecp.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ffdh.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_hash.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_mac.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_pake.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_rsa.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/psa_util.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/ripemd160.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/rsa.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/rsa_alt_helpers.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/sha1.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/sha256.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/sha3.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/sha512.c \
        mbedtls/tf-psa-crypto/drivers/builtin/src/threading.c

libmbedtls_la_SOURCES = \
        $(MBED_PSA_CORE_SOURCES) \
        $(MBED_BUILTIN_SOURCES)

libmbedtls_la_CPPFLAGS = \
        -DMBEDTLS_CONFIG_FILE=\"mbedtls/mbedtls_config.h\" \
        -I$(top_srcdir)/others/mbedtls/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/core \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/include \
        -I$(top_srcdir)/others/mbedtls/tf-psa-crypto/drivers/builtin/src

#libmbedtls_la_CPPFLAGS =
libmbedtls_la_LIBADD =

