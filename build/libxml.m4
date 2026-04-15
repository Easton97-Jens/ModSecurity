dnl Check for LIBXML2 Libraries
dnl Sets:
dnl  LIBXML2_CFLAGS
dnl  LIBXML2_LDADD
dnl  LIBXML2_LDFLAGS
dnl  LIBXML2_VERSION
dnl  LIBXML2_DISPLAY
dnl  LIBXML2_FOUND

AC_DEFUN([CHECK_LIBXML2], [
AC_ARG_WITH([libxml2-source],
    [AS_HELP_STRING([--with-libxml2-source=SOURCE],
        [Select libxml2 source: vendor (others/libxml2) or system [default=vendor]])],
    [msc_libxml2_source="$withval"],
    [msc_libxml2_source="vendor"])

AS_CASE([$msc_libxml2_source],
    [vendor|system], [],
    [AC_MSG_ERROR([Unsupported --with-libxml2-source value '$msc_libxml2_source'. Use vendor or system.])])

if test "x$with_libxml" = "xno"; then
    AC_MSG_NOTICE([LIBXML2 support disabled via --without-libxml])
    LIBXML2_FOUND=2
    LIBXML2_CFLAGS=""
    LIBXML2_LDADD=""
    LIBXML2_LDFLAGS=""
    LIBXML2_VERSION=""
    LIBXML2_DISPLAY=""
elif test "x$msc_libxml2_source" = "xvendor"; then
    LIBXML2_VENDOR_DIR="${PWD}/others/libxml2"
    LIBXML2_VENDOR_BUILD_DIR="${PWD}/others/libxml2-vendor-build"
    AC_PATH_PROG([CMAKE], [cmake])

    if test -z "$CMAKE"; then
        AC_MSG_ERROR([Vendored libxml2 requires CMake, but 'cmake' was not found in PATH.])
    fi

    if ! test -f "${LIBXML2_VENDOR_DIR}/CMakeLists.txt"; then
        AC_MSG_ERROR([\


  Vendored libxml2 was not found at ${LIBXML2_VENDOR_DIR}.
  Initialize submodules first:

     $ git submodule update --init --recursive

        ])
    fi

    AC_MSG_NOTICE([Configuring vendored libxml2 from ${LIBXML2_VENDOR_DIR}])
    AS_MKDIR_P(["${LIBXML2_VENDOR_BUILD_DIR}"])

    LIBXML2_VENDOR_CONFIGURE_CMD="\"${CMAKE}\" -S \"${LIBXML2_VENDOR_DIR}\" -B \"${LIBXML2_VENDOR_BUILD_DIR}\" -DBUILD_SHARED_LIBS=OFF -DLIBXML2_WITH_PYTHON=OFF -DLIBXML2_WITH_PROGRAMS=OFF -DLIBXML2_WITH_TESTS=OFF -DLIBXML2_WITH_ZLIB=OFF -DLIBXML2_WITH_ICONV=OFF -DLIBXML2_WITH_ICU=OFF"
    AC_MSG_NOTICE([${LIBXML2_VENDOR_CONFIGURE_CMD}])
    if ! eval "${LIBXML2_VENDOR_CONFIGURE_CMD}"; then
        AC_MSG_ERROR([Failed to configure vendored libxml2 with CMake.])
    fi

    LIBXML2_VENDOR_BUILD_CMD="\"${CMAKE}\" --build \"${LIBXML2_VENDOR_BUILD_DIR}\" --target LibXml2"
    AC_MSG_NOTICE([${LIBXML2_VENDOR_BUILD_CMD}])
    if ! eval "${LIBXML2_VENDOR_BUILD_CMD}"; then
        AC_MSG_ERROR([Failed to build vendored libxml2.])
    fi

    LIBXML2_CFLAGS="-DWITH_LIBXML2 -I${LIBXML2_VENDOR_DIR}/include -I${LIBXML2_VENDOR_BUILD_DIR} -I${LIBXML2_VENDOR_BUILD_DIR}/libxml"
    LIBXML2_LDADD="-lxml2"
    LIBXML2_LDFLAGS="-L${LIBXML2_VENDOR_BUILD_DIR} -Wl,-rpath,${LIBXML2_VENDOR_BUILD_DIR}"
    LIBXML2_VERSION=`cd "${LIBXML2_VENDOR_DIR}" && git describe --tags --always 2>/dev/null || echo unknown`
    LIBXML2_DISPLAY="vendored source: ${LIBXML2_VENDOR_DIR}, build: ${LIBXML2_VENDOR_BUILD_DIR}"
    LIBXML2_FOUND=1
else
    MSC_CHECK_LIB([LIBXML2], [libxml-2.0], [libxml/parser.h], [xml2], [-DWITH_LIBXML2], [2.6.29], [libxml])
fi

AC_SUBST([LIBXML2_CFLAGS])
AC_SUBST([LIBXML2_LDADD])
AC_SUBST([LIBXML2_LDFLAGS])
AC_SUBST([LIBXML2_VERSION])
AC_SUBST([LIBXML2_DISPLAY])
AC_SUBST([LIBXML2_FOUND])
]) # AC_DEFUN [CHECK_LIBXML2]
