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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "ext/standard/php_math.h"

#ifdef HAVE_PRINTER

static int le_printer, le_brush, le_pen, le_font;

#ifdef PHP_WIN32
#include <windows.h>
#include <wingdi.h>
#include <winspool.h>

#include "php_printer.h"



COLORREF hex_to_rgb(char * hex);
char *rgb_to_hex(COLORREF rgb);
static void printer_dtor(zend_resource *rsrc);
static void object_dtor(zend_resource *rsrc);
char *get_default_printer(void);

zend_function_entry printer_functions[] = {
	PHP_FE(printer_open,				NULL)
	PHP_FE(printer_close,				NULL)
	PHP_FE(printer_write,				NULL)
	PHP_FE(printer_list,				NULL)
	PHP_FE(printer_set_option,			NULL)
	PHP_FE(printer_get_option,			NULL)
	PHP_FE(printer_create_dc,			NULL)
	PHP_FE(printer_delete_dc,			NULL)
	PHP_FE(printer_start_doc,			NULL)
	PHP_FE(printer_end_doc,				NULL)
	PHP_FE(printer_start_page,			NULL)
	PHP_FE(printer_end_page,			NULL)
	PHP_FE(printer_create_pen,			NULL)
	PHP_FE(printer_delete_pen,			NULL)
	PHP_FE(printer_select_pen,			NULL)
	PHP_FE(printer_create_brush,		NULL)
	PHP_FE(printer_delete_brush,		NULL)
	PHP_FE(printer_select_brush,		NULL)
	PHP_FE(printer_create_font,			NULL)
	PHP_FE(printer_delete_font,			NULL)
	PHP_FE(printer_select_font,			NULL)
	PHP_FE(printer_logical_fontheight,	NULL)
	PHP_FE(printer_draw_roundrect,		NULL)
	PHP_FE(printer_draw_rectangle,		NULL)
	PHP_FE(printer_draw_text,			NULL)
	PHP_FE(printer_draw_elipse,			NULL)
	PHP_FE(printer_draw_line,			NULL)
	PHP_FE(printer_draw_chord,			NULL)
	PHP_FE(printer_draw_pie,			NULL)
	PHP_FE(printer_draw_bmp,			NULL)
	PHP_FE(printer_abort,				NULL)
	{NULL, NULL, NULL}
};

zend_module_entry printer_module_entry = {
	STANDARD_MODULE_HEADER,
	"printer",
	printer_functions,
	PHP_MINIT(printer),
	PHP_MSHUTDOWN(printer),
	NULL,
	NULL,
	PHP_MINFO(printer),
	PHP_PRINTER_VERSION,
	STANDARD_MODULE_PROPERTIES
};

ZEND_DECLARE_MODULE_GLOBALS(printer)

#ifdef COMPILE_DL_PRINTER
ZEND_GET_MODULE(printer)
#endif


PHP_MINFO_FUNCTION(printer)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Printer Support", "enabled");
	php_info_print_table_row(2, "Version", PHP_PRINTER_VERSION);
	php_info_print_table_row(2, "Default printing device", PRINTERG(default_printer) ? PRINTERG(default_printer) : "<b>not detected</b>");
	php_info_print_table_row(2, "Module state", "working");
	php_info_print_table_row(2, "RCS Version", "$Id$");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

static PHP_INI_MH(OnUpdatePrinter)
{
    if (new_value && ZSTR_LEN(new_value) > 0) {
        if (PRINTERG(default_printer)) {
            pefree(PRINTERG(default_printer), 1);
        }
        PRINTERG(default_printer) = pestrdup(ZSTR_VAL(new_value), 1);
    }
    return SUCCESS;
}

PHP_INI_BEGIN()
	PHP_INI_ENTRY("printer.default_printer", "", PHP_INI_ALL, OnUpdatePrinter)
PHP_INI_END()

#define COPIES			0
#define MODE			1
#define TITLE			2
#define ORIENTATION		3
#define YRESOLUTION		4
#define XRESOLUTION		5
#define PAPER_FORMAT	6
#define PAPER_LENGTH	7
#define PAPER_WIDTH		8
#define SCALE			9
#define BG_COLOR		10
#define TEXT_COLOR		11
#define TEXT_ALIGN		12
#define DEVICENAME		13
#define DRIVER_VERSION	14
#define OUTPUT_FILE		15
#define VALID_OPTIONS	16
#define BRUSH_SOLID		-1
#define BRUSH_CUSTOM	-2

#define REGP_CONSTANT(a,b)	REGISTER_LONG_CONSTANT(a, b, CONST_CS | CONST_PERSISTENT);

static void php_printer_init(zend_printer_globals *printer_globals) {
	printer_globals->default_printer = get_default_printer();
}

static void php_printer_shutdown(zend_printer_globals *printer_globals) {
	if (printer_globals->default_printer) {
		pefree(printer_globals->default_printer, 1);
	}
}

PHP_MINIT_FUNCTION(printer)
{

    REGISTER_INI_ENTRIES();

    // Register resource types
    le_printer = zend_register_list_dtor_ex(printer_dtor, NULL, "printer", module_number);
    le_pen = zend_register_list_dtor_ex(object_dtor, NULL, "printer pen", module_number);
    le_font = zend_register_list_dtor_ex(object_dtor, NULL, "printer font", module_number);
    le_brush = zend_register_list_dtor_ex(object_dtor, NULL, "printer brush", module_number);

	REGP_CONSTANT("PRINTER_COPIES",				COPIES);
	REGP_CONSTANT("PRINTER_MODE",				MODE);
	REGP_CONSTANT("PRINTER_TITLE",				TITLE);
	REGP_CONSTANT("PRINTER_DEVICENAME",			DEVICENAME);
	REGP_CONSTANT("PRINTER_DRIVERVERSION",		DRIVER_VERSION);
	REGP_CONSTANT("PRINTER_OUTPUT_FILE",		OUTPUT_FILE);
	REGP_CONSTANT("PRINTER_RESOLUTION_Y",		YRESOLUTION);
	REGP_CONSTANT("PRINTER_RESOLUTION_X",		XRESOLUTION);
	REGP_CONSTANT("PRINTER_SCALE",				SCALE);
	REGP_CONSTANT("PRINTER_BACKGROUND_COLOR",	BG_COLOR);
	REGP_CONSTANT("PRINTER_PAPER_LENGTH",		PAPER_LENGTH);
	REGP_CONSTANT("PRINTER_PAPER_WIDTH",		PAPER_WIDTH);

	REGP_CONSTANT("PRINTER_PAPER_FORMAT",		PAPER_FORMAT);
	REGP_CONSTANT("PRINTER_FORMAT_CUSTOM",		0);
	REGP_CONSTANT("PRINTER_FORMAT_LETTER",		DMPAPER_LETTER);
	REGP_CONSTANT("PRINTER_FORMAT_LEGAL",		DMPAPER_LEGAL);
	REGP_CONSTANT("PRINTER_FORMAT_A3",			DMPAPER_A3);
	REGP_CONSTANT("PRINTER_FORMAT_A4",			DMPAPER_A4);
	REGP_CONSTANT("PRINTER_FORMAT_A5",			DMPAPER_A5);
	REGP_CONSTANT("PRINTER_FORMAT_B4",			DMPAPER_B4);
	REGP_CONSTANT("PRINTER_FORMAT_B5",			DMPAPER_B5);
	REGP_CONSTANT("PRINTER_FORMAT_FOLIO",		DMPAPER_FOLIO);

	REGP_CONSTANT("PRINTER_ORIENTATION",			ORIENTATION);
	REGP_CONSTANT("PRINTER_ORIENTATION_PORTRAIT",	DMORIENT_PORTRAIT);
	REGP_CONSTANT("PRINTER_ORIENTATION_LANDSCAPE",	DMORIENT_LANDSCAPE);

	REGP_CONSTANT("PRINTER_TEXT_COLOR",			TEXT_COLOR);
	REGP_CONSTANT("PRINTER_TEXT_ALIGN",			TEXT_ALIGN);
	REGP_CONSTANT("PRINTER_TA_BASELINE",		TA_BASELINE);
	REGP_CONSTANT("PRINTER_TA_BOTTOM",			TA_BOTTOM);
	REGP_CONSTANT("PRINTER_TA_TOP",				TA_TOP);
	REGP_CONSTANT("PRINTER_TA_CENTER",			TA_CENTER);
	REGP_CONSTANT("PRINTER_TA_LEFT",			TA_LEFT);
	REGP_CONSTANT("PRINTER_TA_RIGHT",			TA_RIGHT);
	
	REGP_CONSTANT("PRINTER_PEN_SOLID",			PS_SOLID);
	REGP_CONSTANT("PRINTER_PEN_DASH",			PS_DASH);
	REGP_CONSTANT("PRINTER_PEN_DOT",			PS_DOT);
	REGP_CONSTANT("PRINTER_PEN_DASHDOT",		PS_DASHDOT);
	REGP_CONSTANT("PRINTER_PEN_DASHDOTDOT",		PS_DASHDOTDOT);
	REGP_CONSTANT("PRINTER_PEN_INVISIBLE",		PS_NULL);

	REGP_CONSTANT("PRINTER_BRUSH_SOLID",		BRUSH_SOLID);
	REGP_CONSTANT("PRINTER_BRUSH_CUSTOM",		BRUSH_CUSTOM);
	REGP_CONSTANT("PRINTER_BRUSH_DIAGONAL",		HS_BDIAGONAL);
	REGP_CONSTANT("PRINTER_BRUSH_CROSS",		HS_CROSS);
	REGP_CONSTANT("PRINTER_BRUSH_DIAGCROSS",	HS_DIAGCROSS);
	REGP_CONSTANT("PRINTER_BRUSH_FDIAGONAL",	HS_FDIAGONAL);
	REGP_CONSTANT("PRINTER_BRUSH_HORIZONTAL",	HS_HORIZONTAL);
	REGP_CONSTANT("PRINTER_BRUSH_VERTICAL",		HS_VERTICAL);

	REGP_CONSTANT("PRINTER_FW_THIN",			FW_THIN);
	REGP_CONSTANT("PRINTER_FW_ULTRALIGHT",		FW_ULTRALIGHT);
	REGP_CONSTANT("PRINTER_FW_LIGHT",			FW_LIGHT);
	REGP_CONSTANT("PRINTER_FW_NORMAL",			FW_NORMAL);
	REGP_CONSTANT("PRINTER_FW_MEDIUM",			FW_MEDIUM);
	REGP_CONSTANT("PRINTER_FW_BOLD",			FW_BOLD);
	REGP_CONSTANT("PRINTER_FW_ULTRABOLD",		FW_ULTRABOLD);
	REGP_CONSTANT("PRINTER_FW_HEAVY",			FW_HEAVY);

	REGP_CONSTANT("PRINTER_ENUM_LOCAL",			PRINTER_ENUM_LOCAL);
	REGP_CONSTANT("PRINTER_ENUM_NAME",			PRINTER_ENUM_NAME);
	REGP_CONSTANT("PRINTER_ENUM_SHARED",		PRINTER_ENUM_SHARED);
	REGP_CONSTANT("PRINTER_ENUM_DEFAULT",		PRINTER_ENUM_DEFAULT);
	REGP_CONSTANT("PRINTER_ENUM_CONNECTIONS",	PRINTER_ENUM_CONNECTIONS);
	REGP_CONSTANT("PRINTER_ENUM_NETWORK",		PRINTER_ENUM_NETWORK);
	REGP_CONSTANT("PRINTER_ENUM_REMOTE",		PRINTER_ENUM_REMOTE);

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(printer)
{
#ifdef ZTS
	ts_free_id(printer_globals_id);
#else
	php_printer_shutdown(&printer_globals);
#endif
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}



ZEND_FUNCTION(printer_open)
{
    char *printer_name = NULL;
    size_t printer_name_len = 0;
    printer *resource;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(printer_name, printer_name_len)
    ZEND_PARSE_PARAMETERS_END();

    resource = emalloc(sizeof(printer));
    resource->dmModifiedFields = 0;

    if (printer_name) {
        resource->name = printer_name;
    } else {
        resource->name = PRINTERG(default_printer);
    }

    if (OpenPrinterA(resource->name, &resource->handle, NULL)) {
        resource->pi2 = emalloc(sizeof(PRINTER_INFO_2));
        resource->pi2->pDevMode = emalloc(DocumentPropertiesA(NULL, NULL, resource->name, NULL, NULL, 0));
        if (DocumentPropertiesA(NULL, resource->handle, resource->name, resource->pi2->pDevMode, NULL, DM_OUT_BUFFER) == IDOK) {
            resource->info.pDocName = estrdup("PHP generated Document");
            resource->info.pOutputFile = NULL;
            resource->info.pDatatype = estrdup("TEXT");
            // Remove fwType and cbSize as they don't exist in DOC_INFO_1
            resource->dc = CreateDCA(NULL, resource->name, NULL, resource->pi2->pDevMode);
            RETURN_RES(zend_register_resource(resource, le_printer));
        }
    }

    php_error_docref(NULL, E_WARNING, "couldn't connect to the printer [%s]", resource->name);
    efree(resource);
    RETURN_FALSE;
}


ZEND_FUNCTION(printer_close)
{
    zval *printer_res;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    zend_list_close(Z_RES_P(printer_res));
}


ZEND_FUNCTION(printer_write)
{
    zval *printer_res;
    char *content;
    size_t content_len;
    printer *resource;
    DOC_INFO_1 docinfo;
    DWORD received;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_STRING(content, content_len)
    ZEND_PARSE_PARAMETERS_END();

    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        RETURN_FALSE;
    }

    docinfo.pDocName = resource->info.pDocName;
    docinfo.pOutputFile = resource->info.pOutputFile;
    docinfo.pDatatype = resource->info.pDatatype;

    if (StartDocPrinterA(resource->handle, 1, (LPBYTE)&docinfo) && StartPagePrinter(resource->handle)) {
        WritePrinter(resource->handle, content, content_len, &received);
        EndPagePrinter(resource->handle);
        EndDocPrinter(resource->handle);
        RETURN_TRUE;
    } else {
        php_error_docref(NULL, E_WARNING, "couldn't allocate the printer job [%d]", GetLastError());
        RETURN_FALSE;
    }
}


/* {{{ proto array printer_list(int EnumType [, string Name [, int Level]])
   Return an array of printers attached to the server */
ZEND_FUNCTION(printer_list)
{
    zend_long enum_type;
    char *name = NULL;
    size_t name_len = 0;
    zend_long level = 1; // Default level
    DWORD bNeeded, cReturned;
    BYTE *info_buffer;
    int levels_allowed[] = {0, 1, 1, 0, 1, 1}; // Valid levels: 1, 2, 4, 5

    // Parse parameters: enum_type (required), name (optional), level (optional)
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_LONG(enum_type)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(name, name_len)
        Z_PARAM_LONG(level)
    ZEND_PARSE_PARAMETERS_END();

    // Validate level
    if (level < 0 || level > 5 || !levels_allowed[level]) {
        php_error_docref(NULL, E_WARNING, "Invalid level specified: %ld. Allowed levels are 1, 2, 4, 5", level);
        RETURN_FALSE;
    }

    // Initialize return array
    array_init(return_value);
    if (Z_TYPE_P(return_value) != IS_ARRAY) {
        php_error_docref(NULL, E_WARNING, "Failed to initialize return array");
        RETURN_FALSE;
    }

    // First call to get required buffer size
    EnumPrintersA(enum_type, name, level, NULL, 0, &bNeeded, &cReturned);
    if (bNeeded == 0) {
        // No printers found or error, return empty array
        return;
    }

    // Allocate buffer
    info_buffer = emalloc(bNeeded);
    if (!info_buffer) {
        php_error_docref(NULL, E_WARNING, "Memory allocation failed");
        zval_ptr_dtor(return_value);
        RETURN_FALSE;
    }

    // Enumerate printers
    if (!EnumPrintersA(enum_type, name, level, info_buffer, bNeeded, &bNeeded, &cReturned)) {
        php_error_docref(NULL, E_WARNING, "Failed to enumerate printers: %d", GetLastError());
        efree(info_buffer);
        zval_ptr_dtor(return_value);
        RETURN_FALSE;
    }

    // Process results based on level
    for (DWORD i = 0; i < cReturned; i++) {
        zval printer_info;
        array_init(&printer_info);

        switch (level) {
            case 1: {
                PRINTER_INFO_1A *p1 = &((PRINTER_INFO_1A *)info_buffer)[i];
                add_assoc_string(&printer_info, "NAME", p1->pName ? p1->pName : "");
                add_assoc_string(&printer_info, "DESCRIPTION", p1->pDescription ? p1->pDescription : "");
                add_assoc_string(&printer_info, "COMMENT", p1->pComment ? p1->pComment : "");
                break;
            }
            case 2: {
                PRINTER_INFO_2A *p2 = &((PRINTER_INFO_2A *)info_buffer)[i];
                if (p2->pServerName) add_assoc_string(&printer_info, "SERVERNAME", p2->pServerName);
                if (p2->pPrinterName) add_assoc_string(&printer_info, "PRINTERNAME", p2->pPrinterName);
                if (p2->pShareName) add_assoc_string(&printer_info, "SHARENAME", p2->pShareName);
                if (p2->pPortName) add_assoc_string(&printer_info, "PORTNAME", p2->pPortName);
                if (p2->pDriverName) add_assoc_string(&printer_info, "DRIVERNAME", p2->pDriverName);
                if (p2->pComment) add_assoc_string(&printer_info, "COMMENT", p2->pComment);
                if (p2->pLocation) add_assoc_string(&printer_info, "LOCATION", p2->pLocation);
                if (p2->pSepFile) add_assoc_string(&printer_info, "SEPFILE", p2->pSepFile);
                if (p2->pPrintProcessor) add_assoc_string(&printer_info, "PRINTPROCESSOR", p2->pPrintProcessor);
                if (p2->pDatatype) add_assoc_string(&printer_info, "DATATYPE", p2->pDatatype);
                if (p2->pParameters) add_assoc_string(&printer_info, "PARAMETRES", p2->pParameters);
                add_assoc_long(&printer_info, "ATTRIBUTES", p2->Attributes);
                add_assoc_long(&printer_info, "PRIORITY", p2->Priority);
                add_assoc_long(&printer_info, "DEFAULTPRIORITY", p2->DefaultPriority);
                add_assoc_long(&printer_info, "STARTTIME", p2->StartTime);
                add_assoc_long(&printer_info, "UNTILTIME", p2->UntilTime);
                add_assoc_long(&printer_info, "STATUS", p2->Status);
                add_assoc_long(&printer_info, "CJOBS", p2->cJobs);
                add_assoc_long(&printer_info, "AVERAGEPPM", p2->AveragePPM);
                break;
            }
            case 4: {
                PRINTER_INFO_4A *p4 = &((PRINTER_INFO_4A *)info_buffer)[i];
                add_assoc_string(&printer_info, "PRINTERNAME", p4->pPrinterName ? p4->pPrinterName : "");
                add_assoc_string(&printer_info, "SERVERNAME", p4->pServerName ? p4->pServerName : "");
                add_assoc_long(&printer_info, "ATTRIBUTES", p4->Attributes);
                break;
            }
            case 5: {
                PRINTER_INFO_5A *p5 = &((PRINTER_INFO_5A *)info_buffer)[i];
                add_assoc_string(&printer_info, "PRINTERNAME", p5->pPrinterName ? p5->pPrinterName : "");
                add_assoc_string(&printer_info, "PORTNAME", p5->pPortName ? p5->pPortName : "");
                add_assoc_long(&printer_info, "ATTRIBUTES", p5->Attributes);
                add_assoc_long(&printer_info, "DEVICENOTSELECTEDTIMEOUT", p5->DeviceNotSelectedTimeout);
                add_assoc_long(&printer_info, "TRANSMISSIONRETRYTIMEOUT", p5->TransmissionRetryTimeout);
                break;
            }
        }

        // Add sub-array to return value at index i
        add_index_zval(return_value, i, &printer_info);
    }

    efree(info_buffer);
}

/* }}} */


/* {{{ proto bool printer_set_option(resource connection,string option,mixed value)
   Configure the printer device */
ZEND_FUNCTION(printer_set_option)
{
    zval *printer_res;
    zend_long option;
    zval *value;
    printer *resource;
    PRINTER_DEFAULTS pd = {0};

    // Parse parameters: resource (required), option (required), value (required)
    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(option)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Set up printer defaults for access control
    pd.DesiredAccess = PRINTER_ALL_ACCESS;
    SetLastError(0);

    // Handle each option
    switch (option) {
        case COPIES:
            ZVAL_LONG(value, zval_get_long(value)); // Ensure value is a long
            resource->pi2->pDevMode->dmCopies = (short)Z_LVAL_P(value);
            resource->dmModifiedFields |= DM_COPIES;
            break;

        case MODE:
            ZVAL_STRING(value, zval_get_string(value)); // Ensure string
            if (resource->info.pDatatype) {
                efree((char *)resource->info.pDatatype);
            }
            resource->info.pDatatype = estrdup(Z_STRVAL_P(value));
            break;

        case TITLE:
            ZVAL_STRING(value, zval_get_string(value));
            if (resource->info.pDocName) {
                efree((char *)resource->info.pDocName);
            }
            resource->info.pDocName = estrdup(Z_STRVAL_P(value));
            break;

        case OUTPUT_FILE:
            ZVAL_STRING(value, zval_get_string(value));
            if (resource->info.pOutputFile) {
                efree((char *)resource->info.pOutputFile);
            }
            resource->info.pOutputFile = estrdup(Z_STRVAL_P(value));
            break;

        case ORIENTATION:
            ZVAL_LONG(value, zval_get_long(value));
            resource->pi2->pDevMode->dmOrientation = (short)Z_LVAL_P(value);
            resource->dmModifiedFields |= DM_ORIENTATION;
            break;

        case YRESOLUTION:
            ZVAL_LONG(value, zval_get_long(value));
            resource->pi2->pDevMode->dmYResolution = (short)Z_LVAL_P(value);
            resource->dmModifiedFields |= DM_YRESOLUTION;
            break;

        case XRESOLUTION:
            ZVAL_LONG(value, zval_get_long(value));
            resource->pi2->pDevMode->dmPrintQuality = (short)Z_LVAL_P(value);
            resource->dmModifiedFields |= DM_PRINTQUALITY;
            break;

        case PAPER_FORMAT:
            ZVAL_LONG(value, zval_get_long(value));
            resource->pi2->pDevMode->dmPaperSize = (short)Z_LVAL_P(value);
            resource->dmModifiedFields |= DM_PAPERSIZE;
            break;

        case PAPER_LENGTH:
            ZVAL_LONG(value, zval_get_long(value));
            resource->pi2->pDevMode->dmPaperLength = (short)(Z_LVAL_P(value) * 10);
            resource->dmModifiedFields |= DM_PAPERLENGTH;
            break;

        case PAPER_WIDTH:
            ZVAL_LONG(value, zval_get_long(value));
            resource->pi2->pDevMode->dmPaperWidth = (short)(Z_LVAL_P(value) * 10);
            resource->dmModifiedFields |= DM_PAPERWIDTH;
            break;

        case SCALE:
            ZVAL_LONG(value, zval_get_long(value));
            resource->pi2->pDevMode->dmScale = (short)Z_LVAL_P(value);
            resource->dmModifiedFields |= DM_SCALE;
            break;

        case BG_COLOR:
            ZVAL_STRING(value, zval_get_string(value));
            SetBkColor(resource->dc, hex_to_rgb(Z_STRVAL_P(value)));
            break;

        case TEXT_COLOR:
            ZVAL_STRING(value, zval_get_string(value));
            SetTextColor(resource->dc, hex_to_rgb(Z_STRVAL_P(value)));
            break;

        case TEXT_ALIGN:
            ZVAL_LONG(value, zval_get_long(value));
            SetTextAlign(resource->dc, Z_LVAL_P(value));
            break;

        case VALID_OPTIONS:
            resource->pi2->pSecurityDescriptor = NULL;
            resource->pi2->pDevMode->dmFields = resource->dmModifiedFields;
            resource->dmModifiedFields = 0;
            DocumentPropertiesA(NULL, resource->handle, resource->name, resource->pi2->pDevMode, resource->pi2->pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);
            SetPrinterA(resource->handle, 2, (LPBYTE)resource->pi2, 0);
            SendMessageTimeoutA(HWND_BROADCAST, WM_DEVMODECHANGE, 0L, (LPARAM)(LPCSTR)resource->name, SMTO_NORMAL, 1000, NULL);
            break;

        default:
            php_error_docref(NULL, E_WARNING, "Unknown option passed to printer_set_option(): %ld", option);
            RETURN_FALSE;
    }

    RETURN_TRUE;
}
/* }}} */


/* {{{ proto mixed printer_get_option(int handle, string option)
   Get configured data */
PHP_FUNCTION(printer_get_option)
{
	zval **arg1, **arg2;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	convert_to_long_ex(arg2);

	switch(Z_LVAL_PP(arg2)) {
		case COPIES:
			RETURN_LONG(resource->pi2->pDevMode->dmCopies);

		case MODE:
			RETURN_STRING((char*)resource->info.lpszDatatype,1);

		case TITLE:
			RETURN_STRING((char*)resource->info.lpszDocName,1);

		case OUTPUT_FILE:
			if (resource->info.lpszOutput) {
				RETURN_STRING((char*)resource->info.lpszOutput,1);
			}
			else {
				RETURN_NULL();
			}

		case ORIENTATION:
			RETURN_LONG(resource->pi2->pDevMode->dmOrientation);

		case YRESOLUTION:
			RETURN_LONG(resource->pi2->pDevMode->dmYResolution);
		
		case XRESOLUTION:
			RETURN_LONG(resource->pi2->pDevMode->dmPrintQuality);

		case PAPER_FORMAT:
			RETURN_LONG(resource->pi2->pDevMode->dmPaperSize);

		case PAPER_LENGTH:
			RETURN_LONG(resource->pi2->pDevMode->dmPaperLength / 10);

		case PAPER_WIDTH:
			RETURN_LONG(resource->pi2->pDevMode->dmPaperWidth / 10);

		case SCALE:
			RETURN_LONG(resource->pi2->pDevMode->dmScale);

		case BG_COLOR:
			RETURN_STRING(rgb_to_hex(GetBkColor(resource->dc)), 0);

		case TEXT_COLOR:
			RETURN_STRING(rgb_to_hex(GetTextColor(resource->dc)), 0);

		case TEXT_ALIGN:
			RETURN_LONG(GetTextAlign(resource->dc));

		case DEVICENAME:
			RETURN_STRING(resource->name, 1);

		case DRIVER_VERSION:
			RETURN_LONG(resource->pi2->pDevMode->dmDriverVersion);

		default:
			php_error_docref(NULL E_WARNING, "unknown option passed to printer_get_option()");
			RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto void printer_create_dc(int handle)
   Create a device content */
PHP_FUNCTION(printer_create_dc)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if( resource->dc != NULL ) {
		php_error_docref(NULL E_WARNING, "Deleting old DeviceContext");
		DeleteDC(resource->dc);
	}
	
	resource->dc = CreateDC(NULL, resource->name, NULL, resource->pi2->pDevMode);
}
/* }}} */


/* {{{ proto bool printer_delete_dc(int handle)
   Delete a device content */
PHP_FUNCTION(printer_delete_dc)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if( resource->dc != NULL ) {
		DeleteDC(resource->dc);
		resource->dc = NULL;
		RETURN_TRUE;
	}
	else {
		php_error_docref(NULL E_WARNING, "No DeviceContext created");
		RETURN_FALSE;
	}
}
/* }}} */


/* {{{ proto bool printer_start_doc(int handle)
   Start a document */
PHP_FUNCTION(printer_start_doc)
{
	zval **parameter[2];
	printer *resource;
	int argc = ZEND_NUM_ARGS();
	
	if (argc > 2 || argc < 1 || zend_get_parameters_array_ex(argc, parameter) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, parameter[0], -1, "Printer Handle", le_printer);
	
	if(argc == 2) {
		convert_to_string_ex(parameter[1]);
		if (resource->info.lpszDocName) {
			efree((char *)resource->info.lpszDocName);
		}
		resource->info.lpszDocName = estrdup(Z_STRVAL_PP(parameter[1]));
		resource->info.cbSize	   = sizeof(resource->info);
	}

	if(StartDoc(resource->dc, &resource->info) < 0) {
		php_error_docref(NULL E_WARNING, "couldn't allocate new print job");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool printer_end_doc(int handle)
   End a document */
PHP_FUNCTION(printer_end_doc)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if(EndDoc(resource->dc) < 0) {
		php_error_docref(NULL E_WARNING, "couldn't terminate print job");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool printer_start_page(int handle)
   Start a page */
PHP_FUNCTION(printer_start_page)
{
	zval **arg1;
	printer *resource;

	if( ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE ) {
		;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if(StartPage(resource->dc) < 0) {
		php_error_docref(NULL E_WARNING, "couldn't start a new page");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool printer_end_page(int handle)
   End a page */
PHP_FUNCTION(printer_end_page)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	if(EndPage(resource->dc) < 0) {
		php_error_docref(NULL E_WARNING, "couldn't end the page");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto mixed printer_create_pen(int style, int width, string color)
   Create a pen */
PHP_FUNCTION(printer_create_pen)
{
	zval **arg1, **arg2, **arg3;
	HPEN pen;

	if( zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_long_ex(arg2);
	convert_to_string_ex(arg3);

	pen = CreatePen(Z_LVAL_PP(arg1), Z_LVAL_PP(arg2), hex_to_rgb(Z_STRVAL_PP(arg3)));

	if(!pen) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, pen, le_pen);
}
/* }}} */


/* {{{ proto void printer_delete_pen(resource pen_handle)
   Delete a pen */
PHP_FUNCTION(printer_delete_pen)
{
	zval **arg1;
	HPEN pen;
	
	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(pen, HPEN, arg1, -1, "Pen Handle", le_pen);

	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */


/* {{{ proto void printer_select_pen(resource printer_handle, resource pen_handle)
   Select a pen */
PHP_FUNCTION(printer_select_pen)
{
	zval **arg1, **arg2;
	HPEN pen;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	ZEND_FETCH_RESOURCE(pen, HPEN, arg2, -1, "Pen Handle", le_pen);

	SelectObject(resource->dc, pen);
}
/* }}} */


/* {{{ proto mixed printer_create_brush(resource handle)
   Create a brush */
PHP_FUNCTION(printer_create_brush)
{
	zval **arg1, **arg2;
	HBRUSH brush;
	HBITMAP bmp;
	char* path;

	if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(arg1);
	convert_to_string_ex(arg2);

	switch(Z_LVAL_PP(arg1)) {
		case BRUSH_SOLID:
			brush = CreateSolidBrush(hex_to_rgb(Z_STRVAL_PP(arg2)));
			break;
		case BRUSH_CUSTOM:
			virtual_filepath(Z_STRVAL_PP(arg2), &path);
			bmp = LoadImage(0, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			brush = CreatePatternBrush(bmp);
			break;
		default:
			brush = CreateHatchBrush(Z_LVAL_PP(arg1), hex_to_rgb(Z_STRVAL_PP(arg2)));
	}

	if(!brush) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, brush, le_brush);
}
/* }}} */


/* {{{ proto void printer_delete_brush(resource brush_handle)
   Delete a brush */
PHP_FUNCTION(printer_delete_brush)
{
	zval **arg1;
	HBRUSH brush;
	
	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(brush, HBRUSH, arg1, -1, "Brush Handle", le_brush);

	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */


/* {{{ proto void printer_select_brush(resource printer_handle, resource brush_handle)
   Select a brush */
PHP_FUNCTION(printer_select_brush)
{
	zval **arg1, **arg2;
	HBRUSH brush;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	ZEND_FETCH_RESOURCE(brush, HBRUSH, arg2, -1, "Brush Handle", le_brush);

	SelectObject(resource->dc, brush);
}
/* }}} */


/* {{{ proto mixed printer_create_font(string face, int height, int width, int font_weight, bool italic, bool underline, bool strikeout, int orientaton)
   Create a font */
PHP_FUNCTION(printer_create_font)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	HFONT font;
	char *face;

	if( zend_get_parameters_ex(8, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	face = estrndup(Z_STRVAL_PP(arg1), 32);
	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_boolean_ex(arg5);
	convert_to_boolean_ex(arg6);
	convert_to_boolean_ex(arg7);
	convert_to_long_ex(arg8);

	font = CreateFont(Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg8), Z_LVAL_PP(arg8), Z_LVAL_PP(arg4), Z_BVAL_PP(arg5), Z_BVAL_PP(arg6), Z_BVAL_PP(arg7), DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_ROMAN, face);
	efree(face);

	if(!font) {
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, font, le_font);
}
/* }}} */


/* {{{ proto void printer_delete_font(int fonthandle)
   Delete a font */
PHP_FUNCTION(printer_delete_font)
{
	zval **arg1; 
	HFONT font;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(font, HFONT, arg1, -1, "Font Handle", le_font);

	zend_list_delete(Z_RESVAL_PP(arg1));
}
/* }}} */


/* {{{ proto void printer_select_font(int printerhandle, int fonthandle)
   Select a font */
PHP_FUNCTION(printer_select_font)
{
	zval **arg1, **arg2;
	HFONT font;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	ZEND_FETCH_RESOURCE(font, HFONT, arg2, -1, "Font Handle", le_font);

	SelectObject(resource->dc, font);
}
/* }}} */


/* {{{ proto int printer_logical_fontheight(int handle, int height)
   Get the logical font height */
PHP_FUNCTION(printer_logical_fontheight)
{
	zval **arg1, **arg2;
	printer *resource;

	if( zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);

	RETURN_LONG(MulDiv(Z_LVAL_PP(arg2), GetDeviceCaps(resource->dc, LOGPIXELSY), 72));
}
/* }}} */


/* {{{ proto void printer_draw_roundrect(resource handle, int ul_x, int ul_y, int lr_x, int lr_y, int width, int height)
   Draw a roundrect */	
PHP_FUNCTION(printer_draw_roundrect)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7; 
	printer *resource;
	
	if( zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);

	RoundRect(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5), Z_LVAL_PP(arg6), Z_LVAL_PP(arg7));
}
/* }}} */


/* {{{ proto void printer_draw_rectangle(resource handle, int ul_x, int ul_y, int lr_x, int lr_y)
   Draw a rectangle */
PHP_FUNCTION(printer_draw_rectangle)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5; 
	printer *resource;

	if( zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	Rectangle(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5));
}
/* }}} */


/* {{{ proto void printer_draw_elipse(resource handle, int ul_x, int ul_y, int lr_x, int lr_y)
   Draw an elipse */
PHP_FUNCTION(printer_draw_elipse)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5; 
	printer *resource;

	if( zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	Ellipse(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5));
}
/* }}} */


/* {{{ proto void printer_draw_text(resource handle, string text, int x, int y)
   Draw text */
PHP_FUNCTION(printer_draw_text)
{
	zval **arg1, **arg2, **arg3, **arg4; 
	printer *resource;

	if( zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);

	ExtTextOut(resource->dc, Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), ETO_OPAQUE, NULL, Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2), NULL);
}
/* }}} */


/* {{{ proto void printer_draw_line(int handle, int fx, int fy, int tx, int ty)
   Draw line from x, y to x, y*/
PHP_FUNCTION(printer_draw_line)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	printer *resource;

	if( zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);

	MoveToEx(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), NULL);
	LineTo(resource->dc, Z_LVAL_PP(arg4), Z_LVAL_PP(arg5));
}
/* }}} */


/* {{{ proto void printer_draw_chord(resource handle, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad_x, int rad_y, int rad_x1, int rad_y1)
   Draw a chord*/
PHP_FUNCTION(printer_draw_chord)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9;
	printer *resource;

	if( zend_get_parameters_ex(9, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_long_ex(arg8);
	convert_to_long_ex(arg9);

	Chord(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5), Z_LVAL_PP(arg6), Z_LVAL_PP(arg7), Z_LVAL_PP(arg8), Z_LVAL_PP(arg9));
}
/* }}} */


/* {{{ proto void printer_draw_pie(resource handle, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad1_x, int rad1_y, int rad2_x, int rad2_y)
   Draw a pie*/
PHP_FUNCTION(printer_draw_pie)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9;
	printer *resource;

	if( zend_get_parameters_ex(9, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_long_ex(arg8);
	convert_to_long_ex(arg9);

	Pie(resource->dc, Z_LVAL_PP(arg2), Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), Z_LVAL_PP(arg5), Z_LVAL_PP(arg6), Z_LVAL_PP(arg7), Z_LVAL_PP(arg8), Z_LVAL_PP(arg9));
}
/* }}} */


/* {{{ proto mixed printer_draw_bmp(resource handle, string filename, int x, int y [, int width, int height])
   Draw a bitmap */
PHP_FUNCTION(printer_draw_bmp)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5 = NULL, **arg6 = NULL;
	printer *resource;
	HBITMAP hbmp;
	BITMAP bmp_property;
	HDC dummy;
	char* path;

	int args = ZEND_NUM_ARGS();
	switch (args) {
		case 4:
			if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE ) {
				RETURN_FALSE;
			}
			break;
		case 6:
			if (zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE ) {
				RETURN_FALSE;
			}
			convert_to_long_ex(arg5);
			convert_to_long_ex(arg6);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);

	virtual_filepath(Z_STRVAL_PP(arg2), &path);

	hbmp = (HBITMAP)LoadImage(0, path, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE);

	if (hbmp == NULL) {
		php_error_docref(NULL, E_WARNING, "Failed to load bitmap %s", Z_STRVAL_PP(arg2));
		RETURN_FALSE;
	}

	if (GetObject(hbmp, sizeof(BITMAP), &bmp_property) == 0) {
		if(hbmp)
			DeleteObject(hbmp);
		RETURN_FALSE;
	}

	if (!(GetDeviceCaps(resource->dc, RASTERCAPS) & RC_STRETCHBLT)) {
		php_error_docref(NULL E_WARNING, "Printer does not support bitmaps");
		DeleteObject(hbmp);
		RETURN_FALSE;
	}
			
	if ((dummy = CreateCompatibleDC(resource->dc)) == NULL) {
		DeleteObject(hbmp);
		RETURN_FALSE;
	}
			
	if (SelectObject(dummy, hbmp) == NULL) {
		DeleteDC(dummy);
		DeleteObject(hbmp);
		RETURN_FALSE;
	}
			
	
	if (args > 4) {
		if (!StretchBlt(resource->dc,(int)Z_LVAL_PP(arg3), (int)Z_LVAL_PP(arg4), (int)Z_LVAL_PP(arg5), (int)Z_LVAL_PP(arg6), dummy, 0, 0, bmp_property.bmWidth,bmp_property.bmHeight, SRCCOPY)) {
			php_error_docref(NULL E_WARNING, "Printer failed to accept bitmap");
			DeleteDC(dummy);
			DeleteObject(hbmp);
			RETURN_FALSE;
		}
	}
	else {
		if (!BitBlt(resource->dc, Z_LVAL_PP(arg3), Z_LVAL_PP(arg4), bmp_property.bmWidth, bmp_property.bmHeight, dummy, 0, 0, SRCCOPY)) {
			php_error_docref(NULL E_WARNING, "Printer failed to accept bitmap");
			DeleteDC(dummy);
			DeleteObject(hbmp);
			RETURN_FALSE;
		}
	}

	DeleteDC(dummy);
	DeleteObject(hbmp);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto void printer_abort(resource handle)
   Abort printing*/
PHP_FUNCTION(printer_abort)
{
	zval **arg1;
	printer *resource;

	if( zend_get_parameters_ex(1, &arg1) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(resource, printer *, arg1, -1, "Printer Handle", le_printer);
	
	AbortPrinter(resource->handle);
}
/* }}} */


char *get_default_printer(void) {
	PRINTER_INFO_2 *printer;
	DWORD need, received;
	char *printer_name = NULL, *strtok_buf = NULL, buffer[250];

	if(GetVersion() < 0x80000000){
		GetProfileString("windows", "device", ",,,", buffer, 250);
		php_strtok_r(buffer, ",", &strtok_buf);
		printer_name = pestrdup(buffer, 1);
	}
	else {
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, NULL, 0, &need, &received);
		if(need > 0) {
			printer = (PRINTER_INFO_2 *)emalloc(need+1);
			EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, (LPBYTE)printer, need, &need, &received);
			printer_name = pestrdup(printer->pPrinterName, 1);
			efree(printer);
		}
	}

	return printer_name;
}


int hex2dec(char hex){
	switch(hex) { 
		case 'F': case 'f': 
			return 15;
			break;
        case 'E': case 'e':
			return 14;
			break;
        case 'D': case 'd':
			return 13;
			break;
        case 'C': case 'c':
			return 12;
			break;
        case 'B': case 'b':
			return 11;
			break;
        case 'A': case 'a':
			return 10;
        default:
			return (int)hex;
	}
}

/* convert a hexadecimal number to the rgb colorref */
COLORREF hex_to_rgb(char* hex)
{
	int r = 0,g = 0,b = 0;

	if(strlen(hex) < 6) {
		return RGB(0,0,0);
	}
	else {
		r = hex2dec(hex[0])*16 + hex2dec(hex[1]);
		g = hex2dec(hex[2])*16 + hex2dec(hex[3]);
		b = hex2dec(hex[4])*16 + hex2dec(hex[5]);
		return RGB(r,g,b);
	}

}

/* convert an rgb colorref to hex number */
char *rgb_to_hex(COLORREF rgb)
{
	char* string = emalloc(sizeof(char)*6);
	sprintf(string, "%02x%02x%02x", GetRValue(rgb), GetGValue(rgb),GetBValue(rgb));
	return string;
} 

static void printer_close(zend_rsrc_list_entry *resource)
{
	printer *p = (printer*)resource->ptr;

	ClosePrinter(p->handle);
	if (p->info.lpszDocName) {
		efree((char *)p->info.lpszDocName);
	}
	if (p->info.lpszOutput) {
		efree((char *)p->info.lpszOutput);
	}
	if (p->info.lpszDatatype) {
		efree((char *)p->info.lpszDatatype);
	}
	if (p->pi2) {
		if (p->pi2->pDevMode) {
			efree((char *)p->pi2->pDevMode);
		}
		efree((char *)p->pi2);
	}
	efree(p);
}
static void object_close(zend_rsrc_list_entry *resource)
{
	HGDIOBJ p = (HGDIOBJ)resource->ptr;
	DeleteObject(p);
}

#endif
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */