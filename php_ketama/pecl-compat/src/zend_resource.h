/*
  +----------------------------------------------------------------------+
  | Compatibility macros for different PHP versions                      |
  +----------------------------------------------------------------------+
  | Copyright (c) 2016 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Adam Harvey <aharvey@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef _COMPAT_ZEND_RESOURCE_H
#define _COMPAT_ZEND_RESOURCE_H

/*
 * The PHP 5 and PHP 7 resource APIs use the same function names for mutually
 * incompatible functions, which is unfortunate. A simple version of the PHP 5
 * macro API can be implemented on top of the PHP 7 API, but not vice versa
 * (since, for example, zend_register_resource() in PHP 5 also sets the zval,
 * which is a separate action in PHP 7).
 *
 * Instead of using preprocessor trickery to try to mangle things into a sane
 * API, I've implemented a minimal API that supports basic resource handling
 * and delegates appropriately on both versions.
 *
 * Destructors should be registered using the normal
 * zend_register_list_destructors() or zend_register_list_destructors_ex()
 * functions. The destructor function should take a "zend_resource *" (there is
 * an appropriate typedef in the PHP 5 section to make this work); as only a
 * subset of fields are available across PHP versions, this should be treated
 * as this struct in effect:
 *
 * typedef struct {
 *   void *ptr;
 *   int   type;
 * } zend_resource;
 *
 * Accessing other fields will likely result in compilation errors and/or
 * segfaults.
 */

#include "zend_list.h"

/**
 * Deletes the resource.
 *
 * On PHP 5, this is equivalent to zend_list_delete(Z_LVAL_P(zv)).
 * On PHP 7, this is equivalent to zend_list_close(Z_RES_P(zv)).
 *
 * @param zv The IS_RESOURCE zval to delete.
 */
static zend_always_inline void compat_zend_delete_resource(const zval *zv TSRMLS_DC);

/**
 * Fetches the resource.
 *
 * This API does not support the default ID that's possible with the PHP 5
 * zend_fetch_resource() API, and will always set that value to -1.
 *
 * @param zv             The IS_RESOURCE zval to fetch.
 * @param rsrc_type_name The type name to use in error messages.
 * @param rsrc_type      The resource type ID.
 * @return A void pointer to the resource, which needs to be typecast, or NULL
 *         on error.
 */
static zend_always_inline void *compat_zend_fetch_resource(zval *zv, const char *rsrc_type_name, int rsrc_type TSRMLS_DC);

/**
 * Registers a new resource.
 *
 * @param zv        The zval to set to IS_RESOURCE with the new resource value.
 * @param ptr       A void pointer to the resource.
 * @param rsrc_type The resource type ID.
 */
static zend_always_inline void compat_zend_register_resource(zval *zv, void *ptr, int rsrc_type TSRMLS_DC);

#ifdef PHP_7
/*============================================================================*/

static zend_always_inline void compat_zend_delete_resource(const zval *zv TSRMLS_DC)
{
	if (IS_RESOURCE != Z_TYPE_P(zv)) {
		return;
	}

	zend_list_close(Z_RES_P(zv));
}

/*---------*/

static zend_always_inline void *compat_zend_fetch_resource(zval *zv, const char *rsrc_type_name, int rsrc_type TSRMLS_DC)
{
	if (IS_RESOURCE != Z_TYPE_P(zv)) {
		return NULL;
	}

	return zend_fetch_resource(Z_RES_P(zv), rsrc_type_name, rsrc_type);
}

/*---------*/

static zend_always_inline void compat_zend_register_resource(zval *zv, void *ptr, int rsrc_type TSRMLS_DC)
{
	ZVAL_RES(zv, zend_register_resource(ptr, rsrc_type));
}

#else
/*== PHP 5 ===================================================================*/

/* Used for destructors. */
typedef zend_rsrc_list_entry zend_resource;

/*---------*/

static zend_always_inline void compat_zend_delete_resource(const zval *zv TSRMLS_DC)
{
	if (IS_RESOURCE != Z_TYPE_P(zv)) {
		return;
	}

	zend_list_delete(Z_LVAL_P(zv));
}

/*---------*/

static zend_always_inline void *compat_zend_fetch_resource(zval *zv, const char *rsrc_type_name, int rsrc_type TSRMLS_DC)
{
#if ZEND_MODULE_API_NO >= 20100412
	return zend_fetch_resource(&zv TSRMLS_CC, -1, rsrc_type_name, NULL, 1, rsrc_type);
#else
	return zend_fetch_resource(&zv TSRMLS_CC, -1, (char *)rsrc_type_name, NULL, 1, rsrc_type);
#endif
}

/*---------*/

static zend_always_inline void compat_zend_register_resource(zval *zv, void *ptr, int rsrc_type TSRMLS_DC)
{
	ZEND_REGISTER_RESOURCE(zv, ptr, rsrc_type);
}

#endif /* PHP_7 */
/*============================================================================*/

#endif /* _COMPAT_ZEND_RESOURCE_H */
