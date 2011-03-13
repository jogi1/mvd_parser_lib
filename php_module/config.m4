PHP_ARG_ENABLE(mvd_parsing, whether to enable MVD Parsing support,
[ --enable-mvd_parsing, Enable MVD Parsing support])

AC_MSG_CHECKING(for libmvdparse in default paths)
for i in / /usr/local /usr ; do
if test -r $i/lib/libmvdparse.so; then
 LMP_DIR=$i
 AC_MSG_RESULT(found in $i)
 fi
done

if test -z "$LMP_DIR"; then
 AC_MSG_RESULT(not found)
 AC_MSG_ERROR(Please install libmvdparse)
fi

PHP_ADD_LIBRARY_WITH_PATH(mvdparse, $LMP_DIR/lib, EXTRA_LDFLAGS)
PHP_SUBST(EXTRA_LDFLAGS)

if test "$PHP_MVD_PARSING" = "yes"; then
  AC_DEFINE(HAVE_MVD_PARSING, 1, [Whether you have MVD_Parse])
  PHP_NEW_EXTENSION(mvd_parsing, mvd_parsing.c, $ext_shared)
fi
