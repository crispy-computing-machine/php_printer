/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Frank M. Kromann    <frank@kromann.info>                    |
   |          Daniel Beulshausen  <daniel@php4win.de>                     |
   | Contribution:                                                        |
   |		  Philippe MAES       <luckyluke@dlfp.org>                    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_PRINTER_H
#define PHP_PRINTER_H

#if HAVE_PRINTER
#include <Winspool.h>
#include "zend_list.h" // Explicitly include
#include "printer_arginfo.h" // Explicitly include
extern zend_module_entry printer_module_entry;
#define printer_module_ptr &printer_module_entry

#define PHP_PRINTER_VERSION "0.1.0-dev"

PHP_MINIT_FUNCTION(printer);
PHP_MINFO_FUNCTION(printer);
PHP_MSHUTDOWN_FUNCTION(printer);

ZEND_FUNCTION(printer_open);
ZEND_FUNCTION(printer_close);
ZEND_FUNCTION(printer_write);
ZEND_FUNCTION(printer_list);
ZEND_FUNCTION(printer_set_option);
ZEND_FUNCTION(printer_get_option);
ZEND_FUNCTION(printer_create_dc);
ZEND_FUNCTION(printer_delete_dc);
ZEND_FUNCTION(printer_start_doc);
ZEND_FUNCTION(printer_end_doc);
ZEND_FUNCTION(printer_start_page);
ZEND_FUNCTION(printer_end_page);
ZEND_FUNCTION(printer_create_pen);
ZEND_FUNCTION(printer_delete_pen);
ZEND_FUNCTION(printer_select_pen);
ZEND_FUNCTION(printer_create_brush);
ZEND_FUNCTION(printer_delete_brush);
ZEND_FUNCTION(printer_select_brush);
ZEND_FUNCTION(printer_create_font);
ZEND_FUNCTION(printer_delete_font);
ZEND_FUNCTION(printer_select_font);
ZEND_FUNCTION(printer_logical_fontheight);
ZEND_FUNCTION(printer_draw_roundrect);
ZEND_FUNCTION(printer_draw_rectangle);
ZEND_FUNCTION(printer_draw_text);
ZEND_FUNCTION(printer_draw_ellipse);
ZEND_FUNCTION(printer_draw_line);
ZEND_FUNCTION(printer_draw_chord);
ZEND_FUNCTION(printer_draw_pie);
ZEND_FUNCTION(printer_draw_bmp);
ZEND_FUNCTION(printer_abort);

typedef struct _printer {
    HANDLE handle;
    PRINTER_INFO_2 *pi2;
    DOC_INFO_1 spooler_info; // For spooler operations (e.g., printer_write)
    DOCINFOA gdi_info;       // For GDI operations (e.g., printer_start_doc)
    HDC dc;
    char *name;
    DWORD dmModifiedFields;
} printer;

ZEND_BEGIN_MODULE_GLOBALS(printer)
	char *default_printer;
ZEND_END_MODULE_GLOBALS(printer)

#ifdef ZTS
#define PRINTERG(v) TSRMG(printer_globals_id, zend_printer_globals *, v)
#else
#define PRINTERG(v) (printer_globals.v)
#endif

#else

#define printer_module_ptr NULL

#endif

#define phpext_printer_ptr printer_module_ptr

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */