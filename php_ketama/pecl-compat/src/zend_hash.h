/*
  +----------------------------------------------------------------------+
  | zend_hash compatibility layer for PHP 5 & 7							 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2007 The PHP Group								 |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,		 |
  | that is bundled with this package in the file LICENSE, and is		 |
  | available through the world-wide-web at the following url:			 |
  | http://www.php.net/license/3_01.txt.								 |
  | If you did not receive a copy of the PHP license and are unable to	 |
  | obtain it through the world-wide-web, please send a note to			 |
  | license@php.net so we can mail you a copy immediately.				 |
  +----------------------------------------------------------------------+
  | Author: Francois Laupretre <francois@tekwire.net>					 |
  +----------------------------------------------------------------------+
*/

#ifndef _COMPAT_ZEND_HASH_H
#define _COMPAT_ZEND_HASH_H

#include "zend_hash.h"

#ifdef PHP_7
/*============================================================================*/

#define compat_zend_hash_exists(ht, key) zend_hash_exists(ht, key)

#define compat_zend_hash_str_exists(ht, str, len) zend_hash_str_exists(ht, str, len)

#define COMPAT_HASH_PTR(_zp) (&(Z_PTR_P(_zp)))

/*---------*/
/* Changes from original version:
	- If numeric key, returned (zend_string *) is set to NULL.
	- If non null, returned zend_string must be released.
*/
static zend_always_inline int compat_zend_hash_get_current_key_ex(const HashTable *ht
	, zend_string **str_index, zend_ulong *num_index, HashPosition *pos)
{
	int status;

	if (str_index) (*str_index) = NULL;
	status = zend_hash_get_current_key_ex(ht, str_index, num_index, pos);
	if (*str_index) zend_string_addref(*str_index);
	return status;
}

/*---------*/
/* Added - This function is equivalent to the PHP 5 version of
   zend_hash_get_current_key_ex() */

static zend_always_inline int compat_zend_hash_str_get_current_key_ex(const HashTable *ht
	, char **str_index, size_t *str_length, zend_ulong *num_index
	, zend_bool duplicate, HashPosition *pos)
{
	int status;
	zend_string *zp;

	zp = NULL;
	status = zend_hash_get_current_key_ex(ht, &zp, num_index, pos);
	if (zp) {
		if (duplicate) {
			(*str_index) = estrndup(ZSTR_VAL(zp), ZSTR_LEN(zp));
		} else {
			(*str_index) = ZSTR_VAL(zp);
		}
		(*str_length) = ZSTR_LEN(zp);
	}
	return status;
}	

/*---------*/

static zend_always_inline void *compat_zend_hash_get_current_data_ptr_ex(HashTable *ht
	, HashPosition *pos)
{
	zval *zp;

	zp = zend_hash_get_current_data_ex(ht, pos);
	return Z_PTR_P(zp);
}

#define compat_zend_hash_get_current_data_ptr(ht) \
	compat_zend_hash_get_current_data_ptr_ex(ht, &(ht)->nInternalPointer)

/*---------*/

static zend_always_inline void *compat_zend_hash_get_current_data_ex(HashTable *ht
	, HashPosition *pos)
{
	return (void *)zend_hash_get_current_data_ex(ht, pos);
}

#define compat_zend_hash_get_current_data(ht) \
	compat_zend_hash_get_current_data_ex(ht, &(ht)->nInternalPointer)

#define compat_zend_hash_get_current_zval_ex(ht, pos) \
		(zval *)compat_zend_hash_get_current_data_ex(ht, pos)

#define compat_zend_hash_get_current_zval(ht) \
	compat_zend_hash_get_current_zval_ex(ht, &(ht)->nInternalPointer)

#else
/*= PHP 5 ====================================================================*/

#ifndef HASH_KEY_NON_EXISTENT
#define HASH_KEY_NON_EXISTENT HASH_KEY_NON_EXISTANT
#endif

#define COMPAT_HASH_PTR(_zp) (_zp)

/*------------------------------------------------*/

static zend_always_inline void *zend_hash_add_ptr(HashTable *ht
	, zend_string *key, void *pData)
{
	int status;

	status = zend_hash_quick_add(ht, ZSTR_VAL(key), (uint)(ZSTR_LEN(key) + 1)
		, (ulong)ZSTR_HASH(key), &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_add_new_ptr(ht, key, pData) zend_hash_add_ptr(ht, key, pData)

#define zend_hash_add_mem(ht, key, pData, size) \
	zend_hash_add_ptr(ht, key, _compat_dup(pData, size, ht->persistent))

/*---------*/

static zend_always_inline void *zend_hash_str_add_ptr(HashTable *ht
	, const char *str, size_t len, void *pData)
{
	int status;
	char *strn = _compat_dup_str(str, len, 0);

	status = zend_hash_add(ht, strn, len + 1, &pData, sizeof(void *), NULL);
	efree(strn);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_str_add_new_ptr(ht, str, len, pData) zend_hash_str_add_ptr(ht, str, len, pData)

#define zend_hash_str_add_mem(ht, str, len, pData, size) \
	zend_hash_str_add_ptr(ht, str, len, _compat_dup(pData, size, ht->persistent))

/*---------*/

static zend_always_inline void *zend_hash_index_add_ptr(HashTable *ht
	, zend_ulong h, void *pData)
{
	int status;

	status = zend_hash_index_update(ht, (ulong)h, &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_index_add_new_ptr(ht, h, pData) \
	zend_hash_index_add_ptr(ht, h, pData)

#define zend_hash_index_add_mem(ht, h, pData, size) \
	zend_hash_index_add_ptr(ht, h, _compat_dup(pData, size, ht->persistent))

/*---------*/

static zend_always_inline void *zend_hash_next_index_insert_ptr(HashTable *ht
	, void *pData)
{
	int status;

	status = zend_hash_next_index_insert(ht, &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_next_index_insert_mem(ht, pData, size) \
	zend_hash_next_index_insert_ptr(ht, _compat_dup(pData, size, ht->persistent))

/*---------*/

static zend_always_inline void *zend_hash_update_ptr(HashTable *ht
	, zend_string *key, void *pData)
{
	int status;

	status = zend_hash_quick_update(ht, ZSTR_VAL(key), (uint)(ZSTR_LEN(key) + 1)
		, (ulong)ZSTR_HASH(key), &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_update_mem(ht, key, pData, size) \
	zend_hash_update_ptr(ht, key, _compat_dup(pData, size, ht->persistent))

/*---------*/

static zend_always_inline void *zend_hash_str_update_ptr(HashTable *ht
	, const char *str, size_t len, void *pData)
{
	int status;
	char *strn = _compat_dup_str(str, len, 0);

	status = zend_hash_update(ht, strn, len + 1, &pData, sizeof(void *), NULL);
	efree(strn);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_str_upate_mem(ht, str, len, pData, size) \
	zend_hash_str_update_ptr(ht, str, len, _compat_dup(pData, size, ht->persistent))

#define zend_hash_index_update_ptr(ht, h, pData) \
	zend_hash_index_add_ptr(ht, h, pData)

#define zend_hash_index_update_mem(ht, h, pData, size) \
	zend_hash_index_add_mem(ht, h, pData, size)

/*---------*/

static zend_always_inline void *zend_hash_find_ptr(const HashTable *ht
	, zend_string *key)
{
	int status;
	void **p;

	status = zend_hash_quick_find(ht, ZSTR_VAL(key), (uint)(ZSTR_LEN(key) + 1)
		, (ulong)ZSTR_HASH(key), (void **)(&p));
	return (status == SUCCESS ? (*p) : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_str_find_ptr(const HashTable *ht
	, const char *str, size_t len)
{
	int status;
	void **p;
	char *strn = _compat_dup_str(str, len, 0);

	status = zend_hash_find(ht, strn, len + 1, (void **)(&p));
	efree(strn);
	return (status == SUCCESS ? (*p) : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_index_find_ptr(const HashTable *ht
	, zend_ulong h)
{
	int status;
	void **p;

	status = zend_hash_index_find(ht, h, (void **)(&p));
	return (status == SUCCESS ? (*p) : NULL);
}

/*---------*/

static zend_always_inline zend_bool compat_zend_hash_exists(const HashTable *ht
	, zend_string *key)
{
	return zend_hash_quick_exists(ht, ZSTR_VAL(key), (uint)(ZSTR_LEN(key) + 1)
		, (ulong)ZSTR_HASH(key));
}

/*---------*/

static zend_always_inline zend_bool compat_zend_hash_str_exists(const HashTable *ht
	, const char *str, size_t len)
{
	zend_bool status;
	char *strn = _compat_dup_str(str, len, 0);

	status = zend_hash_exists(ht, strn, (uint)(len + 1));
	efree(strn);
	return status;
}

/*---------*/
/* Changes from original version:
	- Returns a zend_string
	- If key is a string, returned zend_string must be released.
*/
static zend_always_inline int compat_zend_hash_get_current_key_ex(const HashTable *ht
	, zend_string **str_index, zend_ulong *num_index, HashPosition *pos)
{
	int status;
	char *str;
	uint str_length;
	ulong num;

	status = zend_hash_get_current_key_ex(ht, &str, &str_length, &num, 0, pos);
	if (status == HASH_KEY_IS_STRING) {
		(*str_index) = zend_string_init(str, str_length - 1, ht->persistent);
	} else if (status == HASH_KEY_IS_LONG) {
		(*num_index) = (zend_ulong)num;
	}
	return status;
}

/*---------*/
/* Diff with original :
	- Type casts
	- Return string length, without trailing null */

static zend_always_inline int compat_zend_hash_str_get_current_key_ex(const HashTable *ht
	, char **str_index, size_t *str_length, zend_ulong *num_index
	, zend_bool duplicate, HashPosition *pos)
{
	int status;
	uint length;
	ulong num;

	status = zend_hash_get_current_key_ex(ht, str_index, &length, &num, duplicate, pos);
	if (status == HASH_KEY_IS_STRING) {
		(*str_length) = (size_t)(length - 1);
	} else if (status == HASH_KEY_IS_LONG) {
		(*num_index) = (zend_ulong)num;
	}
	return status;
}

/*---------*/

static zend_always_inline void *compat_zend_hash_get_current_data_ptr_ex(HashTable *ht
	, HashPosition *pos)
{
	int status;
	void **p;

	status = zend_hash_get_current_data_ex(ht, (void **)(&p), pos);
	return ((status == SUCCESS) ? (*p) : NULL);
}

#define compat_zend_hash_get_current_data_ptr(ht) \
	compat_zend_hash_get_current_data_ptr_ex(ht, NULL)

/*---------*/

static zend_always_inline void *compat_zend_hash_get_current_data_ex(HashTable *ht
	, HashPosition *pos)
{
	int status;
	void **p;

	status = zend_hash_get_current_data_ex(ht, (void **)(&p), pos);
	return ((status == SUCCESS) ? p : NULL);
}

#define compat_zend_hash_get_current_data(ht) \
	compat_zend_hash_get_current_data_ex(ht, NULL)

#define compat_zend_hash_get_current_zval_ex(ht, pos) \
		*((zval **)compat_zend_hash_get_current_data_ex(ht, pos))

#define compat_zend_hash_get_current_zval(ht) \
	compat_zend_hash_get_current_zval_ex(ht, NULL)

/*------------------------------------------------------------*/

#define ZEND_HASH_FOREACH(_ht, indirect) do { \
	HashPosition _pos; \
	for (zend_hash_internal_pointer_reset_ex(_ht, &_pos) \
		;;zend_hash_move_forward_ex(_ht, &_pos)) { \
		if (zend_hash_has_more_elements_ex(_ht, &_pos) != SUCCESS) break;

#define ZEND_HASH_FOREACH_END() \
	} \
} while (0)

#define ZEND_HASH_FOREACH_PTR(_ht, _ptr) \
	ZEND_HASH_FOREACH(_ht, 0); \
	_ptr = compat_zend_hash_get_current_data_ptr_ex(_ht, &_pos);

#define ZEND_HASH_FOREACH_NUM_KEY(_ht, _h) \
	ZEND_HASH_FOREACH(_ht, 0); \
	compat_zend_hash_get_current_key_ex(_ht, NULL, &_h, &_pos);

#define ZEND_HASH_FOREACH_NUM_KEY_PTR(_ht, _h, _ptr) \
	ZEND_HASH_FOREACH(_ht, 0); \
	_ptr = compat_zend_hash_get_current_data_ptr_ex(_ht, &_pos); \
	compat_zend_hash_get_current_key_ex(_ht, NULL, &_h, &_pos);

/*============================================================================*/
#endif /* PHP_7 */
#endif /* _COMPAT_ZEND_HASH_H */
