// Arginfo for printer_open: optional string printername, returns resource or false
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_open, 0, 0, IS_RESOURCE, 1)
    ZEND_ARG_TYPE_INFO(0, printername, IS_STRING, 1) // Optional (nullable)
ZEND_END_ARG_INFO()

// Arginfo for printer_close: resource connection, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_close, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

// Arginfo for printer_write: resource connection, string content, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_write, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_list: int enum_type, optional string name, optional int level, returns array
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_list, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, enum_type, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 1) // Optional (nullable)
    ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 1)  // Optional (nullable)
ZEND_END_ARG_INFO()

// Arginfo for printer_set_option: resource connection, int option, mixed value, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_set_option, 0, 3, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0) // Enum-like value
    ZEND_ARG_INFO(0, value)                   // Mixed type (string, int, etc.)
ZEND_END_ARG_INFO()

// Arginfo for printer_get_option: resource connection, int option, returns mixed
ZEND_BEGIN_ARG_INFO_EX(arginfo_printer_get_option, 0, 2, 2)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_create_dc: resource connection, returns void
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_create_dc, 0, 1, IS_VOID, 0)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

// Arginfo for printer_delete_dc: resource connection, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_dc, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

// Arginfo for printer_start_doc: resource connection, optional string doc_name, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_start_doc, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, doc_name, IS_STRING, 1) // Optional (nullable)
ZEND_END_ARG_INFO()

// Arginfo for printer_end_doc: resource connection, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_end_doc, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

// Arginfo for printer_start_page: resource connection, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_start_page, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

// Arginfo for printer_end_page: resource connection, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_end_page, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()

// Arginfo for printer_create_pen: int style, int width, string color, returns resource or false
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_create_pen, 0, 3, IS_RESOURCE, 1)
    ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, color, IS_STRING, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_delete_pen: resource pen_handle, returns void
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_pen, 0, 1, IS_VOID, 0)
    ZEND_ARG_INFO(0, pen_handle)
ZEND_END_ARG_INFO()

// Arginfo for printer_select_pen: resource printer_handle, resource pen_handle, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_select_pen, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, printer_handle)
    ZEND_ARG_INFO(0, pen_handle)
ZEND_END_ARG_INFO()

// Arginfo for printer_create_brush: int style, string param, returns resource or false
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_create_brush, 0, 2, IS_RESOURCE, 1)
    ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, param, IS_STRING, 0) // Color or file path
ZEND_END_ARG_INFO()

// Arginfo for printer_delete_brush: resource brush_handle, returns void
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_brush, 0, 1, IS_VOID, 0)
    ZEND_ARG_INFO(0, brush_handle)
ZEND_END_ARG_INFO()

// Arginfo for printer_select_brush: resource printer_handle, resource brush_handle, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_select_brush, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, printer_handle)
    ZEND_ARG_INFO(0, brush_handle)
ZEND_END_ARG_INFO()

// Arginfo for printer_create_font: string face, int height, int width, int font_weight, bool italic, bool underline, bool strikeout, int orientation, returns resource or false
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_create_font, 0, 8, IS_RESOURCE, 1)
    ZEND_ARG_TYPE_INFO(0, face, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, font_weight, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, italic, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, underline, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, strikeout, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, orientation, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_delete_font: resource font_handle, returns void
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_font, 0, 1, IS_VOID, 0)
    ZEND_ARG_INFO(0, font_handle)
ZEND_END_ARG_INFO()

// Arginfo for printer_select_font: resource printer_handle, resource font_handle, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_select_font, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, printer_handle)
    ZEND_ARG_INFO(0, font_handle)
ZEND_END_ARG_INFO()

// Arginfo for printer_logical_fontheight: resource connection, int height, returns long or false
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_logical_fontheight, 0, 2, IS_LONG, 1)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_roundrect: resource connection, int ul_x, int ul_y, int lr_x, int lr_y, int width, int height, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_roundrect, 0, 7, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, ul_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ul_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_rectangle: resource connection, int ul_x, int ul_y, int lr_x, int lr_y, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_rectangle, 0, 5, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, ul_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ul_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_y, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_ellipse: resource connection, int ul_x, int ul_y, int lr_x, int lr_y, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_elipse, 0, 5, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, ul_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ul_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_y, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_text: resource connection, string text, int x, int y, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_text, 0, 4, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_line: resource connection, int fx, int fy, int tx, int ty, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_line, 0, 5, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, fx, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, fy, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, tx, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ty, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_chord: resource connection, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad_x, int rad_y, int rad_x1, int rad_y1, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_chord, 0, 9, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, rec_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_x1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_x1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_y1, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_pie: resource connection, int rec_x, int rec_y, int rec_x1, int rec_y1, int rad1_x, int rad1_y, int rad2_x, int rad2_y, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_pie, 0, 9, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, rec_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_x1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad1_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad1_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad2_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad2_y, IS_LONG, 0)
ZEND_END_ARG_INFO()

// Arginfo for printer_draw_bmp: resource connection, string filename, int x, int y, optional int width, optional int height, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_bmp, 0, 4, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 1)  // Optional (nullable)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 1) // Optional (nullable)
ZEND_END_ARG_INFO()

// Arginfo for printer_abort: resource connection, returns bool
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_abort, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection)
ZEND_END_ARG_INFO()