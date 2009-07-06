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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_ketama.h"
#include "ketama.h"

/* If you declare any globals in php_ketama.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ketama)
*/

/* True global resources - no need for thread safety here */
static int le_ketama;
static int le_ketama_continuum;

static void ketama_continuum_dtor( zend_rsrc_list_entry *rsrc TSRMLS_DC )
{
	ketama_continuum continuum = (ketama_continuum)rsrc->ptr;
	ketama_smoke( continuum );
}

/* {{{ ketama_functions[]
 *
 * Every user visible function must have an entry in ketama_functions[].
 */
zend_function_entry ketama_functions[] = {
	PHP_FE(ketama_roll,		        NULL)
	PHP_FE(ketama_destroy,		    NULL)
	PHP_FE(ketama_get_server,	    NULL)
    PHP_FE(ketama_print_continuum,  NULL)
    PHP_FE(ketama_error,            NULL)
	{NULL, NULL, NULL}	/* Must be the last line in ketama_functions[] */
};
/* }}} */

/* {{{ ketama_module_entry
 */
zend_module_entry ketama_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"ketama",
	ketama_functions,
	PHP_MINIT(ketama),
	PHP_MSHUTDOWN(ketama),
	PHP_RINIT(ketama),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(ketama),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(ketama),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_KETAMA
ZEND_GET_MODULE(ketama)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("ketama.global_value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_ketama_globals, ketama_globals)
    STD_PHP_INI_ENTRY("ketama.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_ketama_globals, ketama_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_ketama_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_ketama_init_globals(zend_ketama_globals *ketama_globals)
{
	ketama_globals->global_value = 0;
	ketama_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ketama)
{
	/* If you have INI entries, uncomment these lines 
	ZEND_INIT_MODULE_GLOBALS(ketama, php_ketama_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	*/
	le_ketama_continuum = zend_register_list_destructors_ex(ketama_continuum_dtor, NULL, "ketama continuum", module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ketama)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(ketama)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(ketama)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ketama)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ketama support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* {{{ proto resource ketama_roll()
   Rolls the ketama */
PHP_FUNCTION(ketama_roll)
{
	char *filename;
	long filename_len;

	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len ) == FAILURE )
	{
		return;
	}

	ketama_continuum c;
	if ( ketama_roll( &c, filename ) )
	{
		ZEND_REGISTER_RESOURCE( return_value, c, le_ketama_continuum );
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "unable to create Ketama continuum");
	}
}
/* }}} */


/* {{{ proto resource ketama_destroy()
   Frees the allocated ketama continuum */
PHP_FUNCTION(ketama_destroy)
{
	zval *r;
	ketama_continuum continuum;

	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &r ) == FAILURE )
	{
		return;
	}

	ZEND_FETCH_RESOURCE( continuum, ketama_continuum, &r, -1, "ketama continuum", le_ketama_continuum );
	zend_list_delete( Z_LVAL_P( r ) );
}
/* }}} */


/* {{{ proto resource ketama_print_continuum()
   Prints the given ketama continuum */
PHP_FUNCTION(ketama_print_continuum)
{
    zval *r;
    ketama_continuum continuum;

    if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "r", &r ) == FAILURE )
    {
        return;
    }

    ZEND_FETCH_RESOURCE( continuum, ketama_continuum, &r, -1, "ketama continuum", le_ketama_continuum );
    ketama_print_continuum( continuum );
}
/* }}} */


/* {{{ proto resource ketama_error()
   Prints the latest ketama error */
PHP_FUNCTION(ketama_error)
{
    RETURN_STRING( (char *)ketama_error(), 1 );
}
/* }}} */


/* {{{ proto resource ketama_get_server(string $key, resource $continuum)
   Finds a server for the given key in the continuum */
PHP_FUNCTION(ketama_get_server)
{
	zval *zcontinuum;
	ketama_continuum continuum;
	char *key;
	long key_len;
	mcs* server;

	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "sr", &key, &key_len, &zcontinuum ) == FAILURE )
	{
		return;
	}

	ZEND_FETCH_RESOURCE( continuum, ketama_continuum, &zcontinuum, -1, "ketama continuum", le_ketama_continuum );
	server = ketama_get_server( key, continuum );

	array_init( return_value );
	add_assoc_long( return_value, "point", server->point );
	add_assoc_string( return_value, "ip", server->ip, 1 );
}
/* }}} */

/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
