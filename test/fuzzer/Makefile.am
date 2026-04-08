
# make clean
CLEANFILES =

# make maintainer-clean
MAINTAINERCLEANFILES = \
	Makefile.in


noinst_PROGRAMS = afl_fuzzer transaction_fuzzer rule_parser_fuzzer

EXTRA_DIST = \
	README.md \
	corpus/transaction/basic.bin \
	corpus/rule_parser/basic_regex.bin

afl_fuzzer_SOURCES = \
	afl_fuzzer.cc

afl_fuzzer_LDADD = \
	$(GLOBAL_LDADD) \
	$(CURL_LDADD) \
	$(GEOIP_LDFLAGS) $(GEOIP_LDADD) \
	$(PCRE_LDADD) \
	$(PCRE2_LDADD) \
	$(YAJL_LDFLAGS) $(YAJL_LDADD) \
	$(LMDB_LDFLAGS) $(LMDB_LDADD) \
	$(MAXMIND_LDFLAGS) $(MAXMIND_LDADD) \
	$(SSDEEP_LDFLAGS) $(SSDEEP_LDADD) \
	$(LUA_LDFLAGS) $(LUA_LDADD) \
	$(LIBXML2_LDADD) \
	$(top_builddir)/src/libmodsecurity.la \
	$(top_builddir)/others/libinjection.la \
	$(top_builddir)/others/libmbedtls.la

afl_fuzzer_CPPFLAGS = \
	-Icommon \
	-I../ \
	-I../../ \
	-O0 \
	-g \
	-I$(top_builddir)/headers \
	$(CURL_CFLAGS) \
	$(GEOIP_CFLAGS) \
	$(MAXMIND_CFLAGS) \
	$(GLOBAL_CPPFLAGS) \
	$(MODSEC_NO_LOGS) \
	$(YAJL_CFLAGS) \
	$(LMDB_CFLAGS) \
	$(PCRE_CFLAGS) \
	$(PCRE2_CFLAGS) \
	$(LIBXML2_CFLAGS)

transaction_fuzzer_SOURCES = \
	transaction_fuzzer.cc
transaction_fuzzer_LDADD = $(afl_fuzzer_LDADD)
transaction_fuzzer_CPPFLAGS = $(afl_fuzzer_CPPFLAGS)

rule_parser_fuzzer_SOURCES = \
	rule_parser_fuzzer.cc
rule_parser_fuzzer_LDADD = $(afl_fuzzer_LDADD)
rule_parser_fuzzer_CPPFLAGS = $(afl_fuzzer_CPPFLAGS)
