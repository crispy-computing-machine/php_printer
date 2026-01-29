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

PHP_MINIT_FUNCTION(printer)
{
    // Your existing init code here (e.g., registering constants or INI entries)

    // New: Detect GD resource type
    zval func_name, retval, params[2];
    ZVAL_STRING(&func_name, "imagecreatetruecolor");
    ZVAL_LONG(&params[0], 1);  // width
    ZVAL_LONG(&params[1], 1);  // height

    if (call_user_function(NULL, NULL, &func_name, &retval, 2, params) == SUCCESS &&
        Z_TYPE(retval) == IS_RESOURCE) {

        le_gd = Z_RES(retval)->type;  // Save type ID (PHP 7/8 compatible)

        // Clean up temp image
        zval destroy_func, destroy_params[1], destroy_retval;
        ZVAL_STRING(&destroy_func, "imagedestroy");
        ZVAL_COPY(&destroy_params[0], &retval);
        call_user_function(NULL, NULL, &destroy_func, &destroy_retval, 1, destroy_params);

        zval_ptr_dtor(&destroy_func);
        zval_ptr_dtor(&destroy_retval);
    } else {
        le_gd = -1;  // GD not available
    }

    zval_ptr_dtor(&func_name);
    zval_ptr_dtor(&retval);

    if (le_gd == -1) {
        php_error_docref(NULL, E_NOTICE, "GD extension not loaded - GD features in printer disabled");
    }

    // Your existing return (usually SUCCESS)
    return SUCCESS;
}


PHP_MINFO_FUNCTION(printer);    // Try to detect GD resource type by creating a temporary image
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
ZEND_FUNCTION(printer_draw_image);
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

typedef struct gdImageStruct {
    unsigned char **pixels;
    int sx;
    int sy;
    int colorsTotal;
    int red[256];
    int green[256];
    int blue[256];
    int open[256];
    int transparent;
    int *polyInts;
    int polyAllocated;
    struct gdImageStruct *brush;
    struct gdImageStruct *tile;
    int brushColorMap[256];
    int tileColorMap[256];
    int styleLength;
    int stylePos;
    int *style;
    int interlace;
    int thick;
    int alpha[256];
    int trueColor;
    int **tpixels;
    int alphaBlendingFlag;
    int saveAlphaFlag;
    int AA;
    int AA_color;
    int AA_dont_blend;
    int cx1;
    int cy1;
    int cx2;
    int cy2;
    int res_x;
    int res_y;
    int paletteQuantizationMethod;
    int paletteQuantizationSpeed;
    int paletteQuantizationMinQuality;
    int paletteQuantizationMaxQuality;
    int interpolation_id;
    // interpolation_method interpolation; // Skip if not needed
    int AALevel;
    void *fontCache; // gdFontPtr
} *gdImagePtr;

// Macros if needed (from gd.h)
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)

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