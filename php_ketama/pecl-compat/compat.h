/*
  +----------------------------------------------------------------------+
  | Compatibility macros for different PHP versions                      |
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

#ifndef _COMPAT_H
#define _COMPAT_H

#define PECL_COMPAT_VERSION 1.3

#include <stdio.h>
#include <assert.h>
#include <fcntl.h>

#include "php.h"
#include "zend.h"
#include "zend_extensions.h"
#include "zend_API.h"

#define PHP_5_0_X_API_NO                220040412
#define PHP_5_1_X_API_NO                220051025
#define PHP_5_2_X_API_NO                220060519
#define PHP_5_3_X_API_NO                220090626
#define PHP_5_4_X_API_NO                220100525
#define PHP_5_5_X_API_NO                220121212
#define PHP_5_6_X_API_NO                220131226

#if PHP_MAJOR_VERSION >= 7
#	define PHP_7
#endif

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#if HAVE_STRING_H
#	include <string.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif

#ifdef PHP_WIN32
#	include "win32/time.h"
#elif defined(NETWARE)
#	include <sys/timeval.h>
#	include <sys/time.h>
#else
#	include <sys/time.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
#	include <sys/resource.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STDLIB_H
#	include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#ifdef HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif

#ifdef PHP_WIN32
#include <win32/php_stdint.h>
#else
#include <inttypes.h>
#endif

#if ZEND_EXTENSION_API_NO >= PHP_5_6_X_API_NO
#include "zend_virtual_cwd.h"
#else
#include "TSRM/tsrm_virtual_cwd.h"
#endif

#ifdef PHP_7
#include "Zend/zend_portability.h"
#endif

/*-- Include submodules */

#include "src/misc.h"
#include "src/zend_string.h"
#include "src/zend_hash.h"
#include "src/zend_resource.h"

#endif /* _COMPAT_H */
