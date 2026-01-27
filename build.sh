#!/bin/sh
rm -rf autom4te.cache
rm -f aclocal.m4
cd src
rm -f headers.mk
echo "noinst_HEADERS = \\" > headers.mk
ls -1 \
    actions/*.h \
    actions/ctl/*.h \
    actions/data/*.h \
    actions/disruptive/*.h \
    actions/transformations/*.h \
    debug_log/*.h \
    audit_log/writer/*.h \
    collection/backend/*.h \
    operators/*.h \
    parser/*.h \
    request_body_processor/*.h \
    utils/*.h \
    variables/*.h \
    engine/*.h \
    *.h | tr "\012" " " >> headers.mk
cd ../
##############################################################################
# NEW: Automatically generate psa_crypto_driver_wrappers.h, path will be searched
##############################################################################
echo "[+] Searching for psa_crypto_driver_wrappers.h …"
if ! find . -maxdepth 10 -type f -name 'psa_crypto_driver_wrappers.h' | grep -q \
.; then
    echo "[+] File not found, searching for generate_driver_wrappers.py …"
    GEN_SCRIPT=$(find . -maxdepth 10 -type f -name 'generate_driver_wrappers.py' | head -n 1)
    if [ -n "$GEN_SCRIPT" ]; then
        GEN_DIR=$(dirname "$GEN_SCRIPT")
        echo "[+] Generator script found in: $GEN_DIR"
        (
            cd "$GEN_DIR" || exit 1
            # optional: silently install Python dependencies
            python3 -m pip install --user jinja2 jsonschema >/dev/null 2>&1 || true
            echo "[+] Running: python3 $(basename "$GEN_SCRIPT")"
            python3 "$(basename "$GEN_SCRIPT")"
        )
    else
        echo "[!] No generate_driver_wrappers.py found – skipping this step."
    fi
else
    echo "[+] psa_crypto_driver_wrappers.h already exists, nothing to do."
fi
##############################################################################
case `uname` in Darwin*) glibtoolize --force --copy ;;
  *) libtoolize --force --copy ;; esac
autoreconf --install
autoheader
automake --add-missing --foreign --copy --force-missing
autoconf --force
rm -rf autom4te.cache
