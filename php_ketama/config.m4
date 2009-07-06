dnl $Id$
dnl config.m4 for extension ketama

PHP_ARG_WITH(ketama, for ketama support,
[  --with-ketama             Include ketama support])

if test "$PHP_KETAMA" != "no"; then
  dnl # --with-ketama -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/ketama.h"  # you most likely want to change this
  if test -r $PHP_KETAMA/; then # path given as parameter
    KETAMA_DIR=$PHP_KETAMA
  else # search default path list
    AC_MSG_CHECKING([for ketama files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        KETAMA_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$KETAMA_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the ketama distribution])
  fi

  # --with-ketama -> add include path
  PHP_ADD_INCLUDE($KETAMA_DIR/include)

  # --with-ketama -> check for lib and symbol presence
  LIBNAME=ketama # you may want to change this
  LIBSYMBOL=ketama_roll # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $KETAMA_DIR/lib, KETAMA_SHARED_LIBADD)
    AC_DEFINE(HAVE_KETAMALIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong ketama lib version or lib not found])
  ],[
    -L$KETAMA_DIR/lib -lm -ldl
  ])

  PHP_SUBST(KETAMA_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ketama, ketama.c, $ext_shared)
fi
