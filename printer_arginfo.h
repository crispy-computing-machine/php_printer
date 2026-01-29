/* All IS_RESOURCE references are removed from param and return types, replaced with mixed. Non-resource return types (bool, array, int, void) are reimplemented with their specific types. */

/* @return mixed (was resource, now resource|false) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_printer_open, 0, 0, 0)
    ZEND_ARG_TYPE_INFO(0, printername, IS_STRING, 1) // Optional, nullable
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_close, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_write, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, content, IS_STRING, 0)
ZEND_END_ARG_INFO()

/* @return array */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_list, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, enum_type, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 1)
    ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 1)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_set_option, 0, 3, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
    ZEND_ARG_INFO(0, value) // Mixed
ZEND_END_ARG_INFO()

/* @return mixed */
ZEND_BEGIN_ARG_INFO_EX(arginfo_printer_get_option, 0, 0, 2)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return void */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_create_dc, 0, 1, IS_VOID, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_dc, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_start_doc, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, doc_name, IS_STRING, 1)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_end_doc, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_start_page, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_end_page, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return mixed (was resource, now resource|false) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_printer_create_pen, 0, 0, 3)
    ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, color, IS_STRING, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_pen, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, pen) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_select_pen, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_INFO(0, pen)        // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return mixed (was resource, now resource|false) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_printer_create_brush, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, style, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, param, IS_STRING, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_brush, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, brush) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_select_brush, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_INFO(0, brush)      // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return mixed (was resource, now resource|false) */
ZEND_BEGIN_ARG_INFO_EX(arginfo_printer_create_font, 0, 0, 8)
    ZEND_ARG_TYPE_INFO(0, face, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, weight, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, italic, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, underline, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, strikeout, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, orientation, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_delete_font, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, font) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_select_font, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_INFO(0, font)       // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()

/* @return int|false */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_logical_fontheight, 0, 2, IS_LONG, 1)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_roundrect, 0, 7, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, ul_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ul_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_rectangle, 0, 5, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, ul_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ul_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_y, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_ellipse, 0, 5, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, ul_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ul_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, lr_y, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_text, 0, 4, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, text, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_line, 0, 5, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, fx, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, fy, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, tx, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, ty, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_chord, 0, 9, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, rec_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_x1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_x1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad_y1, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_pie, 0, 9, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, rec_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_x1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rec_y1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad1_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad1_y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad2_x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, rad2_y, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_bmp, 0, 4, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
    ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 1)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 1)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_draw_image, 0, 4, IS_TRUE, 0)
    ZEND_ARG_TYPE_INFO(0, printer, IS_RESOURCE, 0)
    ZEND_ARG_OBJ_INFO(0, image, GdImage, 0)             // <-- GdImage object
    ZEND_ARG_TYPE_INFO(0, x, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, y, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, width, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, height, IS_LONG, 0)
ZEND_END_ARG_INFO()

/* @return bool */
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_printer_abort, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_INFO(0, connection) // Mixed (was IS_RESOURCE)
ZEND_END_ARG_INFO()