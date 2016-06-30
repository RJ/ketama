/*
  +----------------------------------------------------------------------+
  | Compatibility between PHP versions                                   |
  +----------------------------------------------------------------------+
  | Copyright (c) 2015 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Francois Laupretre <francois@tekwire.net>                    |
  +----------------------------------------------------------------------+
*/

#ifndef __PECL_COMPAT_MISC_H
#define __PECL_COMPAT_MISC_H 1

#ifdef PHP_7
/*============================================================================*/

typedef zend_string * OPENED_PATH_PTR; /* Type of stream opened_path argument */
typedef size_t        COMPAT_ARG_SIZE_T; /* Size of string arguments */
typedef zend_long     COMPAT_ARG_LONG_T; /* Type of long (integer) arguments */

#define compat_zval_ptr_dtor(zp)	zval_ptr_dtor(zp)

#else
/*== PHP 5 ===================================================================*/

typedef char *    OPENED_PATH_PTR;
typedef off_t     zend_off_t;
typedef int       COMPAT_ARG_SIZE_T;
typedef long      COMPAT_ARG_LONG_T;
typedef long      zend_long;

#define compat_zval_ptr_dtor(zp)	zval_dtor(zp)

#endif
/*============================================================================*/

#ifndef MIN
#	define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#	define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

/*---------------------------------------------------------------*/
/* (Taken from pcre/pcrelib/internal.h) */
/* To cope with SunOS4 and other systems that lack memmove() but have bcopy(),
define a macro for memmove() if HAVE_MEMMOVE is false, provided that HAVE_BCOPY
is set. Otherwise, include an emulating function for those systems that have
neither (there are some non-Unix environments where this is the case). This
assumes that all calls to memmove are moving strings upwards in store,
which is the case in this extension. */

#if ! HAVE_MEMMOVE
#	ifdef memmove
#		undef  memmove					/* some systems may have a macro */
#	endif
#	if HAVE_BCOPY
#		define memmove(a, b, c) bcopy(b, a, c)
#	else
		static void *my_memmove(unsigned char *dest, const unsigned char *src,
								size_t n)
		{
			int i;

			dest += n;
			src += n;
			for (i = 0; i < n; ++i)
				*(--dest) = *(--src);
		}
#		define memmove(a, b, c) my_memmove(a, b, c)
#	endif	/* not HAVE_BCOPY */
#endif		/* not HAVE_MEMMOVE */

#ifdef _AIX
#	undef PHP_SHLIB_SUFFIX
#	define PHP_SHLIB_SUFFIX "a"
#endif

#ifndef ZVAL_IS_ARRAY
#define ZVAL_IS_ARRAY(zp)	(Z_TYPE_P((zp))==IS_ARRAY)
#endif

#ifndef ZVAL_IS_STRING
#define ZVAL_IS_STRING(zp)	(Z_TYPE_P((zp))==IS_STRING)
#endif

#ifndef ZVAL_IS_LONG
#define ZVAL_IS_LONG(zp)	(Z_TYPE_P((zp))==IS_LONG)
#endif

#ifndef ZVAL_IS_BOOL
#define ZVAL_IS_BOOL(zp)	(Z_TYPE_P((zp))==IS_BOOL)
#endif

#ifndef INIT_ZVAL
#define INIT_ZVAL(z) memset(&z, 0, sizeof(z))
#endif

#ifndef ZVAL_UNDEF
#define ZVAL_UNDEF(z) INIT_ZVAL(*(z))
#endif

#ifndef MAKE_STD_ZVAL
#define MAKE_STD_ZVAL(zp) { zp = emalloc(sizeof(zval)); INIT_ZVAL(*zp); }
#endif

#ifndef ALLOC_INIT_ZVAL
#define ALLOC_INIT_ZVAL(zp) MAKE_STD_ZVAL(zp)
#endif

#ifndef ZEND_ASSUME
#if defined(ZEND_WIN32) && !defined(__clang__)
# define ZEND_ASSUME(c) __assume(c)
#else
# define ZEND_ASSUME(c)
#endif
#endif

#ifndef ZEND_ASSERT
#if ZEND_DEBUG
# define ZEND_ASSERT(c) assert(c)
#else
# define ZEND_ASSERT(c) ZEND_ASSUME(c)
#endif
#endif

#ifndef EMPTY_SWITCH_DEFAULT_CASE
/* Only use this macro if you know for sure that all of the switches values
   are covered by its case statements */
#if ZEND_DEBUG
# define EMPTY_SWITCH_DEFAULT_CASE() default: ZEND_ASSERT(0); break;
#else
# define EMPTY_SWITCH_DEFAULT_CASE() default: ZEND_ASSUME(0); break;
#endif
#endif

#ifndef ZEND_IGNORE_VALUE
#if defined(__GNUC__) && __GNUC__ >= 4
# define ZEND_IGNORE_VALUE(x) (({ __typeof__ (x) __x = (x); (void) __x; }))
#else
# define ZEND_IGNORE_VALUE(x) ((void) (x))
#endif
#endif

#if PHP_API_VERSION >= 20100412
	typedef size_t PHP_ESCAPE_HTML_ENTITIES_SIZE;
#else
	typedef int PHP_ESCAPE_HTML_ENTITIES_SIZE;
#endif

/* Avoid a warning when compiling stream wrapper declarations for
   openfile/opendir/url_stat */

#if ZEND_EXTENSION_API_NO >= PHP_5_6_X_API_NO
#	define COMPAT_STREAM_CONST_DECL const
#else
#	define COMPAT_STREAM_CONST_DECL
#endif

#ifndef ZEND_MODULE_GLOBALS_ACCESSOR
#	ifdef ZTS
#		define ZEND_MODULE_GLOBALS_ACCESSOR(module_name, v) \
			TSRMG(module_name##_globals_id, zend_##module_name##_globals *, v)
#	else
#		define ZEND_MODULE_GLOBALS_ACCESSOR(module_name, v) \
			(module_name##_globals.v)
#	endif
#endif

#ifndef ZEND_MODULE_GLOBALS_BULK
#	ifdef ZTS
#		define ZEND_MODULE_GLOBALS_BULK(module_name) \
			((zend_##module_name##_globals *) \
				(*((void ***) tsrm_ls))[module_name##_globals_id - 1])
#	else
#		define ZEND_MODULE_GLOBALS_BULK(module_name) \
			(&module_name##_globals)
#	endif
#endif

#ifndef ZEND_TSRMLS_CACHE_DEFINE
#	define ZEND_TSRMLS_CACHE_DEFINE()
#endif

#ifndef ZEND_TSRMLS_CACHE_UPDATE
#	define ZEND_TSRMLS_CACHE_UPDATE()
#endif

#ifndef PHP_FE_END
# define PHP_FE_END { NULL, NULL, NULL }
#endif

#ifndef ZEND_MOD_END
	/* for php < 5.3.7 */
#	define ZEND_MOD_END {NULL, NULL, NULL}
#endif

/*============================================================================*/
/* Duplicate a memory buffer */
/* Supports zero size (allocates 1 byte) */

static zend_always_inline void *_compat_dup(const void *ptr, size_t size, int persistent)
{
	char *p;

	if (!ptr) return NULL;
	if (size) {
		p = pemalloc(size, persistent);
		memmove(p, ptr, size);
	} else {
		p = pemalloc(1,persistent);
		(*p) = '\0'; /* Ensures deterministic behavior */
	}

	return p;
}

/*---------------------------------------------------------------*/
/* Duplicate a string and set terminating null.
   Input string does not have to be null-terminated */

static zend_always_inline void *_compat_dup_str(const void *ptr, size_t size, int persistent)
{
	char *p;

	if (!ptr) return NULL;

	p = pemalloc(size + 1, persistent);
	if (size) memmove(p, ptr, size);
	p[size] = '\0';
	return p;
}

/*-----------------------------------------------------*/
/* Fatal error - display message and abort process */

static zend_always_inline void compat_unsupported(char *msg)
{
	php_error(E_CORE_ERROR, "This feature is not supported in this environment : %s", msg);
	exit(1);
}

/*============================================================================*/
#endif	/* __PECL_COMPAT_MISC_H */
