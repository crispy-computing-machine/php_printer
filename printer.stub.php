<?php 
const PRINTER_COPIES = 0;
const PRINTER_MODE = 1;
const PRINTER_TITLE = 2;
const PRINTER_DEVICENAME = 13;
const PRINTER_DRIVERVERSION = 14;
const PRINTER_OUTPUT_FILE = 15;
const PRINTER_RESOLUTION_Y = 4;
const PRINTER_RESOLUTION_X = 5;
const PRINTER_SCALE = 9;
const PRINTER_BACKGROUND_COLOR = 10;
const PRINTER_PAPER_LENGTH = 7;
const PRINTER_PAPER_WIDTH = 8;
const PRINTER_PAPER_FORMAT = 6;
const PRINTER_FORMAT_CUSTOM = 0;
const PRINTER_FORMAT_LETTER = 1;
const PRINTER_FORMAT_LEGAL = 5;
const PRINTER_FORMAT_A3 = 8;
const PRINTER_FORMAT_A4 = 9;
const PRINTER_FORMAT_A5 = 11;
const PRINTER_FORMAT_B4 = 12;
const PRINTER_FORMAT_B5 = 13;
const PRINTER_FORMAT_FOLIO = 14;
const PRINTER_ORIENTATION = 3;
const PRINTER_ORIENTATION_PORTRAIT = 1;
const PRINTER_ORIENTATION_LANDSCAPE = 2;
const PRINTER_TEXT_COLOR = 11;
const PRINTER_TEXT_ALIGN = 12;
const PRINTER_TA_BASELINE = 24;
const PRINTER_TA_BOTTOM = 8;
const PRINTER_TA_TOP = 0;
const PRINTER_TA_CENTER = 6;
const PRINTER_TA_LEFT = 0;
const PRINTER_TA_RIGHT = 2;
const PRINTER_PEN_SOLID = 0;
const PRINTER_PEN_DASH = 1;
const PRINTER_PEN_DOT = 2;
const PRINTER_PEN_DASHDOT = 3;
const PRINTER_PEN_DASHDOTDOT = 4;
const PRINTER_PEN_INVISIBLE = 5;
const PRINTER_BRUSH_SOLID = -1;
const PRINTER_BRUSH_CUSTOM = -2;
const PRINTER_BRUSH_DIAGONAL = 3;
const PRINTER_BRUSH_CROSS = 4;
const PRINTER_BRUSH_DIAGCROSS = 5;
const PRINTER_BRUSH_FDIAGONAL = 2;
const PRINTER_BRUSH_HORIZONTAL = 0;
const PRINTER_BRUSH_VERTICAL = 1;
const PRINTER_FW_THIN = 100;
const PRINTER_FW_ULTRALIGHT = 200;
const PRINTER_FW_LIGHT = 300;
const PRINTER_FW_NORMAL = 400;
const PRINTER_FW_MEDIUM = 500;
const PRINTER_FW_BOLD = 700;
const PRINTER_FW_ULTRABOLD = 800;
const PRINTER_FW_HEAVY = 900;
const PRINTER_ENUM_LOCAL = 2;
const PRINTER_ENUM_NAME = 8;
const PRINTER_ENUM_SHARED = 32;
const PRINTER_ENUM_DEFAULT = 1;
const PRINTER_ENUM_CONNECTIONS = 4;
const PRINTER_ENUM_NETWORK = 64;
const PRINTER_ENUM_REMOTE = 16;

/**
 * Open a connection to a printer
 *
 * @param string|null $printername
 * @return resource
 */
function printer_open(?string $printername){}

/**
 * Close a connection to a printer
 *
 * @param $connection
 * @return bool
 */
function printer_close($connection) : bool{}

/**
 * Write data to a printer
 *
 * @param $connection
 * @param string $content
 * @return bool
 */
function printer_write($connection, string $content) : bool{}

/**
 * List all printers and metadata
 *
 * @param int $enum_type
 * @param string|null $name
 * @param int|null $level
 * @return array
 */
function printer_list(int $enum_type, ?string $name, ?int $level) : array{}

/**
 * Set an option on a printer. See constants.
 *
 * @param $connection
 * @param int $option
 * @param $value
 * @return bool
 */
function printer_set_option($connection, int $option, $value) : bool{}

/**
 * Get options values previously set with printer_set_option
 * @param $connection
 * @param int $option
 * @return void
 */
function printer_get_option($connection, int $option){}

/**
 * Create a device context to start drawing with GDI
 * @param $connection
 * @return void
 */
function printer_create_dc($connection) : void{}

/**
 * Delete a device context to end drawing with GDI
 * @param $connection
 * @return bool
 */
function printer_delete_dc($connection) : bool{}

/**
 * Start a new document
 *
 * @param $connection
 * @param string|null $doc_name
 * @return bool
 */
function printer_start_doc($connection, ?string $doc_name) : bool{}

/**
 * End a document
 *
 * @param $connection
 * @return bool
 */
function printer_end_doc($connection) : bool{}

/**
 * Start a page within a document
 *
 * @param $connection
 * @return bool
 */
function printer_start_page($connection) : bool{}

/**
 * End page within document
 *
 * @param $connection
 * @return bool
 */
function printer_end_page($connection) : bool{}

/**
 * Create a pen for drawing with GDI
 *
 * @param int $style
 * @param int $width
 * @param string $color
 * @return void
 */
function printer_create_pen(int $style, int $width, string $color){}

/**
 * Delete pen
 *
 * @param $pen
 * @return bool
 */
function printer_delete_pen($pen) : bool{}

/**
 * Select a pen to use for drawing with GDI
 *
 * @param $connection
 * @param $pen
 * @return bool
 */
function printer_select_pen($connection, $pen) : bool{}

/**
 * Create a brish for drwing with GDI
 * @param int $style
 * @param string $param
 * @return void
 */
function printer_create_brush(int $style, string $param){}

/**
 * Delete brush
 * @param $brush
 * @return bool
 */
function printer_delete_brush($brush) : bool{}

/**
 * Select brush to use for drawing with GDI
 *
 * @param $connection
 * @param $brush
 * @return bool
 */
function printer_select_brush($connection, $brush) : bool{}

/**
 * Create a font for drawing text with GDI
 *
 * @param string $face
 * @param int $height
 * @param int $width
 * @param int $weight
 * @param bool $italic
 * @param bool $underline
 * @param bool $strikeout
 * @param int $orientation
 * @return void
 */
function printer_create_font(string $face, int $height, int $width, int $weight, bool $italic, bool $underline, bool $strikeout, int $orientation){}

/**
 * Delete font
 *
 * @param $font
 * @return bool
 */
function printer_delete_font($font) : bool{}

/**
 * Select font for drawing with GDI
 *
 * @param $connection
 * @param $font
 * @return bool
 */
function printer_select_font($connection, $font) : bool{}

/**
 * Calculate font height
 *
 * @param $connection
 * @param int $height
 * @return int|null
 */
function printer_logical_fontheight($connection, int $height) : ?int{}

/**
 * Draw rounded rectangle
 *
 * @param $connection
 * @param int $ul_x
 * @param int $ul_y
 * @param int $lr_x
 * @param int $lr_y
 * @param int $width
 * @param int $height
 * @return bool
 */
function printer_draw_roundrect($connection, int $ul_x, int $ul_y, int $lr_x, int $lr_y, int $width, int $height) : bool{}

/**
 * Draw rectangle
 *
 * @param $connection
 * @param int $ul_x
 * @param int $ul_y
 * @param int $lr_x
 * @param int $lr_y
 * @return bool
 */
function printer_draw_rectangle($connection, int $ul_x, int $ul_y, int $lr_x, int $lr_y) : bool{}

/**
 * Draw text
 *
 * @param $connection
 * @param string $text
 * @param int $x
 * @param int $y
 * @return bool
 */
function printer_draw_text($connection, string $text, int $x, int $y) : bool{}

/**
 * Draw ellipse
 *
 * @param $connection
 * @param int $ul_x
 * @param int $ul_y
 * @param int $lr_x
 * @param int $lr_y
 * @return bool
 */
function printer_draw_ellipse($connection, int $ul_x, int $ul_y, int $lr_x, int $lr_y) : bool{}

/**
 * Draw line
 *
 * @param $connection
 * @param int $fx
 * @param int $fy
 * @param int $tx
 * @param int $ty
 * @return bool
 */
function printer_draw_line($connection, int $fx, int $fy, int $tx, int $ty) : bool{}

/**
 * Draw chord
 *
 * @param $connection
 * @param int $rec_x
 * @param int $rec_y
 * @param int $rec_x1
 * @param int $rec_y1
 * @param int $rad_x
 * @param int $rad_y
 * @param int $rad_x1
 * @param int $rad_y1
 * @return bool
 */
function printer_draw_chord($connection, int $rec_x, int $rec_y, int $rec_x1, int $rec_y1, int $rad_x, int $rad_y, int $rad_x1, int $rad_y1) : bool{}

/**
 * Draw pie
 *
 * @param $connection
 * @param int $rec_x
 * @param int $rec_y
 * @param int $rec_x1
 * @param int $rec_y1
 * @param int $rad1_x
 * @param int $rad1_y
 * @param int $rad2_x
 * @param int $rad2_y
 * @return bool
 */
function printer_draw_pie($connection, int $rec_x, int $rec_y, int $rec_x1, int $rec_y1, int $rad1_x, int $rad1_y, int $rad2_x, int $rad2_y) : bool{}

/**
 * Draw BMP
 *
 * @param $connection
 * @param string $filename
 * @param int $x
 * @param int $y
 * @param int|null $width
 * @param int|null $height
 * @return bool
 */
function printer_draw_bmp($connection, string $filename, int $x, int $y, ?int $width, ?int $height) : bool{}

/**
 * Abort print job
 *
 * @param $connection
 * @return bool
 */
function printer_abort($connection) : bool{}