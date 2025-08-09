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

    ELEMENT_ // TODO: add all tags
} bem_element;

typedef enum {
    COMPUTE_BASE,
    COMPUTE_BEFORE,
    COMPUTE_AFTER,
    COMPUTE_FIRST_LINE,
    COMPUTE_FIRST_LETTER
} bem_compute;

struct bem_file {
    bem_memory_pool *pool;
    const char *url;

    gzFile file_pointer;

    const bem_uchar *buffer, *buffer_pointer, *buffer_end;

    int line_number;
};

struct bem_memory_pool {
    struct lconv *locale;
    size_t locale_decimal_length;

    // TODO: fonts

    size_t strings_amount;
    size_t strings_size;
    char **strings;

    /*
    TODO:
    bem_dict *urls;
    bem_url_callback url_callback;
    void *url_context;
    */

    /*
    TODO: 
    bem_error_callback error_callback;
    void *error_context;
    char *last_error;
    */
};

typedef struct {
    bem_element element;
    const char *attribute_name;
    const char *property_name, *property_value;
} bem_attribute_mapping;

typedef struct {
    int score;
    int order;
    
    // TODO: bem_rule_set *rule;
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
    // TODO:
} bem_box;

typedef struct {
    // TODO:
} bem_media;

typedef struct {
    bem_border_collapse border_collapse;
    bem_caption_side caption_side;
    bem_empty_cells empty_cells;
    bem_table_layout table_layout;
} bem_table;

typedef struct {
    // TODO:
} bem_text;

struct bem_document {
    bem_memory_pool *pool;
    // TODO: bem_stylesheet *css;
    bem_node *root;
    
    /*
    TODO:
    bem_error_callback error_callback;
    void *error_context;

    bem_url_callback url_callback;
    void *url_context;
    */
};

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

            // TODO: bem_dict *attributes;
            // TODO: const bem_dict *base_properties;

            bem_document *html;
        } element;
        
        char string[1];
        char unknown[1];
    } value;
};

// TODO: Define function headers
