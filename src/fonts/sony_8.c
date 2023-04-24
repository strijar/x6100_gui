/*******************************************************************************
 * Size: 8 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef SONY_8
#define SONY_8 1
#endif

#if SONY_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x75, 0x75, 0x75, 0x53,

    /* U+0022 "\"" */
    0x56,

    /* U+0023 "#" */
    0x5, 0x5a, 0x2, 0xc7, 0xd5, 0x4d, 0x7c, 0x30,
    0xa5, 0x60,

    /* U+0024 "$" */
    0x8a, 0xa6, 0x6a, 0xb5, 0x2, 0x3b, 0x69, 0xb7,
    0x1, 0x10,

    /* U+0025 "%" */
    0x86, 0x80, 0x91, 0x9, 0x9, 0x79, 0x65, 0x66,
    0x97, 0x90, 0x90, 0x19, 0x8, 0x68,

    /* U+0026 "&" */
    0x3b, 0xa5, 0x2, 0xb9, 0x40, 0x87, 0xb9, 0x48,
    0x9a, 0xe5,

    /* U+0027 "'" */
    0x50,

    /* U+0028 "(" */
    0x15, 0x74, 0x84, 0x84, 0x84, 0x58,

    /* U+0029 ")" */
    0x42, 0x2a, 0x2a, 0x2a, 0x2a, 0x57,

    /* U+002A "*" */
    0x27, 0x22, 0xd3, 0x11, 0x20,

    /* U+002B "+" */
    0x5, 0x50, 0x5a, 0xa5, 0x1, 0x10,

    /* U+002C "," */
    0x6, 0x10,

    /* U+002D "-" */
    0x66, 0x0,

    /* U+002E "." */
    0x50,

    /* U+002F "/" */
    0x0, 0x64, 0x2, 0x80, 0x9, 0x0, 0x63, 0x0,

    /* U+0030 "0" */
    0x5b, 0xac, 0x18, 0x4a, 0xa3, 0x89, 0x69, 0x35,
    0xd9, 0xc1,

    /* U+0031 "1" */
    0x4d, 0x30, 0x93, 0x9, 0x30, 0x93,

    /* U+0032 "2" */
    0x39, 0x9b, 0x0, 0x0, 0xb1, 0x29, 0x99, 0x6,
    0xb9, 0x90,

    /* U+0033 "3" */
    0x49, 0x9c, 0x0, 0x29, 0xc0, 0x0, 0xc, 0x4,
    0x99, 0xb0,

    /* U+0034 "4" */
    0x6, 0x7c, 0x2b, 0xc, 0x5a, 0x9e, 0x0, 0xc,

    /* U+0035 "5" */
    0x6b, 0x99, 0x5, 0xa9, 0x90, 0x0, 0xc, 0x3,
    0x99, 0xb0,

    /* U+0036 "6" */
    0x6a, 0x99, 0x9, 0xb9, 0xa1, 0x93, 0x8, 0x36,
    0xb9, 0xb1,

    /* U+0037 "7" */
    0x49, 0x9e, 0x0, 0x4, 0x80, 0x0, 0xb0, 0x0,
    0x66, 0x0,

    /* U+0038 "8" */
    0x7a, 0x9c, 0x15, 0xb9, 0xd0, 0x93, 0x9, 0x26,
    0xb9, 0xc1,

    /* U+0039 "9" */
    0x6a, 0x9c, 0x19, 0x20, 0x93, 0x4a, 0x9d, 0x34,
    0x99, 0xc1,

    /* U+003A ":" */
    0x33,

    /* U+003B ";" */
    0x40, 0x50,

    /* U+003C "<" */
    0x0, 0x3, 0x47, 0x62, 0x37, 0x73, 0x0, 0x2,

    /* U+003D "=" */
    0x58, 0x86, 0x47, 0x75,

    /* U+003E ">" */
    0x30, 0x0, 0x16, 0x74, 0x37, 0x72, 0x20, 0x0,

    /* U+003F "?" */
    0x79, 0xb6, 0x0, 0x2a, 0x8, 0x94, 0xd, 0x0,

    /* U+0040 "@" */
    0x6, 0x55, 0x61, 0x74, 0x88, 0x37, 0x72, 0x56,
    0x51, 0x16, 0x55, 0x50,

    /* U+0041 "A" */
    0x0, 0x5d, 0x10, 0x0, 0xa4, 0x70, 0x4, 0xb8,
    0xc0, 0xb, 0x0, 0x57,

    /* U+0042 "B" */
    0x9a, 0x9c, 0x19, 0x30, 0x83, 0x9a, 0x9d, 0x19,
    0xa9, 0xc2,

    /* U+0043 "C" */
    0x6b, 0x98, 0x93, 0x0, 0x93, 0x0, 0x6b, 0x98,

    /* U+0044 "D" */
    0x9a, 0x9c, 0x29, 0x30, 0x74, 0x93, 0x7, 0x49,
    0xa9, 0xc2,

    /* U+0045 "E" */
    0x6b, 0x98, 0x93, 0x0, 0x9a, 0x96, 0x6a, 0x98,

    /* U+0046 "F" */
    0x6b, 0x98, 0x93, 0x0, 0x9a, 0x98, 0x93, 0x0,

    /* U+0047 "G" */
    0x6b, 0x98, 0x9, 0x30, 0x10, 0x93, 0x8, 0x36,
    0xb9, 0xc1,

    /* U+0048 "H" */
    0x93, 0x7, 0x59, 0x30, 0x75, 0x9a, 0x9c, 0x59,
    0x30, 0x75,

    /* U+0049 "I" */
    0x93, 0x93, 0x93, 0x93,

    /* U+004A "J" */
    0xc, 0xc, 0xc, 0x99,

    /* U+004B "K" */
    0x93, 0x58, 0x9, 0x98, 0x0, 0x98, 0x90, 0x9,
    0x35, 0x90,

    /* U+004C "L" */
    0x93, 0x0, 0x93, 0x0, 0x93, 0x0, 0x6b, 0x98,

    /* U+004D "M" */
    0x7c, 0x0, 0x8a, 0x9a, 0x41, 0xab, 0x93, 0xb9,
    0x3b, 0x92, 0x7a, 0xb,

    /* U+004E "N" */
    0x7b, 0x7, 0x49, 0x86, 0x74, 0x92, 0xa9, 0x49,
    0x21, 0xe3,

    /* U+004F "O" */
    0x6b, 0x9c, 0x19, 0x30, 0x92, 0x93, 0x9, 0x26,
    0xb9, 0xc1,

    /* U+0050 "P" */
    0x9a, 0x9b, 0x19, 0x30, 0x83, 0x9a, 0x9a, 0x19,
    0x30, 0x0,

    /* U+0051 "Q" */
    0x6b, 0x9c, 0x19, 0x30, 0x93, 0x93, 0x9, 0x36,
    0xb9, 0xc1, 0x0, 0x26, 0x0,

    /* U+0052 "R" */
    0x9a, 0x9c, 0x19, 0x30, 0x93, 0x9a, 0x99, 0x9,
    0x37, 0x60,

    /* U+0053 "S" */
    0x8a, 0x96, 0x69, 0x95, 0x0, 0xb, 0x69, 0xa8,

    /* U+0054 "T" */
    0x8d, 0xa4, 0xa, 0x20, 0xa, 0x20, 0xa, 0x20,

    /* U+0055 "U" */
    0x93, 0x8, 0x49, 0x30, 0x84, 0x93, 0x8, 0x46,
    0xb9, 0xc1,

    /* U+0056 "V" */
    0xc, 0x0, 0x75, 0x5, 0x70, 0xc0, 0x0, 0xb7,
    0x50, 0x0, 0x5c, 0x0,

    /* U+0057 "W" */
    0xb0, 0x6a, 0xb, 0x7, 0x4b, 0xa1, 0xb0, 0x2a,
    0xa6, 0xa6, 0x0, 0xc5, 0x1e, 0x10,

    /* U+0058 "X" */
    0x76, 0x1b, 0x10, 0x9c, 0x20, 0xa, 0xc2, 0x8,
    0x51, 0xb1,

    /* U+0059 "Y" */
    0xa, 0x31, 0xc0, 0x1, 0xbb, 0x20, 0x0, 0x58,
    0x0, 0x0, 0x47, 0x0,

    /* U+005A "Z" */
    0x69, 0xba, 0x1, 0xb1, 0x1b, 0x10, 0xab, 0x96,

    /* U+005B "[" */
    0x46, 0x64, 0x64, 0x64, 0x64, 0x46,

    /* U+005C "\\" */
    0x80, 0x0, 0x18, 0x0, 0x6, 0x30, 0x0, 0x90,

    /* U+005D "]" */
    0x55, 0x28, 0x28, 0x28, 0x28, 0x55,

    /* U+005E "^" */
    0x57, 0x20,

    /* U+005F "_" */
    0x77, 0x77, 0x0,

    /* U+0060 "`" */
    0x5,

    /* U+0061 "a" */
    0x48, 0x89, 0x48, 0x7b, 0x79, 0x89,

    /* U+0062 "b" */
    0x91, 0x0, 0x99, 0x89, 0x91, 0xb, 0x69, 0x89,

    /* U+0063 "c" */
    0x69, 0x80, 0x91, 0x0, 0x69, 0x80,

    /* U+0064 "d" */
    0x0, 0xb, 0x69, 0x8c, 0x91, 0xb, 0x69, 0x89,

    /* U+0065 "e" */
    0x69, 0x89, 0x98, 0x73, 0x69, 0x86,

    /* U+0066 "f" */
    0x68, 0x98, 0x91, 0x91,

    /* U+0067 "g" */
    0x6a, 0x89, 0x92, 0xb, 0x6a, 0x8b, 0x48, 0x89,

    /* U+0068 "h" */
    0x91, 0x0, 0x9a, 0xa8, 0x91, 0xb, 0x91, 0xb,

    /* U+0069 "i" */
    0x5b, 0xbb,

    /* U+006A "j" */
    0x4, 0x0, 0x91, 0x9, 0x10, 0x91, 0x3b, 0x0,

    /* U+006B "k" */
    0x91, 0x0, 0x93, 0xa1, 0x9c, 0x20, 0x92, 0xa2,

    /* U+006C "l" */
    0x91, 0x91, 0x91, 0x91,

    /* U+006D "m" */
    0x5b, 0xbb, 0xc2, 0x91, 0x73, 0x64, 0x91, 0x73,
    0x64,

    /* U+006E "n" */
    0x9a, 0xb6, 0xb0, 0x28, 0xb0, 0x28,

    /* U+006F "o" */
    0x78, 0x88, 0xa0, 0xa, 0x89, 0x88,

    /* U+0070 "p" */
    0x79, 0x89, 0x91, 0xa, 0x99, 0x88, 0x91, 0x0,

    /* U+0071 "q" */
    0x69, 0x88, 0x91, 0xa, 0x69, 0x8b, 0x0, 0xa,

    /* U+0072 "r" */
    0x98, 0x1b, 0x0, 0xb0, 0x0,

    /* U+0073 "s" */
    0x88, 0x82, 0x58, 0x91, 0x58, 0xb3,

    /* U+0074 "t" */
    0x91, 0x98, 0x91, 0x91,

    /* U+0075 "u" */
    0x91, 0xb, 0x91, 0xb, 0x6b, 0xa9,

    /* U+0076 "v" */
    0xa, 0x1, 0xa0, 0x38, 0x82, 0x0, 0x98, 0x0,

    /* U+0077 "w" */
    0xa0, 0xc3, 0x91, 0x67, 0x88, 0xa0, 0x1d, 0x1a,
    0x60,

    /* U+0078 "x" */
    0x66, 0xa2, 0xc, 0x70, 0x75, 0x93,

    /* U+0079 "y" */
    0x92, 0xb, 0x92, 0xb, 0x5b, 0xab, 0x4a, 0xa8,

    /* U+007A "z" */
    0x68, 0xc7, 0x8, 0x70, 0x9b, 0x85,

    /* U+007B "{" */
    0x28, 0x6, 0x40, 0x83, 0x8, 0x30, 0x64, 0x2,
    0x80,

    /* U+007C "|" */
    0x44, 0x55, 0x55, 0x55, 0x55,

    /* U+007D "}" */
    0x74, 0x1, 0x90, 0xb, 0x0, 0xb0, 0x19, 0x7,
    0x40,

    /* U+007E "~" */
    0x27, 0x87, 0x40
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 38, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 31, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 36, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 5, .adv_w = 82, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 15, .adv_w = 65, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 25, .adv_w = 114, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 77, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 23, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 50, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 56, .adv_w = 35, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 62, .adv_w = 50, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 67, .adv_w = 64, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 73, .adv_w = 21, .box_w = 1, .box_h = 3, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 75, .adv_w = 34, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 77, .adv_w = 21, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 62, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 86, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 75, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 102, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 112, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 75, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 130, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 160, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 22, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 181, .adv_w = 22, .box_w = 1, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 64, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 195, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 66, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 94, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 223, .adv_w = 75, .box_w = 6, .box_h = 4, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 235, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 70, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 263, .adv_w = 72, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 71, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 279, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 76, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 26, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 36, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 307, .adv_w = 66, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 63, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 99, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 337, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 347, .adv_w = 74, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 74, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 367, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 380, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 68, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 398, .adv_w = 56, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 406, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 416, .adv_w = 72, .box_w = 6, .box_h = 4, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 428, .adv_w = 100, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 67, .box_w = 6, .box_h = 4, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 464, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 472, .adv_w = 34, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 478, .adv_w = 57, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 34, .box_w = 2, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 492, .adv_w = 46, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 494, .adv_w = 64, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 497, .adv_w = 40, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 498, .adv_w = 67, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 504, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 54, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 518, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 65, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 532, .adv_w = 39, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 536, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 544, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 552, .adv_w = 25, .box_w = 1, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 25, .box_w = 3, .box_h = 5, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 562, .adv_w = 58, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 570, .adv_w = 25, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 574, .adv_w = 91, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 583, .adv_w = 63, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 589, .adv_w = 66, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 595, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 603, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 611, .adv_w = 39, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 616, .adv_w = 60, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 622, .adv_w = 38, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 67, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 632, .adv_w = 63, .box_w = 5, .box_h = 3, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 640, .adv_w = 86, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 649, .adv_w = 58, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 655, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 663, .adv_w = 63, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 669, .adv_w = 36, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 678, .adv_w = 64, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 683, .adv_w = 36, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 692, .adv_w = 85, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 1}
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
    -13, -6, -4, -3, -8, -3, -6, -5,
    -3, -6, -3, -4, -3, -4, -3, -4,
    -4, -6, -6, -4, -3, -8, -8, -6,
    -8, -3, -3, -3, -4, -13, -3, -13,
    -4, -3, -3, -3, -3, -1, -3, -5,
    -3, -3, -3, -3, -3, -11, -13, -11,
    -6, -10, -10, -10, -10, -10, -8, -8,
    -10, -8, -10, -8, -10, -8, -10, -10,
    -8, -9, -8, -6, -10, -5, -5, -5,
    -5, -5, -5, -3, -3, -5, -3, -5,
    -3, -4, -3, -3, -6, -1, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3,
    -4, -8, -6, -8, -5, -5, -5, -5,
    -3, -3, -5, -3, -5, -3, -4, -3,
    -5, -5, -5, -5, -8, -4, -8, 3,
    -4, 3, -6, -6, -4, -4, -5, -6,
    -6
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
const lv_font_t sony_8 = {
#else
lv_font_t sony_8 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 7,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if SONY_8*/

