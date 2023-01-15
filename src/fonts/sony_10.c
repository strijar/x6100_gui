/*******************************************************************************
 * Size: 10 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef SONY_10
#define SONY_10 1
#endif

#if SONY_10

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x5a, 0x5a, 0x5a, 0x5a, 0x49,

    /* U+0022 "\"" */
    0x67, 0x30, 0x0,

    /* U+0023 "#" */
    0x0, 0xd0, 0xd0, 0x2b, 0xda, 0xe8, 0x5, 0x83,
    0xa0, 0x5c, 0xbb, 0xc5, 0xb, 0x2a, 0x30,

    /* U+0024 "$" */
    0x0, 0x50, 0x6, 0xed, 0xc8, 0xa5, 0x50, 0x4,
    0xdd, 0xd5, 0x0, 0x56, 0xa7, 0xcd, 0xd6, 0x0,
    0x10, 0x0,

    /* U+0025 "%" */
    0x99, 0xb4, 0x8, 0x50, 0xb, 0x16, 0x65, 0x80,
    0x0, 0x69, 0x95, 0xb6, 0x99, 0x30, 0x0, 0xb1,
    0xb1, 0x56, 0x0, 0x94, 0x9, 0x9b, 0x40,

    /* U+0026 "&" */
    0x1d, 0xbd, 0x30, 0x1, 0xd2, 0xb4, 0x0, 0x2d,
    0xf6, 0x53, 0x9, 0x52, 0xcd, 0x50, 0x5d, 0xcd,
    0xe7, 0x0,

    /* U+0027 "'" */
    0x70, 0x20,

    /* U+0028 "(" */
    0x7, 0x4, 0xb0, 0x69, 0x6, 0x90, 0x69, 0x6,
    0x90, 0x59, 0x2, 0xd1,

    /* U+0029 ")" */
    0x34, 0x0, 0xe0, 0xe, 0x10, 0xe1, 0xe, 0x10,
    0xe1, 0xe, 0x4, 0xc0,

    /* U+002A "*" */
    0x5, 0x30, 0x4b, 0xb3, 0x9, 0x90, 0x0, 0x0,

    /* U+002B "+" */
    0x0, 0xd0, 0x6, 0x9e, 0x96, 0x0, 0xd0, 0x0,
    0x0, 0x0,

    /* U+002C "," */
    0x50, 0x70,

    /* U+002D "-" */
    0x88, 0x60,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x0, 0xa, 0x30, 0x5, 0x70, 0x1, 0xb0, 0x0,
    0xa2, 0x0, 0x67, 0x0, 0x0,

    /* U+0030 "0" */
    0x2d, 0xcc, 0xd1, 0x69, 0x0, 0xb3, 0x69, 0x0,
    0xb4, 0x69, 0x0, 0xb3, 0x2d, 0xcc, 0xd1,

    /* U+0031 "1" */
    0x1c, 0xf0, 0xf, 0x0, 0xf0, 0xf, 0x0, 0xf0,

    /* U+0032 "2" */
    0x2c, 0xcc, 0xc0, 0x0, 0x0, 0xe1, 0xb, 0xcd,
    0xa0, 0x3c, 0x0, 0x0, 0x4f, 0xcc, 0xc0,

    /* U+0033 "3" */
    0x3c, 0xcd, 0xb0, 0x0, 0x0, 0xf0, 0x0, 0xad,
    0xb0, 0x0, 0x0, 0xf0, 0x3c, 0xcc, 0xb0,

    /* U+0034 "4" */
    0x0, 0xd3, 0xf0, 0x79, 0xf, 0x2d, 0x0, 0xf5,
    0xdc, 0xcf, 0x0, 0x0, 0xf0,

    /* U+0035 "5" */
    0x4e, 0xcc, 0xc0, 0x4b, 0x0, 0x0, 0x3d, 0xcd,
    0xa0, 0x0, 0x0, 0xf0, 0x3c, 0xcd, 0xb0,

    /* U+0036 "6" */
    0x3e, 0xcc, 0xc1, 0x78, 0x0, 0x0, 0x7e, 0xcc,
    0xc1, 0x78, 0x0, 0xb4, 0x3d, 0xcc, 0xd2,

    /* U+0037 "7" */
    0x4c, 0xcc, 0xf0, 0x0, 0x6, 0x90, 0x0, 0xd,
    0x10, 0x0, 0x88, 0x0, 0x1, 0xd0, 0x0,

    /* U+0038 "8" */
    0x4e, 0xcc, 0xd1, 0x78, 0x0, 0xc3, 0x2f, 0xcd,
    0xd0, 0x78, 0x0, 0xc3, 0x4d, 0xcc, 0xd1,

    /* U+0039 "9" */
    0x4d, 0xcc, 0xd1, 0x87, 0x0, 0xb4, 0x3d, 0xcc,
    0xf4, 0x0, 0x0, 0xb4, 0x3c, 0xcc, 0xd1,

    /* U+003A ":" */
    0x51, 0x0, 0x51,

    /* U+003B ";" */
    0x51, 0x0, 0x21, 0x41,

    /* U+003C "<" */
    0x0, 0x2, 0x41, 0x79, 0x72, 0x6b, 0x50, 0x0,
    0x4, 0x96, 0x0, 0x0, 0x0,

    /* U+003D "=" */
    0x6b, 0xbb, 0x75, 0x99, 0x96,

    /* U+003E ">" */
    0x52, 0x0, 0x1, 0x79, 0x71, 0x0, 0x5b, 0x66,
    0x94, 0x0, 0x0, 0x0, 0x0,

    /* U+003F "?" */
    0x9c, 0xce, 0x40, 0x0, 0x78, 0x5, 0xbc, 0x40,
    0x84, 0x0, 0x7, 0x40, 0x0,

    /* U+0040 "@" */
    0x4, 0x77, 0x67, 0x15, 0x67, 0x8a, 0x69, 0x90,
    0xb0, 0x82, 0x95, 0x55, 0x66, 0x73, 0x5, 0x76,
    0x77, 0x0,

    /* U+0041 "A" */
    0x0, 0xc, 0xa0, 0x0, 0x5, 0x9d, 0x30, 0x0,
    0xd2, 0x5a, 0x0, 0x4e, 0xbb, 0xf2, 0xc, 0x40,
    0x7, 0x90,

    /* U+0042 "B" */
    0x7e, 0xcc, 0xd1, 0x78, 0x0, 0xb4, 0x7e, 0xcc,
    0xe1, 0x78, 0x0, 0xb4, 0x7e, 0xcc, 0xd1,

    /* U+0043 "C" */
    0x4e, 0xcc, 0xa7, 0x80, 0x0, 0x78, 0x0, 0x7,
    0x80, 0x0, 0x3d, 0xcc, 0xa0,

    /* U+0044 "D" */
    0x7e, 0xcc, 0xd2, 0x78, 0x0, 0x95, 0x78, 0x0,
    0x95, 0x78, 0x0, 0xa5, 0x7e, 0xcc, 0xd2,

    /* U+0045 "E" */
    0x3d, 0xcc, 0xa7, 0x80, 0x0, 0x7e, 0xcc, 0x87,
    0x80, 0x0, 0x3d, 0xcc, 0xa0,

    /* U+0046 "F" */
    0x3e, 0xcc, 0xa7, 0x80, 0x0, 0x7e, 0xcc, 0xa7,
    0x80, 0x0, 0x78, 0x0, 0x0,

    /* U+0047 "G" */
    0x3d, 0xcc, 0xb0, 0x78, 0x0, 0x0, 0x78, 0x0,
    0x83, 0x78, 0x0, 0xb4, 0x3d, 0xcc, 0xd1,

    /* U+0048 "H" */
    0x78, 0x0, 0x96, 0x78, 0x0, 0x96, 0x7e, 0xcc,
    0xe6, 0x78, 0x0, 0x96, 0x78, 0x0, 0x96,

    /* U+0049 "I" */
    0x78, 0x78, 0x78, 0x78, 0x78,

    /* U+004A "J" */
    0xa, 0x50, 0xa5, 0xa, 0x50, 0xa4, 0xbd, 0x10,

    /* U+004B "K" */
    0x78, 0x9, 0x90, 0x78, 0x99, 0x0, 0x7c, 0xd0,
    0x0, 0x78, 0x8a, 0x0, 0x78, 0x7, 0xa0,

    /* U+004C "L" */
    0x78, 0x0, 0x7, 0x80, 0x0, 0x78, 0x0, 0x7,
    0x80, 0x0, 0x3d, 0xcc, 0xa0,

    /* U+004D "M" */
    0x5e, 0x20, 0x6, 0xe1, 0x7c, 0xa0, 0xd, 0xd3,
    0x77, 0xd2, 0x69, 0xb3, 0x77, 0x5a, 0xd1, 0xb3,
    0x77, 0xc, 0x80, 0xb3,

    /* U+004E "N" */
    0x5e, 0x20, 0x95, 0x7a, 0xc0, 0x95, 0x77, 0x86,
    0x95, 0x77, 0xd, 0xb5, 0x77, 0x3, 0xe3,

    /* U+004F "O" */
    0x3d, 0xcd, 0xc0, 0x78, 0x0, 0xc3, 0x78, 0x0,
    0xc3, 0x78, 0x0, 0xc3, 0x3d, 0xcc, 0xc0,

    /* U+0050 "P" */
    0x7e, 0xcc, 0xd2, 0x78, 0x0, 0xb4, 0x7e, 0xcc,
    0xc1, 0x78, 0x0, 0x0, 0x78, 0x0, 0x0,

    /* U+0051 "Q" */
    0x3e, 0xcc, 0xd1, 0x78, 0x0, 0xb4, 0x78, 0x0,
    0xb4, 0x78, 0x0, 0xb4, 0x3d, 0xcc, 0xd1, 0x0,
    0x9, 0xb0,

    /* U+0052 "R" */
    0x7e, 0xcc, 0xd1, 0x78, 0x0, 0xb4, 0x7c, 0xca,
    0xb1, 0x78, 0xa8, 0x0, 0x78, 0x8, 0xa0,

    /* U+0053 "S" */
    0x5e, 0xcc, 0x89, 0x60, 0x0, 0x4c, 0xcc, 0x50,
    0x0, 0x5a, 0x7c, 0xcd, 0x60,

    /* U+0054 "T" */
    0xbd, 0xec, 0x40, 0x4b, 0x0, 0x4, 0xb0, 0x0,
    0x4b, 0x0, 0x4, 0xb0, 0x0,

    /* U+0055 "U" */
    0x78, 0x0, 0xa5, 0x78, 0x0, 0xa5, 0x78, 0x0,
    0xa5, 0x78, 0x0, 0xa5, 0x3d, 0xcc, 0xd1,

    /* U+0056 "V" */
    0xd, 0x20, 0x8, 0x70, 0x5a, 0x0, 0xe0, 0x0,
    0xd2, 0x78, 0x0, 0x6, 0x9d, 0x10, 0x0, 0xd,
    0x70, 0x0,

    /* U+0057 "W" */
    0xb3, 0xb, 0x80, 0x69, 0x78, 0x1c, 0xd0, 0xa4,
    0x2c, 0x67, 0xb3, 0xe0, 0xd, 0xc3, 0x6c, 0xa0,
    0x7, 0xd0, 0x1e, 0x40,

    /* U+0058 "X" */
    0x7a, 0x3, 0xd1, 0x9, 0x9d, 0x20, 0x0, 0xf8,
    0x0, 0xa, 0x7d, 0x30, 0x99, 0x2, 0xd2,

    /* U+0059 "Y" */
    0xb, 0x60, 0x2d, 0x10, 0x1d, 0x3d, 0x30, 0x0,
    0x3f, 0x70, 0x0, 0x0, 0xd1, 0x0, 0x0, 0xd,
    0x10, 0x0,

    /* U+005A "Z" */
    0x8c, 0xce, 0x90, 0x3, 0xd1, 0x2, 0xd2, 0x1,
    0xd3, 0x0, 0x9e, 0xcc, 0x80,

    /* U+005B "[" */
    0x3b, 0x14, 0x90, 0x49, 0x4, 0x90, 0x49, 0x4,
    0x90, 0x49, 0x3, 0xb1,

    /* U+005C "\\" */
    0x92, 0x0, 0x1a, 0x0, 0x6, 0x50, 0x0, 0xb1,
    0x0, 0x29,

    /* U+005D "]" */
    0x4b, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x5b,

    /* U+005E "^" */
    0x6, 0x20, 0x63, 0x80,

    /* U+005F "_" */
    0x99, 0x99, 0x90,

    /* U+0060 "`" */
    0x80, 0x0,

    /* U+0061 "a" */
    0x4b, 0xbc, 0x72, 0xba, 0xbb, 0x76, 0x2, 0xb4,
    0xcb, 0xc7,

    /* U+0062 "b" */
    0x76, 0x0, 0x7, 0xda, 0xc7, 0x76, 0x2, 0xb7,
    0x60, 0x2b, 0x4c, 0xbc, 0x70,

    /* U+0063 "c" */
    0x4d, 0xb9, 0x76, 0x0, 0x76, 0x0, 0x4d, 0xb9,

    /* U+0064 "d" */
    0x0, 0x2, 0xb4, 0xdb, 0xcb, 0x76, 0x2, 0xb7,
    0x60, 0x2b, 0x4d, 0xbc, 0x70,

    /* U+0065 "e" */
    0x4d, 0xac, 0x77, 0x76, 0xa7, 0x7b, 0x40, 0x3,
    0xdb, 0xb7,

    /* U+0066 "f" */
    0x4b, 0x47, 0xc5, 0x76, 0x7, 0x60, 0x76, 0x0,

    /* U+0067 "g" */
    0x3d, 0xbc, 0x77, 0x60, 0x2b, 0x76, 0x2, 0xb4,
    0xdb, 0xcb, 0x0, 0x2, 0xb5, 0xbb, 0xc6,

    /* U+0068 "h" */
    0x76, 0x0, 0x7, 0xed, 0xe6, 0x76, 0x3, 0xa7,
    0x60, 0x2a, 0x76, 0x2, 0xa0,

    /* U+0069 "i" */
    0x62, 0x94, 0x94, 0x94, 0x94,

    /* U+006A "j" */
    0x4, 0x40, 0x76, 0x7, 0x60, 0x76, 0x7, 0x60,
    0x76, 0x6d, 0x20,

    /* U+006B "k" */
    0x76, 0x0, 0x7, 0x63, 0xb1, 0x7a, 0xb0, 0x7,
    0x9c, 0x10, 0x76, 0x2c, 0x10,

    /* U+006C "l" */
    0x76, 0x76, 0x76, 0x76, 0x76,

    /* U+006D "m" */
    0x3e, 0xdb, 0xde, 0x57, 0x60, 0xe0, 0x49, 0x76,
    0xd, 0x4, 0x97, 0x60, 0xd0, 0x49,

    /* U+006E "n" */
    0x6e, 0xde, 0x3a, 0x30, 0x76, 0xb2, 0x6, 0x7b,
    0x20, 0x67,

    /* U+006F "o" */
    0x5d, 0xad, 0x59, 0x40, 0x49, 0x94, 0x4, 0x95,
    0xdb, 0xd5,

    /* U+0070 "p" */
    0x4d, 0xac, 0x77, 0x60, 0x2a, 0x76, 0x2, 0xb7,
    0xda, 0xc7, 0x76, 0x0, 0x7, 0x60, 0x0,

    /* U+0071 "q" */
    0x4d, 0xac, 0x67, 0x60, 0x2a, 0x76, 0x2, 0xb4,
    0xda, 0xbb, 0x0, 0x2, 0xb0, 0x0, 0x2b,

    /* U+0072 "r" */
    0x7c, 0x7b, 0x20, 0xb2, 0xb, 0x20,

    /* U+0073 "s" */
    0x7c, 0xbb, 0x15, 0xbb, 0xa0, 0x0, 0xc, 0x26,
    0xbb, 0xd0,

    /* U+0074 "t" */
    0x76, 0x7, 0xc5, 0x76, 0x7, 0x60, 0x76, 0x0,

    /* U+0075 "u" */
    0x76, 0x2, 0xb7, 0x60, 0x2b, 0x77, 0x3, 0xb3,
    0xed, 0xe6,

    /* U+0076 "v" */
    0xc, 0x10, 0x2b, 0x4, 0x90, 0xb3, 0x0, 0xb6,
    0xa0, 0x0, 0x3e, 0x20,

    /* U+0077 "w" */
    0xb2, 0x3d, 0x6, 0x66, 0x79, 0xa4, 0xb1, 0x1c,
    0xc3, 0xac, 0x0, 0xb8, 0xc, 0x60,

    /* U+0078 "x" */
    0x69, 0x2c, 0x10, 0x9e, 0x30, 0xa, 0xd4, 0x8,
    0x81, 0xc2,

    /* U+0079 "y" */
    0x76, 0x2, 0xb7, 0x60, 0x2b, 0x76, 0x2, 0xb3,
    0xed, 0xdb, 0x0, 0x3, 0xa4, 0xee, 0xe5,

    /* U+007A "z" */
    0x7b, 0xbe, 0x70, 0x8, 0xa0, 0xb, 0x70, 0x9,
    0xeb, 0xb5,

    /* U+007B "{" */
    0xa, 0x53, 0xa0, 0x39, 0x8, 0x70, 0x86, 0x3,
    0x90, 0x3a, 0x0, 0xa6,

    /* U+007C "|" */
    0x4d, 0xdd, 0xdd, 0xd0,

    /* U+007D "}" */
    0x88, 0x0, 0xd0, 0xd, 0x0, 0xa4, 0xa, 0x50,
    0xd0, 0xd, 0x8, 0x80,

    /* U+007E "~" */
    0x9, 0xb7, 0x38, 0x14, 0x30, 0x7c, 0x60
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 47, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 39, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 45, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 8, .adv_w = 103, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 23, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 41, .adv_w = 142, .box_w = 9, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 96, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 28, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 84, .adv_w = 44, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 96, .adv_w = 44, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 108, .adv_w = 63, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 116, .adv_w = 80, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 126, .adv_w = 26, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 128, .adv_w = 43, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 130, .adv_w = 27, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 131, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 144, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 94, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 94, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 225, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 285, .adv_w = 28, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 288, .adv_w = 27, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 292, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 305, .adv_w = 80, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 310, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 323, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 117, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 354, .adv_w = 93, .box_w = 7, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 372, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 387, .adv_w = 87, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 428, .adv_w = 89, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 441, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 95, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 471, .adv_w = 32, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 476, .adv_w = 45, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 484, .adv_w = 82, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 499, .adv_w = 79, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 124, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 532, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 547, .adv_w = 92, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 562, .adv_w = 93, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 577, .adv_w = 93, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 595, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 610, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 623, .adv_w = 70, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 636, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 651, .adv_w = 90, .box_w = 7, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 669, .adv_w = 125, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 88, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 704, .adv_w = 84, .box_w = 7, .box_h = 5, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 722, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 735, .adv_w = 43, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 747, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 757, .adv_w = 43, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 765, .adv_w = 57, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 769, .adv_w = 80, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 772, .adv_w = 50, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 774, .adv_w = 84, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 784, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 797, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 805, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 818, .adv_w = 81, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 828, .adv_w = 48, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 836, .adv_w = 84, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 851, .adv_w = 83, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 864, .adv_w = 31, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 869, .adv_w = 31, .box_w = 3, .box_h = 7, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 880, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 893, .adv_w = 31, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 898, .adv_w = 114, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 912, .adv_w = 79, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 922, .adv_w = 83, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 932, .adv_w = 84, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 947, .adv_w = 84, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 962, .adv_w = 49, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 968, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 978, .adv_w = 47, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 986, .adv_w = 84, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 996, .adv_w = 79, .box_w = 6, .box_h = 4, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1008, .adv_w = 107, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1022, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1032, .adv_w = 83, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1047, .adv_w = 78, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1057, .adv_w = 45, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1069, .adv_w = 80, .box_w = 1, .box_h = 7, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 1073, .adv_w = 45, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1085, .adv_w = 107, .box_w = 7, .box_h = 2, .ofs_x = 0, .ofs_y = 1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    14, 53,
    14, 55,
    14, 56,
    14, 57,
    14, 58,
    34, 14,
    34, 53,
    34, 55,
    34, 56,
    34, 58,
    34, 85,
    34, 87,
    34, 88,
    34, 90,
    36, 14,
    37, 13,
    37, 15,
    39, 13,
    39, 15,
    39, 34,
    39, 66,
    45, 53,
    45, 55,
    45, 56,
    45, 58,
    45, 87,
    45, 88,
    45, 90,
    48, 13,
    49, 13,
    49, 14,
    49, 15,
    49, 34,
    49, 66,
    49, 68,
    49, 69,
    49, 70,
    49, 72,
    49, 80,
    51, 14,
    51, 68,
    51, 69,
    51, 70,
    51, 80,
    51, 82,
    53, 13,
    53, 14,
    53, 15,
    53, 34,
    53, 66,
    53, 68,
    53, 69,
    53, 70,
    53, 72,
    53, 78,
    53, 79,
    53, 80,
    53, 81,
    53, 82,
    53, 83,
    53, 84,
    53, 86,
    53, 87,
    53, 88,
    53, 89,
    53, 90,
    53, 91,
    55, 14,
    55, 15,
    55, 34,
    55, 66,
    55, 68,
    55, 69,
    55, 70,
    55, 72,
    55, 78,
    55, 79,
    55, 80,
    55, 81,
    55, 82,
    55, 83,
    55, 84,
    55, 86,
    56, 14,
    56, 15,
    56, 34,
    56, 66,
    56, 68,
    56, 69,
    56, 70,
    56, 80,
    56, 82,
    57, 14,
    57, 70,
    57, 80,
    57, 82,
    57, 90,
    58, 14,
    58, 34,
    58, 66,
    58, 68,
    58, 69,
    58, 70,
    58, 72,
    58, 78,
    58, 79,
    58, 80,
    58, 81,
    58, 82,
    58, 83,
    58, 84,
    58, 86,
    71, 13,
    71, 14,
    71, 15,
    76, 14,
    83, 13,
    83, 14,
    83, 15,
    85, 13,
    85, 14,
    85, 15,
    87, 13,
    87, 15,
    88, 13,
    88, 15,
    89, 14,
    90, 13,
    90, 15
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -16, -8, -5, -3, -10, -3, -8, -6,
    -3, -8, -3, -5, -3, -5, -3, -5,
    -5, -8, -8, -5, -3, -10, -10, -8,
    -10, -3, -3, -3, -5, -16, -3, -16,
    -5, -3, -3, -3, -3, -2, -3, -6,
    -3, -3, -3, -3, -3, -14, -16, -14,
    -8, -13, -13, -13, -13, -13, -10, -10,
    -13, -10, -13, -10, -13, -10, -13, -13,
    -10, -11, -10, -8, -13, -6, -6, -6,
    -6, -6, -6, -3, -3, -6, -3, -6,
    -3, -5, -3, -3, -8, -2, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3,
    -5, -10, -8, -10, -6, -6, -6, -6,
    -3, -3, -6, -3, -6, -3, -5, -3,
    -6, -6, -6, -6, -10, -5, -10, 3,
    -5, 3, -8, -8, -5, -5, -6, -8,
    -8
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 129,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t sony_10 = {
#else
lv_font_t sony_10 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if SONY_10*/

