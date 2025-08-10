/*
 * Copyright (c) 2025 Tyson Horvath (tavro)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <zlib.h>

#define BEM_SHA3_256_SIZE 32
#define BEM_SHA3_512_SIZE 64

typedef enum {
    BACKGROUND_ATTACHMENT_SCROLL,
    BACKGROUND_ATTACHMENT_FIXED
} bem_background_attachment;

typedef enum {
    BACKGROUND_BOX_BORDER,
    BACKGROUND_BOX_PADDING,
    BACKGROUND_BOX_CONTENT
} bem_background_box;

typedef enum {
    BACKGROUND_REPEAT,
    BACKGROUND_REPEAT_NONE,
    BACKGROUND_REPEAT_X,
    BACKGROUND_REPEAT_Y
} bem_background_repeat;

typedef enum {
    BORDER_COLLAPSE_SEPARATE,
    BORDER_COLLAPSE_COLLAPSE
} bem_border_collapse;

typedef enum {
    BORDER_IMAGE_REPEAT_STRETCH,
    BORDER_IMAGE_REPEAT_REPEAT,
    BORDER_IMAGE_REPEAT_ROUND,
    BORDER_IMAGE_REPEAT_SPACE
} bem_border_image_repeat;

typedef enum {
    BORDER_STYLE_HIDDEN,
    BORDER_STYLE_NONE,
    BORDER_STYLE_DOTTED,
    BORDER_STYLE_DASHED,
    BORDER_STYLE_SOLID,
    BORDER_STYLE_DOUBLE,
    BORDER_STYLE_GROOVE,
    BORDER_STYLE_RIDGE,
    BORDER_STYLE_INSET,
    BORDER_STYLE_OUTSET
} bem_border_style;

typedef enum {
    BREAK_AUTO,
    BREAK_ALWAYS,
    BREAK_AVOID,
    BREAK_LEFT,
    BREAK_RIGHT
} bem_break;

typedef enum {
    CAPTION_SIDE_TOP,
    CAPTION_SIDE_BOTTOM
} bem_caption_side;

typedef enum {
    DIRECTION_LEFT_TO_RIGHT,
    DIRECTION_RIGHT_TO_LEFT
} bem_direction;

typedef enum {
    DISPLAY_NONE,
    DISPLAY_BLOCK,
    DISPLAY_INLINE,
    DISPLAY_INLINE_BLOCK,
    DISPLAY_INLINE_TABLE,
    DISPLAY_LIST_ITEM,
    DISPLAY_TABLE,
    DISPLAY_TABLE_CAPTION,
    DISPLAY_TABLE_HEADER_GROUP,
    DISPLAY_TABLE_FOOTER_GROUP,
    DISPLAY_TABLE_ROW_GROUP,
    DISPLAY_TABLE_ROW,
    DISPLAY_TABLE_COLUMN_GROUP,
    DISPLAY_TABLE_COLUMN,
    DISPLAY_TABLE_CELL
} bem_display;

typedef enum {
    EMPTY_CELLS_HIDE,
    EMPTY_CELLS_SHOW
} bem_empty_cells;

typedef enum {
    FLOAT_NONE,
    FLOAT_LEFT,
    FLOAT_RIGHT
} bem_float;

typedef enum {
    LIST_STYLE_POSITION_INSIDE,
    LIST_STYLE_POSITION_OUTSIDE
} bem_list_style_position;

typedef enum {
    LIST_STYLE_TYPE_DISC,
    LIST_STYLE_TYPE_CIRCLE,
    LIST_STYLE_TYPE_SQUARE,
    LIST_STYLE_TYPE_DECIMAL,
    LIST_STYLE_TYPE_DECIMAL_LEADING_ZERO,
    LIST_STYLE_TYPE_LOWER_ROMAN,
    LIST_STYLE_TYPE_UPPER_ROMAN,
    LIST_STYLE_TYPE_LOWER_GREEK,
    LIST_STYLE_TYPE_LOWER_LATIN,
    LIST_STYLE_TYPE_ARMENIAN,
    LIST_STYLE_TYPE_GEORGIAN,
    LIST_STYLE_TYPE_LOWER_ALPHA,
    LIST_STYLE_TYPE_UPPER_ALPHA,
    LIST_STYLE_TYPE_NONE
} bem_list_style_type;

typedef enum {
    OVERFLOW_HIDDEN,
    OVERFLOW_VISIBLE,
    OVERFLOW_SCROLL,
    OVERFLOW_AUTO
} bem_overflow;

typedef enum {
    TABLE_LAYOUT_AUTO,
    TABLE_LAYOUT_FIXED
} bem_table_layout;

typedef enum {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_RIGHT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_JUSTIFY
} bem_text_align;

typedef enum {
    TEXT_DECORATION_NONE,
    TEXT_DECORATION_UNDERLINE,
    TEXT_DECORATION_OVERLINE,
    TEXT_DECORATION_LINE_THROUGH
} bem_text_decoration;

typedef enum {
    TEXT_TRANSFORM_NONE,
    TEXT_TRANSFORM_CAPITALIZE,
    TEXT_TRANSFORM_LOWERCASE,
    TEXT_TRANSFORM_UPPERCASE
} bem_text_transform;

typedef enum {
    UNICODE_BIDIRECTIONAL_NORMAL,
    UNICODE_BIDIRECTIONAL_EMBED,
    UNICODE_BIDIRECTIONAL_OVERRIDE
} bem_unicode_bidirectional;

typedef enum {
    WHITE_SPACE_NORMAL,
    WHITE_SPACE_NO_WRAP,
    WHITE_SPACE_PRE,
    WHITE_SPACE_PRE_LINE,
    WHITE_SPACE_PRE_WRAP
} bem_white_space;

typedef enum {
    ELEMENT_UNKNOWN = -2,
    ELEMENT_STRING = -1,
    ELEMENT_WILDCARD,
    ELEMENT_COMMENT,
    ELEMENT_DOCTYPE,

    ELEMENT_A,
    ELEMENT_ABBR,
    ELEMENT_ACRONYM,
    ELEMENT_ADDRESS,
    ELEMENT_APPLET,
    ELEMENT_AREA,
    ELEMENT_ARTICLE,
    ELEMENT_ASIDE,
    ELEMENT_AUDIO,

    ELEMENT_B,
    ELEMENT_BASE,
    ELEMENT_BASEFONT,
    ELEMENT_BDI,
    ELEMENT_BDO,
    ELEMENT_BIG,
    ELEMENT_BLINK,
    ELEMENT_BLOCKQUOTE,
    ELEMENT_BODY,
    ELEMENT_BR,
    ELEMENT_BUTTON,

    ELEMENT_CANVAS,
    ELEMENT_CAPTION,
    ELEMENT_CENTER,
    ELEMENT_CITE,
    ELEMENT_CODE,
    ELEMENT_COL,
    ELEMENT_COLGROUP,

    ELEMENT_DATA,
    ELEMENT_DATALIST,
    ELEMENT_DD,
    ELEMENT_DEL,
    ELEMENT_DETAILS,
    ELEMENT_DFN,
    ELEMENT_DIALOG,
    ELEMENT_DIR,
    ELEMENT_DIV,
    ELEMENT_DL,
    ELEMENT_DT,

    ELEMENT_EM,
    ELEMENT_EMBED,

    ELEMENT_FIELDSET,
    ELEMENT_FIGCAPTION,
    ELEMENT_FIGURE,
    ELEMENT_FONT,
    ELEMENT_FOOTER,
    ELEMENT_FORM,
    ELEMENT_FRAME,
    ELEMENT_FRAMESET,

    ELEMENT_H1,
    ELEMENT_H2,
    ELEMENT_H3,
    ELEMENT_H4,
    ELEMENT_H5,
    ELEMENT_H6,
    ELEMENT_HEAD,
    ELEMENT_HEADER,
    ELEMENT_HR,
    ELEMENT_HTML,

    ELEMENT_I,
    ELEMENT_IFRAME,
    ELEMENT_IMG,
    ELEMENT_INPUT,
    ELEMENT_INS,
    ELEMENT_ISINDEX,

    ELEMENT_KBD,

    ELEMENT_LABEL,
    ELEMENT_LEGEND,
    ELEMENT_LI,
    ELEMENT_LINK,

    ELEMENT_MAIN,
    ELEMENT_MAP,
    ELEMENT_MARK,
    ELEMENT_MENU,
    ELEMENT_META,
    ELEMENT_METER,
    ELEMENT_MULTICOL,

    ELEMENT_NAV,
    ELEMENT_NOBR,
    ELEMENT_NOFRAMES,
    ELEMENT_NOSCRIPT,

    ELEMENT_OBJECT,
    ELEMENT_OL,
    ELEMENT_OPTGROUP,
    ELEMENT_OPTION,
    ELEMENT_OUTPUT,

    ELEMENT_P,
    ELEMENT_PARAM,
    ELEMENT_PICTURE,
    ELEMENT_PRE,
    ELEMENT_PROGRESS,

    ELEMENT_Q,

    ELEMENT_RB,
    ELEMENT_RP,
    ELEMENT_RT,
    ELEMENT_RTC,
    ELEMENT_RUBY,

    ELEMENT_S,
    ELEMENT_SAMP,
    ELEMENT_SCRIPT,
    ELEMENT_SECTION,
    ELEMENT_SELECT,
    ELEMENT_SMALL,
    ELEMENT_SOURCE,
    ELEMENT_SPACER,
    ELEMENT_SPAN,
    ELEMENT_STRIKE,
    ELEMENT_STRONG,
    ELEMENT_STYLE,
    ELEMENT_SUB,
    ELEMENT_SUMMARY,
    ELEMENT_SUP,

    ELEMENT_TABLE,
    ELEMENT_TBODY,
    ELEMENT_TD,
    ELEMENT_TEMPLATE,
    ELEMENT_TEXTAREA,
    ELEMENT_TFOOT,
    ELEMENT_TH,
    ELEMENT_THEAD,
    ELEMENT_TIME,
    ELEMENT_TITLE,
    ELEMENT_TR,
    ELEMENT_TRACK,
    ELEMENT_TT,

    ELEMENT_U,
    ELEMENT_UL,

    ELEMENT_VAR,
    ELEMENT_VIDEO,

    ELEMENT_WBR,

    ELEMENT_MAX
} bem_element;

typedef enum {
    COMPUTE_BASE,
    COMPUTE_BEFORE,
    COMPUTE_AFTER,
    COMPUTE_FIRST_LINE,
    COMPUTE_FIRST_LETTER
} bem_compute;

typedef enum {
    RESOLUTION_NONE,
    RESOLUTION_PER_INCH,
    RESOLUTION_PER_CM
} bem_resolution;

typedef enum {
    MATCH_ATTRIBUTE_EXIST,
    MATCH_ATTRIBUTE_EQUALS,
    MATCH_ATTRIBUTE_CONTAINS,
    MATCH_ATTRIBUTE_BEGINS,
    MATCH_ATTRIBUTE_ENDS,
    MATCH_ATTRIBUTE_LANGUAGE
    MATCH_ATTRIBUTE_SPACE
    MATCH_CLASS,
    MATCH_ID,
    MATCH_PSEUDO_CLASS
} bem_match;

typedef enum {
    RELATION_CHILD,
    RELATION_IMMEDIATE_CHILD,
    RELATION_SIBLING,
    RELATION_IMMEDIATE_SIBLING
} bem_relation;

typedef unsigned char bem_uchar;

typedef unsigned char bem_sha3_256[BEM_SHA3_256_SIZE];
typedef unsigned char bem_sha3_512[BEM_SHA3_512_SIZE];

typedef struct {
    float left_offset;
    float top_offset;
    float right_offset;
    float bottom_offset;
} bem_rectangle;

typedef struct {
    bem_match match;

    const char *name, *value;
} bem_css_selector_statement;

typedef struct {
    bem_css_selector *previous;
    bem_element element;
    bem_relation relation;

    size_t statement_amount;
    bem_css_selector_statement *statements;
} bem_css_selector;

typedef struct {
    bem_sha3_256 hash;
    bem_css_selector *selector;
    bem_dictionary *properties;
} bem_rule_set;

typedef struct {
    int needs_sorting;

    size_t rules_size;
    size_t rules_amount;
    bem_rule_set **rules;
} bem_rule_collection;

typedef struct {
    bem_memory_pool *pool;
    bem_media media;
    bem_rule_collection all_rules;
    bem_rule_collection rules[ELEMENT_MAX];
} bem_stylesheet;

typedef struct {
    unsigned char bytes_used;
    unsigned char bytes_per_block;
    unsigned char state[200];
} bem_sha3;

typedef struct {
    bem_memory_pool *pool;
    
    const char *format;
    
    int width, height;
    int x_resolution, y_resolution;

    bem_resolution units;
} bem_image;

typedef struct {
    const char *key;
    const char *value;
} bem_pair;

typedef struct {
    bem_memory_pool *pool;
    const char *url;

    gzFile file_pointer;

    const bem_uchar *buffer, *buffer_pointer, *buffer_end;

    int line_number;
} bem_file;

typedef struct {
    struct lconv *locale;
    size_t locale_decimal_length;

    // TODO: font stuff

    size_t strings_amount;
    size_t strings_size;
    char **strings;

    bem_dictionary *urls;
    bem_url_callback url_callback;
    void *url_context;

    bem_error_callback error_callback;
    void *error_context;
    char *last_error;
} bem_memory_pool;

typedef struct {
    bem_memory_pool *pool;

    size_t pair_amount;
    size_t pairs_size;

    bem_pair *pairs;
} bem_dictionary; 

typedef struct {
    bem_element element;
    const char *attribute_name;
    const char *property_name, *property_value;
} bem_attribute_mapping;

typedef struct {
    int score;
    int order;
    
    bem_rule_set *rule;
} bem_css_match;

typedef struct {
    float red;
    float green;
    float blue;
    float alpha;
} bem_color;

typedef struct {
    float horizontal_position;
    float vertical_position;
} bem_point;

typedef struct {
    float width;
    float height;
} bem_size;

typedef struct {
    bem_color color;
    bem_border_style style;
    
    float width;
} bem_border_properties;

typedef struct {
    float horizontal_offset;
    float vertical_offset;
    float blur_radius;
    float spread_distance;

    bem_color color;

    bool inset;
} bem_box_shadow;

typedef struct {
    bem_border_properties left;
    bem_border_properties top;
    bem_border_properties right;
    bem_border_properties bottom;
} bem_border;

typedef struct {
    bem_size bottom_left;
    bem_size bottom_right;
    bem_size top_left;
    bem_size top_right;
} bem_border_radius;

typedef struct {
    bem_rectangle bounds;
    bem_rectangle clip;
    bem_rectangle border_image_outset;
    bem_rectangle border_image_slice;
    bem_rectangle border_image_width;
    bem_rectangle list_style_type;
    bem_rectangle margin;
    bem_rectangle padding;

    bem_size size;
    bem_size max_size;
    bem_size min_size;
    bem_size background_size;
    bem_size border_spacing;

    bem_background_attachment background_attachment;

    bem_background_box background_clip;
    bem_background_box background_origin;

    bem_color background_color;

    const char *background_image;
    const char *border_image;
    const char *list_style_image;

    bem_point background_position;

    bem_background_repeat background_repeat;

    bem_border border;

    bool border_image_fill;

    bem_border_image_repeat border_image_repeat[2];

    bem_border_radius border_radius;

    bem_box_shadow box_shadow;

    bem_break break_after;
    bem_break break_before;
    bem_break break_inside;

    bem_float float_value;

    bem_list_style_position list_style_position;

    bem_list_style_type list_style_type;

    int orphans;
    int windows;
    int z_index;

    bem_overflow overflow;
} bem_box;

typedef struct {
    const char *type;

    int color_bits;
    int monochrome_bits;

    bem_rectangle margin;
    bem_size size;
} bem_media;

typedef struct {
    bem_border_collapse border_collapse;
    bem_caption_side caption_side;
    bem_empty_cells empty_cells;
    bem_table_layout table_layout;
} bem_table;

typedef struct {
    bem_color color;
    bem_direction direction;

    // TODO: font stuff

    const char* font_family;
    const char* quotes[4];

    float font_size;
    float font_size_adjust;
    float letter_spacing;
    float line_height;
    float text_indent;
    float word_spacing;
    
    bem_text_align text_align;
    bem_text_decoration text_decoration;
    bem_text_transform text_transform;
    bem_unicode_bidirectional unicode_bidirectional;
    bem_white_space white_space;
} bem_text;

typedef struct {
    bem_memory_pool *pool;
    bem_stylesheet *css;
    bem_node *root;
    
    bem_error_callback error_callback;
    void *error_context;

    bem_url_callback url_callback;
    void *url_context;
} bem_document;

struct bem_node {
    bem_element element;

    bem_node *parent;
    bem_node *previous;
    bem_node *next;

    union {
        char comment[1];

        struct {
            bem_node *first_child;
            bem_node *last_child;

            bem_dictionary *attributes;
            const bem_dictionary *base_properties;

            bem_document *html;
        } element;
        
        char string[1];
        char unknown[1];
    } value;
};

typedef int (*bem_comparison_function)(const void *, const void *);
typedef bool (*bem_error_callback)(void *context, const char *message, int line_number);
typedef char *(*bem_url_callback)(void *context, const char *url, char *buffer, size_t buffer_size);

// TODO: Define function headers
