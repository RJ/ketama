/*
  +----------------------------------------------------------------------+
  | zend_string compatibility layer for PHP 5 & 7                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2007 The PHP Group                                |
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

#ifndef _COMPAT_ZEND_STRING_H
#define _COMPAT_ZEND_STRING_H

#ifdef PHP_7
/*============================================================================*/
#include "zend_string.h"

#ifndef ZSTR_IS_PERSISTENT
#	define ZSTR_IS_PERSISTENT(s) (GC_FLAGS(s) & IS_STR_PERSISTENT)
#endif

#else
/*============================================================================*/
/*---- zend_string for PHP 5 ----*/

struct _zend_string {
		int			persistent;
		int			hash_is_set; /* needed because computed hash may be null */
        zend_ulong	h; /* hash value */
		uint32_t	refcount;
        size_t		len;
        char		val[1];
};

typedef struct _zend_string zend_string;

/* Shortcuts */

#define ZSTR_VAL(zstr)  (zstr)->val
#define ZSTR_LEN(zstr)  (zstr)->len
#define ZSTR_H(zstr)    (zstr)->h
#define ZSTR_HASH(zstr) zend_string_hash_val(zstr)

#define _ZSTR_HEADER_SIZE XtOffsetOf(zend_string, val)
#define _ZSTR_STRUCT_SIZE(len) (_ZSTR_HEADER_SIZE + len + 1)

#define ZSTR_IS_PERSISTENT(s) (s->persistent)

/*---------*/

static zend_always_inline uint32_t zend_string_refcount(zend_string *s)
{
	return (s->refcount);
}

/*---------*/

static zend_always_inline uint32_t zend_string_addref(zend_string *s)
{
	return ++(s->refcount);
}

/*---------*/

static zend_always_inline uint32_t zend_string_delref(zend_string *s)
{
	return --(s->refcount);
}

/*---------*/

static zend_always_inline zend_ulong zend_string_hash_val(zend_string *s)
{
	char c, *p;

	if (! s->hash_is_set) {
		/* Compute with terminating null but preserve string */
		p = &(ZSTR_VAL(s)[ZSTR_LEN(s)]);
		c = (*p);
		(*p) = '\0';
		ZSTR_H(s) = zend_get_hash_value(ZSTR_VAL(s), ZSTR_LEN(s)+1);
		(*p) = c;
		s->hash_is_set = 1;
	}
	return ZSTR_H(s);
}

/*---------*/

static zend_always_inline void zend_string_forget_hash_val(zend_string *s)
{
	s->hash_is_set = 0;
	ZSTR_H(s) = 0; /* Security */
}

/*---------*/

static zend_always_inline zend_string *zend_string_alloc(size_t len, int persistent)
{
	zend_string *ret = (zend_string *)pemalloc(ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
	ret->persistent = persistent;
	zend_string_forget_hash_val(ret);
	ret->refcount = 1;
	ZSTR_LEN(ret) = len;
	return ret;
}

/*---------*/

static zend_always_inline zend_string *zend_string_safe_alloc(size_t n
	, size_t m, size_t l, int persistent)
{
	zend_string *ret = (zend_string *)safe_pemalloc(n, m, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(l)), persistent);
	ret->persistent = persistent;
	zend_string_forget_hash_val(ret);
	ret->refcount = 1;
	ZSTR_LEN(ret) = l;
	return ret;
}

/*---------*/

static zend_always_inline zend_string *zend_string_init(const char *str, size_t len, int persistent)
{
	zend_string *ret = zend_string_alloc(len, persistent);

	memcpy(ZSTR_VAL(ret), str, len);
	ZSTR_VAL(ret)[len] = '\0';
	return ret;
}

/*---------*/

static zend_always_inline zend_string *zend_string_dup(zend_string *s, int persistent)
{
	zend_string *target;

	target = zend_string_init(ZSTR_VAL(s), ZSTR_LEN(s), persistent);
	if (s->hash_is_set) {
		ZSTR_H(target) = ZSTR_H(s);
		target->hash_is_set = 1;
	}

	return target;
}

/*---------*/

static zend_always_inline zend_string *zend_string_realloc(zend_string *s
	, size_t len, int persistent)
{
	zend_string *ret;

	if (EXPECTED(s->refcount == 1)) {
		ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
		ZSTR_LEN(ret) = len;
		zend_string_forget_hash_val(ret);
		return ret;
	}

	zend_string_delref(s);
    ret = zend_string_alloc(len, persistent);
    memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), MIN(len, ZSTR_LEN(s)) + 1);

	return ret;
}

/*---------*/

static zend_always_inline zend_string *zend_string_extend(zend_string *s
	, size_t len, int persistent)
{
    zend_string *ret;

    ZEND_ASSERT(len >= ZSTR_LEN(s));

	if (EXPECTED(s->refcount == 1)) {
		ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
		ZSTR_LEN(ret) = len;
		zend_string_forget_hash_val(ret);
		return ret;
	}

	zend_string_delref(s);
    ret = zend_string_alloc(len, persistent);
    memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), ZSTR_LEN(s) + 1);

    return ret;
}

/*---------*/

static zend_always_inline zend_string *zend_string_truncate(zend_string *s
	, size_t len, int persistent)
{
    zend_string *ret;

    ZEND_ASSERT(len <= ZSTR_LEN(s));

	if (EXPECTED(s->refcount == 1)) {
		ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
		ZSTR_LEN(ret) = len;
		zend_string_forget_hash_val(ret);
		return ret;
	}

	zend_string_delref(s);
    ret = zend_string_alloc(len, persistent);
    memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), len + 1);

    return ret;
}

/*---------*/

static zend_always_inline zend_string *zend_string_safe_realloc(zend_string *s
	, size_t n, size_t m, size_t l, int persistent)
{
    zend_string *ret;

	if (EXPECTED(s->refcount == 1)) {
		ret = (zend_string *)safe_perealloc(s, n, m, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(l)), persistent);
		ZSTR_LEN(ret) = (n * m) + l;
		zend_string_forget_hash_val(ret);
		return ret;
	}

	zend_string_delref(s);
	ret = zend_string_safe_alloc(n, m, l, persistent);
    memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), MIN((n * m) + l, ZSTR_LEN(s)) + 1);
    return ret;
}

/*---------*/

static zend_always_inline void zend_string_free(zend_string *s)
{
	if (s) {
		pefree(s, s->persistent);
	}
}


/*---------*/

static zend_always_inline void zend_string_release(zend_string *s)
{
	if (s) {
		s->refcount--;
		if (s->refcount == 0) {
			zend_string_free(s);
		}
	}
}

/*---------*/

static zend_always_inline zend_string *zend_string_copy(zend_string *s)
{
	zend_string_addref(s);
	return (s);
}

/*---------*/

static zend_always_inline zend_bool zend_string_equals(zend_string *s1, zend_string *s2)
{
	return s1 == s2 || (s1->len == s2->len && !memcmp(s1->val, s2->val, s1->len));
}

#define zend_string_equals_literal(str, literal) \
	((str)->len == sizeof(literal)-1 && !memcmp((str)->val, literal, sizeof(literal) - 1))

#endif /* PHP_7 */
#endif /* _COMPAT_ZEND_STRING_H */
