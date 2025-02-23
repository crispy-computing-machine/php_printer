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
            resource->spooler_info.pDocName = estrdup("PHP generated Document");
            resource->spooler_info.pOutputFile = NULL;
            resource->spooler_info.pDatatype = estrdup("TEXT");
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

    // Parse parameters: printer resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource (optional, since we’re just closing)
    printer *resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Close the printer resource
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

    docinfo.pDocName = resource->spooler_info.pDocName;
    docinfo.pOutputFile = resource->spooler_info.pOutputFile;
    docinfo.pDatatype = resource->spooler_info.pDatatype;

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
            if (resource->spooler_info.pDatatype) {
                efree((char *)resource->spooler_info.pDatatype);
            }
            resource->spooler_info.pDatatype = estrdup(Z_STRVAL_P(value));
            break;

        case TITLE:
            ZVAL_STRING(value, zval_get_string(value));
            if (resource->spooler_info.pDocName) {
                efree((char *)resource->spooler_info.pDocName);
            }
            resource->spooler_info.pDocName = estrdup(Z_STRVAL_P(value));
            break;

        case OUTPUT_FILE:
            ZVAL_STRING(value, zval_get_string(value));
            if (resource->spooler_info.pOutputFile) {
                efree((char *)resource->spooler_info.pOutputFile);
            }
            resource->spooler_info.pOutputFile = estrdup(Z_STRVAL_P(value));
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
ZEND_FUNCTION(printer_get_option)
{
    zval *printer_res;
    zend_long option;
    printer *resource;

    // Parse parameters: resource (required), option (required)
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(option)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Handle each option
    switch (option) {
        case COPIES:
            RETURN_LONG(resource->pi2->pDevMode->dmCopies);

        case MODE:
            RETURN_STRING(resource->spooler_info.pDatatype ? resource->spooler_info.pDatatype : "");

        case TITLE:
            RETURN_STRING(resource->spooler_info.pDocName ? resource->spooler_info.pDocName : "");

        case OUTPUT_FILE:
            if (resource->spooler_info.pOutputFile) {
                RETURN_STRING(resource->spooler_info.pOutputFile);
            } else {
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

        case BG_COLOR: {
            char *hex = rgb_to_hex(GetBkColor(resource->dc));
            RETURN_STRING(hex); // hex is allocated by rgb_to_hex, will be freed by Zend
        }

        case TEXT_COLOR: {
            char *hex = rgb_to_hex(GetTextColor(resource->dc));
            RETURN_STRING(hex);
        }

        case TEXT_ALIGN:
            RETURN_LONG(GetTextAlign(resource->dc));

        case DEVICENAME:
            RETURN_STRING(resource->name ? resource->name : "");

        case DRIVER_VERSION:
            RETURN_LONG(resource->pi2->pDevMode->dmDriverVersion);

        default:
            php_error_docref(NULL, E_WARNING, "Unknown option passed to printer_get_option(): %ld", option);
            RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto void printer_create_dc(int handle)
   Create a device content */
ZEND_FUNCTION(printer_create_dc)
{
    zval *printer_res;
    printer *resource;

    // Parse parameters: resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return; // No return value expected, so just return
    }

    // Delete old device context if it exists
    if (resource->dc != NULL) {
        php_error_docref(NULL, E_WARNING, "Deleting old DeviceContext");
        DeleteDC(resource->dc);
    }

    // Create new device context
    resource->dc = CreateDCA(NULL, resource->name, NULL, resource->pi2->pDevMode);
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "Failed to create new DeviceContext: %d", GetLastError());
    }
}
/* }}} */


/* {{{ proto bool printer_delete_dc(int handle)
   Delete a device content */
ZEND_FUNCTION(printer_delete_dc)
{
    zval *printer_res;
    printer *resource;

    // Parse parameters: resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Delete the device context if it exists
    if (resource->dc != NULL) {
        if (DeleteDC(resource->dc)) {
            resource->dc = NULL;
            RETURN_TRUE;
        } else {
            php_error_docref(NULL, E_WARNING, "Failed to delete DeviceContext: %d", GetLastError());
            RETURN_FALSE;
        }
    } else {
        php_error_docref(NULL, E_WARNING, "No DeviceContext created");
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto bool printer_start_doc(int handle)
   Start a document */
ZEND_FUNCTION(printer_start_doc)
{
    zval *printer_res;
    char *doc_name = NULL;
    size_t doc_name_len = 0;
    printer *resource;

    // Parse parameters: resource (required), doc_name (optional)
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(doc_name, doc_name_len)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Update document name if provided
    if (doc_name) {
        if (resource->gdi_info.lpszDocName) {
            efree((char *)resource->gdi_info.lpszDocName);
        }
        resource->gdi_info.lpszDocName = estrdup(doc_name);
    } else if (!resource->gdi_info.lpszDocName) {
        // Default name if not set
        resource->gdi_info.lpszDocName = estrdup("PHP Document");
    }

    // Initialize DOCINFO if not already set
    resource->gdi_info.cbSize = sizeof(DOCINFOA);
    if (!resource->gdi_info.lpszOutput) {
        resource->gdi_info.lpszOutput = NULL;
    }
    if (!resource->gdi_info.lpszDatatype) {
        resource->gdi_info.lpszDatatype = NULL; // Default to printer's default datatype
    }
    resource->gdi_info.fwType = 0;

    // Start the document
    if (StartDocA(resource->dc, &resource->gdi_info) > 0) {
        RETURN_TRUE;
    } else {
        php_error_docref(NULL, E_WARNING, "Couldn't allocate new print job: %d", GetLastError());
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto bool printer_end_doc(int handle)
   End a document */
ZEND_FUNCTION(printer_end_doc)
{
    zval *printer_res;
    printer *resource;

    // Parse parameters: resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to end document");
        RETURN_FALSE;
    }

    // End the document
    if (EndDoc(resource->dc) > 0) {
        RETURN_TRUE;
    } else {
        php_error_docref(NULL, E_WARNING, "Couldn't terminate print job: %d", GetLastError());
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto bool printer_start_page(int handle)
   Start a page */
ZEND_FUNCTION(printer_start_page)
{
    zval *printer_res;
    printer *resource;

    // Parse parameters: resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to start page");
        RETURN_FALSE;
    }

    // Start the page
    if (StartPage(resource->dc) > 0) {
        RETURN_TRUE;
    } else {
        php_error_docref(NULL, E_WARNING, "Couldn't start a new page: %d", GetLastError());
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto bool printer_end_page(int handle)
   End a page */
ZEND_FUNCTION(printer_end_page)
{
    zval *printer_res;
    printer *resource;

    // Parse parameters: resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to end page");
        RETURN_FALSE;
    }

    // End the page
    if (EndPage(resource->dc) > 0) {
        RETURN_TRUE;
    } else {
        php_error_docref(NULL, E_WARNING, "Couldn't end the page: %d", GetLastError());
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto mixed printer_create_pen(int style, int width, string color)
   Create a pen */
ZEND_FUNCTION(printer_create_pen)
{
    zend_long style;
    zend_long width;
    char *color;
    size_t color_len;
    HPEN pen;

    // Parse parameters: style (required), width (required), color (required)
    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_LONG(style)
        Z_PARAM_LONG(width)
        Z_PARAM_STRING(color, color_len)
    ZEND_PARSE_PARAMETERS_END();

    // Create the pen
    pen = CreatePen((int)style, (int)width, hex_to_rgb(color));
    if (!pen) {
        php_error_docref(NULL, E_WARNING, "Failed to create pen: %d", GetLastError());
        RETURN_FALSE;
    }

    // Register and return the pen resource
    RETURN_RES(zend_register_resource(pen, le_pen));
}
/* }}} */


/* {{{ proto void printer_delete_pen(resource pen_handle)
   Delete a pen */
ZEND_FUNCTION(printer_delete_pen)
{
    zval *pen_res;
    HPEN pen;

    // Parse parameters: pen resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(pen_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the pen resource
    pen = zend_fetch_resource(Z_RES_P(pen_res), "Pen Handle", le_pen);
    if (!pen) {
        php_error_docref(NULL, E_WARNING, "Invalid pen resource");
        RETURN_FALSE;
    }

    // Delete the pen resource
    zend_list_close(Z_RES_P(pen_res));
    RETURN_TRUE;
}
/* }}} */


/* {{{ proto void printer_select_pen(resource printer_handle, resource pen_handle)
   Select a pen */
ZEND_FUNCTION(printer_select_pen)
{
    zval *printer_res, *pen_res;
    printer *resource;
    HPEN pen;

    // Parse parameters: printer resource (required), pen resource (required)
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_RESOURCE(pen_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Fetch the pen resource
    pen = zend_fetch_resource(Z_RES_P(pen_res), "Pen Handle", le_pen);
    if (!pen) {
        php_error_docref(NULL, E_WARNING, "Invalid pen resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to select pen");
        return;
    }

    // Select the pen into the device context
    SelectObject(resource->dc, pen);
}
/* }}} */


/* {{{ proto mixed printer_create_brush(resource handle)
   Create a brush */
ZEND_FUNCTION(printer_create_brush)
{
    zend_long style;
    char *param;
    size_t param_len;
    HBRUSH brush;
    HBITMAP bmp = NULL;

    // Parse parameters: style (required), param (required)
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(style)
        Z_PARAM_STRING(param, param_len)
    ZEND_PARSE_PARAMETERS_END();

    // Create the brush based on style
    switch (style) {
        case BRUSH_SOLID:
            brush = CreateSolidBrush(hex_to_rgb(param));
            break;

        case BRUSH_CUSTOM: {
            char *path;
            virtual_filepath(param, &path); // Resolve virtual path (PHP function)
            bmp = (HBITMAP)LoadImageA(0, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (!bmp) {
                php_error_docref(NULL, E_WARNING, "Failed to load bitmap for custom brush: %s (%d)", param, GetLastError());
                RETURN_FALSE;
            }
            brush = CreatePatternBrush(bmp);
            DeleteObject(bmp); // Clean up bitmap after creating brush
            break;
        }

        default:
            brush = CreateHatchBrush((int)style, hex_to_rgb(param));
            break;
    }

    // Check if brush creation succeeded
    if (!brush) {
        php_error_docref(NULL, E_WARNING, "Failed to create brush: %d", GetLastError());
        RETURN_FALSE;
    }

    // Register and return the brush resource
    RETURN_RES(zend_register_resource(brush, le_brush));
}
/* }}} */


/* {{{ proto void printer_delete_brush(resource brush_handle)
   Delete a brush */
ZEND_FUNCTION(printer_delete_brush)
{
    zval *brush_res;
    HBRUSH brush;

    // Parse parameters: brush resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(brush_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the brush resource
    brush = zend_fetch_resource(Z_RES_P(brush_res), "Brush Handle", le_brush);
    if (!brush) {
        php_error_docref(NULL, E_WARNING, "Invalid brush resource");
        return; // Void return, consistent with original
    }

    // Delete the brush resource
    zend_list_close(Z_RES_P(brush_res));
}
/* }}} */


/* {{{ proto void printer_select_brush(resource printer_handle, resource brush_handle)
   Select a brush */
ZEND_FUNCTION(printer_select_brush)
{
    zval *printer_res, *brush_res;
    printer *resource;
    HBRUSH brush;

    // Parse parameters: printer resource (required), brush resource (required)
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_RESOURCE(brush_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Fetch the brush resource
    brush = zend_fetch_resource(Z_RES_P(brush_res), "Brush Handle", le_brush);
    if (!brush) {
        php_error_docref(NULL, E_WARNING, "Invalid brush resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to select brush");
        return;
    }

    // Select the brush into the device context
    SelectObject(resource->dc, brush);
}
/* }}} */


/* {{{ proto mixed printer_create_font(string face, int height, int width, int font_weight, bool italic, bool underline, bool strikeout, int orientaton)
   Create a font */
ZEND_FUNCTION(printer_create_font)
{
    char *face;
    size_t face_len;
    zend_long height, width, font_weight, italic, underline, strikeout, orientation;
    HFONT font;

    // Parse parameters: 8 required arguments
    ZEND_PARSE_PARAMETERS_START(8, 8)
        Z_PARAM_STRING(face, face_len)
        Z_PARAM_LONG(height)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(font_weight)
        Z_PARAM_BOOL(italic)
        Z_PARAM_BOOL(underline)
        Z_PARAM_BOOL(strikeout)
        Z_PARAM_LONG(orientation)
    ZEND_PARSE_PARAMETERS_END();

    // Limit face name to 32 characters (LF_FACESIZE - 1, including null terminator)
    char face_name[LF_FACESIZE];
    if (face_len >= LF_FACESIZE) {
        php_error_docref(NULL, E_WARNING, "Font face name exceeds maximum length of %d characters", LF_FACESIZE - 1);
        RETURN_FALSE;
    }
    strncpy(face_name, face, face_len);
    face_name[face_len] = '\0';

    // Create the font
    font = CreateFontA(
        (int)height,           // Height
        (int)width,            // Width
        (int)orientation,      // Escapement
        (int)orientation,      // Orientation (same as escapement here, per original)
        (int)font_weight,      // Weight
        (DWORD)italic,         // Italic
        (DWORD)underline,      // Underline
        (DWORD)strikeout,      // Strikeout
        DEFAULT_CHARSET,       // Character set
        OUT_DEFAULT_PRECIS,    // Output precision
        CLIP_DEFAULT_PRECIS,   // Clipping precision
        ANTIALIASED_QUALITY,   // Quality
        DEFAULT_PITCH | FF_ROMAN, // Pitch and family
        face_name              // Face name
    );

    // Check if font creation succeeded
    if (!font) {
        php_error_docref(NULL, E_WARNING, "Failed to create font: %d", GetLastError());
        RETURN_FALSE;
    }

    // Register and return the font resource
    RETURN_RES(zend_register_resource(font, le_font));
}
/* }}} */


/* {{{ proto void printer_delete_font(int fonthandle)
   Delete a font */
ZEND_FUNCTION(printer_delete_font)
{
    zval *font_res;
    HFONT font;

    // Parse parameters: font resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(font_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the font resource
    font = zend_fetch_resource(Z_RES_P(font_res), "Font Handle", le_font);
    if (!font) {
        php_error_docref(NULL, E_WARNING, "Invalid font resource");
        return; // Void return, consistent with original
    }

    // Delete the font resource
    zend_list_close(Z_RES_P(font_res));
}
/* }}} */


/* {{{ proto void printer_select_font(int printerhandle, int fonthandle)
   Select a font */
ZEND_FUNCTION(printer_select_font)
{
    zval *printer_res, *font_res;
    printer *resource;
    HFONT font;

    // Parse parameters: printer resource (required), font resource (required)
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_RESOURCE(font_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Fetch the font resource
    font = zend_fetch_resource(Z_RES_P(font_res), "Font Handle", le_font);
    if (!font) {
        php_error_docref(NULL, E_WARNING, "Invalid font resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to select font");
        return;
    }

    // Select the font into the device context
    SelectObject(resource->dc, font);
}
/* }}} */


/* {{{ proto int printer_logical_fontheight(int handle, int height)
   Get the logical font height */
ZEND_FUNCTION(printer_logical_fontheight)
{
    zval *printer_res;
    zend_long height;
    printer *resource;

    // Parse parameters: printer resource (required), height (required)
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(height)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to calculate font height");
        RETURN_FALSE;
    }

    // Calculate logical font height
    int logical_height = MulDiv((int)height, GetDeviceCaps(resource->dc, LOGPIXELSY), 72);
    if (logical_height == 0) {
        php_error_docref(NULL, E_WARNING, "Failed to calculate logical font height: %d", GetLastError());
        RETURN_FALSE;
    }

    RETURN_LONG(logical_height);
}
/* }}} */


/* {{{ proto void printer_draw_roundrect(resource handle, int ul_x, int ul_y, int lr_x, int lr_y, int width, int height)
   Draw a roundrect */	
ZEND_FUNCTION(printer_draw_roundrect)
{
    zval *printer_res;
    zend_long ul_x, ul_y, lr_x, lr_y, width, height;
    printer *resource;

    // Parse parameters: printer resource (required), 6 long values (required)
    ZEND_PARSE_PARAMETERS_START(7, 7)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(ul_x)
        Z_PARAM_LONG(ul_y)
        Z_PARAM_LONG(lr_x)
        Z_PARAM_LONG(lr_y)
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw roundrect");
        return;
    }

    // Draw the rounded rectangle
    if (!RoundRect(resource->dc, (int)ul_x, (int)ul_y, (int)lr_x, (int)lr_y, (int)width, (int)height)) {
        php_error_docref(NULL, E_WARNING, "Failed to draw roundrect: %d", GetLastError());
    }
}
/* }}} */


/* {{{ proto void printer_draw_rectangle(resource handle, int ul_x, int ul_y, int lr_x, int lr_y)
   Draw a rectangle */
ZEND_FUNCTION(printer_draw_rectangle)
{
    zval *printer_res;
    zend_long ul_x, ul_y, lr_x, lr_y;
    printer *resource;

    // Parse parameters: printer resource (required), 4 long values (required)
    ZEND_PARSE_PARAMETERS_START(5, 5)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(ul_x)
        Z_PARAM_LONG(ul_y)
        Z_PARAM_LONG(lr_x)
        Z_PARAM_LONG(lr_y)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw rectangle");
        return;
    }

    // Draw the rectangle
    if (!Rectangle(resource->dc, (int)ul_x, (int)ul_y, (int)lr_x, (int)lr_y)) {
        php_error_docref(NULL, E_WARNING, "Failed to draw rectangle: %d", GetLastError());
    }
}
/* }}} */


/* {{{ proto void printer_draw_elipse(resource handle, int ul_x, int ul_y, int lr_x, int lr_y)
   Draw an elipse */
ZEND_FUNCTION(printer_draw_ellipse)
{
    zval *printer_res;
    zend_long ul_x, ul_y, lr_x, lr_y;
    printer *resource;

    // Parse parameters: printer resource (required), 4 long values (required)
    ZEND_PARSE_PARAMETERS_START(5, 5)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(ul_x)
        Z_PARAM_LONG(ul_y)
        Z_PARAM_LONG(lr_x)
        Z_PARAM_LONG(lr_y)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw ellipse");
        return;
    }

    // Draw the ellipse
    if (!Ellipse(resource->dc, (int)ul_x, (int)ul_y, (int)lr_x, (int)lr_y)) {
        php_error_docref(NULL, E_WARNING, "Failed to draw ellipse: %d", GetLastError());
    }
}
/* }}} */


/* {{{ proto void printer_draw_text(resource handle, string text, int x, int y)
   Draw text */
ZEND_FUNCTION(printer_draw_text)
{
    zval *printer_res;
    char *text;
    size_t text_len;
    zend_long x, y;
    printer *resource;

    // Parse parameters: printer resource (required), text (required), x (required), y (required)
    ZEND_PARSE_PARAMETERS_START(4, 4)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_STRING(text, text_len)
        Z_PARAM_LONG(x)
        Z_PARAM_LONG(y)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw text");
        return;
    }

    // Draw the text
    if (!ExtTextOutA(resource->dc, (int)x, (int)y, ETO_OPAQUE, NULL, text, (UINT)text_len, NULL)) {
        php_error_docref(NULL, E_WARNING, "Failed to draw text: %d", GetLastError());
    }
}
/* }}} */


/* {{{ proto void printer_draw_line(int handle, int fx, int fy, int tx, int ty)
   Draw line from x, y to x, y*/
ZEND_FUNCTION(printer_draw_line)
{
    zval *printer_res;
    zend_long fx, fy, tx, ty;
    printer *resource;

    // Parse parameters: printer resource (required), 4 long values (required)
    ZEND_PARSE_PARAMETERS_START(5, 5)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(fx)
        Z_PARAM_LONG(fy)
        Z_PARAM_LONG(tx)
        Z_PARAM_LONG(ty)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw line");
        return;
    }

    // Draw the line
    if (!MoveToEx(resource->dc, (int)fx, (int)fy, NULL)) {
        php_error_docref(NULL, E_WARNING, "Failed to set starting point for line: %d", GetLastError());
        return;
    }
    if (!LineTo(resource->dc, (int)tx, (int)ty)) {
        php_error_docref(NULL, E_WARNING, "Failed to draw line: %d", GetLastError());
    }
}
/* }}} */


/* {{{ proto void printer_draw_chord(resource handle, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad_x, int rad_y, int rad_x1, int rad_y1)
   Draw a chord*/
ZEND_FUNCTION(printer_draw_chord)
{
    zval *printer_res;
    zend_long rec_x, rec_y, rec_x1, rec_y1, rad_x, rad_y, rad_x1, rad_y1;
    printer *resource;

    // Parse parameters: printer resource (required), 8 long values (required)
    ZEND_PARSE_PARAMETERS_START(9, 9)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(rec_x)
        Z_PARAM_LONG(rec_y)
        Z_PARAM_LONG(rec_x1)
        Z_PARAM_LONG(rec_y1)
        Z_PARAM_LONG(rad_x)
        Z_PARAM_LONG(rad_y)
        Z_PARAM_LONG(rad_x1)
        Z_PARAM_LONG(rad_y1)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw chord");
        return;
    }

    // Draw the chord
    if (!Chord(resource->dc, (int)rec_x, (int)rec_y, (int)rec_x1, (int)rec_y1,
               (int)rad_x, (int)rad_y, (int)rad_x1, (int)rad_y1)) {
        php_error_docref(NULL, E_WARNING, "Failed to draw chord: %d", GetLastError());
    }
}
/* }}} */


/* {{{ proto void printer_draw_pie(resource handle, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad1_x, int rad1_y, int rad2_x, int rad2_y)
   Draw a pie*/
ZEND_FUNCTION(printer_draw_pie)
{
    zval *printer_res;
    zend_long rec_x, rec_y, rec_x1, rec_y1, rad1_x, rad1_y, rad2_x, rad2_y;
    printer *resource;

    // Parse parameters: printer resource (required), 8 long values (required)
    ZEND_PARSE_PARAMETERS_START(9, 9)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_LONG(rec_x)
        Z_PARAM_LONG(rec_y)
        Z_PARAM_LONG(rec_x1)
        Z_PARAM_LONG(rec_y1)
        Z_PARAM_LONG(rad1_x)
        Z_PARAM_LONG(rad1_y)
        Z_PARAM_LONG(rad2_x)
        Z_PARAM_LONG(rad2_y)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw pie");
        RETURN_FALSE;
    }

    // Draw the pie
    if (Pie(resource->dc, (int)rec_x, (int)rec_y, (int)rec_x1, (int)rec_y1,
            (int)rad1_x, (int)rad1_y, (int)rad2_x, (int)rad2_y)) {
        RETURN_TRUE;
    } else {
        php_error_docref(NULL, E_WARNING, "Failed to draw pie: %d", GetLastError());
        RETURN_FALSE;
    }
}
/* }}} */


/* {{{ proto mixed printer_draw_bmp(resource handle, string filename, int x, int y [, int width, int height])
   Draw a bitmap */
ZEND_FUNCTION(printer_draw_bmp)
{
    zval *printer_res;
    char *filename;
    size_t filename_len;
    zend_long x, y;
    zend_long width = 0, height = 0;
    printer *resource;
    HBITMAP hbmp = NULL;
    HDC dummy = NULL;
    BITMAP bmp_property;

    // Parse parameters: printer resource (required), filename (required), x (required), y (required),
    // width (optional), height (optional)
    ZEND_PARSE_PARAMETERS_START(4, 6)
        Z_PARAM_RESOURCE(printer_res)
        Z_PARAM_STRING(filename, filename_len)
        Z_PARAM_LONG(x)
        Z_PARAM_LONG(y)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(width)
        Z_PARAM_LONG(height)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        RETURN_FALSE;
    }

    // Check if a device context exists
    if (resource->dc == NULL) {
        php_error_docref(NULL, E_WARNING, "No DeviceContext available to draw bitmap");
        RETURN_FALSE;
    }

    // Resolve virtual file path
    char *path;
    virtual_filepath(filename, &path);

    // Load the bitmap
    hbmp = (HBITMAP)LoadImageA(0, path, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    if (hbmp == NULL) {
        php_error_docref(NULL, E_WARNING, "Failed to load bitmap %s: %d", filename, GetLastError());
        RETURN_FALSE;
    }

    // Get bitmap properties
    if (GetObject(hbmp, sizeof(BITMAP), &bmp_property) == 0) {
        php_error_docref(NULL, E_WARNING, "Failed to get bitmap properties: %d", GetLastError());
        DeleteObject(hbmp);
        RETURN_FALSE;
    }

    // Check printer capabilities
    if (!(GetDeviceCaps(resource->dc, RASTERCAPS) & RC_STRETCHBLT)) {
        php_error_docref(NULL, E_WARNING, "Printer does not support bitmaps");
        DeleteObject(hbmp);
        RETURN_FALSE;
    }

    // Create a compatible DC
    dummy = CreateCompatibleDC(resource->dc);
    if (dummy == NULL) {
        php_error_docref(NULL, E_WARNING, "Failed to create compatible DC: %d", GetLastError());
        DeleteObject(hbmp);
        RETURN_FALSE;
    }

    // Select the bitmap into the compatible DC
    if (SelectObject(dummy, hbmp) == NULL) {
        php_error_docref(NULL, E_WARNING, "Failed to select bitmap into compatible DC: %d", GetLastError());
        DeleteDC(dummy);
        DeleteObject(hbmp);
        RETURN_FALSE;
    }

    // Draw the bitmap (with optional stretching)
    BOOL result;
    if (width > 0 && height > 0) {
        result = StretchBlt(resource->dc, (int)x, (int)y, (int)width, (int)height,
                            dummy, 0, 0, bmp_property.bmWidth, bmp_property.bmHeight, SRCCOPY);
    } else {
        result = BitBlt(resource->dc, (int)x, (int)y, bmp_property.bmWidth, bmp_property.bmHeight,
                        dummy, 0, 0, SRCCOPY);
    }

    // Clean up
    DeleteDC(dummy);
    DeleteObject(hbmp);

    // Check drawing result
    if (!result) {
        php_error_docref(NULL, E_WARNING, "Failed to draw bitmap: %d", GetLastError());
        RETURN_FALSE;
    }

    RETURN_TRUE;
}
/* }}} */


/* {{{ proto void printer_abort(resource handle)
   Abort printing*/
ZEND_FUNCTION(printer_abort)
{
    zval *printer_res;
    printer *resource;

    // Parse parameters: printer resource (required)
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_RESOURCE(printer_res)
    ZEND_PARSE_PARAMETERS_END();

    // Fetch the printer resource
    resource = zend_fetch_resource(Z_RES_P(printer_res), "Printer Handle", le_printer);
    if (!resource) {
        php_error_docref(NULL, E_WARNING, "Invalid printer resource");
        return;
    }

    // Abort the print job
    if (!AbortPrinter(resource->handle)) {
        php_error_docref(NULL, E_WARNING, "Failed to abort print job: %d", GetLastError());
    }
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

static void printer_dtor(zend_resource *rsrc)
{
    printer *p = (printer *)rsrc->ptr;

    ClosePrinter(p->handle);
    if (p->gdi_info.lpszDocName) {
        efree((char *)p->gdi_info.lpszDocName);
    }
    if (p->spooler_info.pOutputFile) { // Updated from info.lpszOutput to spooler_info.pOutputFile
        efree((char *)p->spooler_info.pOutputFile);
    }
    if (p->spooler_info.pDatatype) {   // Updated from info.lpszDatatype to spooler_info.pDatatype
        efree((char *)p->spooler_info.pDatatype);
    }
    if (p->pi2) {
        if (p->pi2->pDevMode) {
            efree((char *)p->pi2->pDevMode);
        }
        efree((char *)p->pi2);
    }
    if (p->dc) {
        DeleteDC(p->dc); // Clean up DC if it exists
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