/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Christian Muehlhaeuser <chris@last.fm>                       |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.10.8.1.4.1 2006/01/01 13:46:48 sniper Exp $ */

#ifndef PHP_KETAMA_H
#define PHP_KETAMA_H

extern zend_module_entry ketama_module_entry;
#define phpext_ketama_ptr &ketama_module_entry

#ifdef PHP_WIN32
#define PHP_KETAMA_API __declspec(dllexport)
#else
#define PHP_KETAMA_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(ketama);
PHP_MSHUTDOWN_FUNCTION(ketama);
PHP_RINIT_FUNCTION(ketama);
PHP_RSHUTDOWN_FUNCTION(ketama);
PHP_MINFO_FUNCTION(ketama);

PHP_FUNCTION(ketama_roll);
PHP_FUNCTION(ketama_destroy);
PHP_FUNCTION(ketama_get_server);
PHP_FUNCTION(ketama_print_continuum);
PHP_FUNCTION(ketama_error);

/*
	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(ketama)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(ketama)
*/

/* In every utility function you add that needs to use variables
   in php_ketama_globals, call TSRMLS_FETCH(); after declaring other
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as KETAMA_G(variable).  You are
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define KETAMA_G(v) TSRMG(ketama_globals_id, zend_ketama_globals *, v)
#else
#define KETAMA_G(v) (ketama_globals.v)
#endif

#endif	/* PHP_KETAMA_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
