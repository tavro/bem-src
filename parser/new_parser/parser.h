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
  BEM_LOGOP_NONE,
  BEM_LOGOP_OR,
  BEM_LOGOP_AND
} bem_logop;

typedef enum {
  BEM_TYPE_ERROR,
  BEM_TYPE_RESERVED,
  BEM_TYPE_STRING,
  BEM_TYPE_QSTRING,
  BEM_TYPE_NUMBER
} bem_type;

static const char * const types[] = {
  "ERROR",
  "RESERVED",
  "STRING",
  "QSTRING",
  "NUMBER"
};

typedef enum {
    FONT_STRECH_NORMAL,
    FONT_STRECH_ULTRA_CONDENCED,
    FONT_STRECH_EXTRA_CONDENCED,
    FONT_STRECH_CONDENCED,
    FONT_STRECH_SEMI_CONDENCED,
    FONT_STRECH_ULTRA_EXPANDED,
    FONT_STRECH_EXTRA_EXPANDED,
    FONT_STRECH_EXPANDED,
    FONT_STRECH_SEMI_EXPANDED
} font_strech;

typedef enum {
    FONT_STYLE_NORMAL,
    FONT_STYLE_ITALIC,
    FONT_STYLE_OBLIQUE
} font_style;

typedef enum {
    FONT_VARIANT_NORMAL,
    FONT_VARIANT_SMALL_CAPS
} font_variant;

typedef enum {
    FONT_WEIGHT_NORMAL,
    FONT_WEIGHT_BOLD,
    FONT_WEIGHT_BOLDER,
    FONT_WEIGHT_LIGHTER,
    FONT_WEIGHT_100 = 100,
    FONT_WEIGHT_200 = 200,
    FONT_WEIGHT_300 = 300,
    FONT_WEIGHT_400 = 400,
    FONT_WEIGHT_500 = 500,
    FONT_WEIGHT_600 = 600,
    FONT_WEIGHT_700 = 700,
    FONT_WEIGHT_800 = 800,
    FONT_WEIGHT_900 = 900
} font_weight;

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
} bem_stylesheet_selector_statement;

typedef struct {
    bem_stylesheet_selector *previous;
    bem_element element;
    bem_relation relation;

    size_t statement_amount;
    bem_stylesheet_selector_statement *statements;
} bem_stylesheet_selector;

typedef struct {
    bem_sha3_256 hash;
    bem_stylesheet_selector *selector;
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
  char font_url[380];
  char font_family[128];

  unsigned char	font_style;
  unsigned char	font_index;

  unsigned short font_weight;
} bem_font_cache;

typedef struct {
  const char *font_url;
  const char *font_family;

  size_t font_index;
  
  bem_font *font;

  bem_font_stretch font_stretch;
  bem_font_style font_style;
  bem_font_variant font_variant;
  bem_font_weight font_weight;
} bem_font_info;

typedef struct {
    struct lconv *locale;
    size_t locale_decimal_length;

    bool fonts_loaded;
    size_t font_amount;
    size_t fonts_size;
    size_t font_index[256];
    bem_font_info *fonts;

    size_t string_amount;
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
} bem_stylesheet_match;

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

    bem_font *font;
    bem_font_stretch font_strech;
    bem_font_style font_style;
    bem_font_variant font_variant;
    bem_font_weight font_weight;

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

typedef struct {
    short width;
    short left_bearing;
} bem_font_metric;

typedef struct {
    bem_memory_pool *pool;
    
    size_t index;
    size_t fonts_amount;
    
    const char *copyright;
    const char *family;
    const char *postscript_name;
    const char *version;
    
    bool is_fixed_width;
    
    int max_char;
    int min_char;
    
    size_t cmap_amount;
    int *cmap;
    
    bem_font_metric	*widths[262144 / 256]; // TODO: Make constant for this
    
    float units;
    float italic_angle;

    short ascent, descent;
    short cap_height, x_height;
    short x_max, x_min, y_max, y_min;
	short weight;

    bem_font_stretch stretch;
    bem_font_style style;
} bem_font;

typedef struct {
  unsigned tag;
  unsigned checksum;
  unsigned offset;
  unsigned length;
} bem_off_dir;

typedef struct {
  int num_entries;
  bem_off_dir *entries;
} bem_off_table;

typedef struct {
  unsigned short platform_id, encoding_id, language_id, name_id;
  unsigned short length, offset;
} bem_off_name;

typedef struct {
  int num_names;
  bem_off_name *names;
  unsigned char *storage;
  unsigned storage_size;
} bem_off_names;

typedef struct {
  unsigned short start_code, end_code;
  unsigned short id_range_offset;
  short	id_delta;
} bem_off_cmap4;

typedef struct {
  unsigned start_char_code, end_char_code;
  unsigned start_glyph_id;
} bem_off_cmap12;

typedef struct {
  unsigned start_char_code, end_char_code;
  unsigned glyph_id;
} bem_off_cmap13;

typedef struct {
  unsigned short units_per_em;
  short	x_min, y_min, x_max, y_max;
  unsigned short mac_style;
} bem_off_head;

typedef struct {
  short ascender, descender;
  int number_of_horizontal_metrics;
} bem_off_hhea;

typedef struct {
  unsigned short weight_class, width_class,	fs_type;
  short	typo_ascender, typo_descender;
  short x_height, cap_height;
} bem_off_os_2;

typedef struct {
  float	italic_angle;
  unsigned is_fixed_pitch;
} bem_off_post;

typedef int (*bem_comparison_function)(const void *, const void *);
typedef bool (*bem_error_callback)(void *context, const char *message, int line_number);
typedef char *(*bem_url_callback)(void *context, const char *url, char *buffer, size_t buffer_size);

extern bool	defaultErrorCallback(void *context, const char *message, int line_number);
extern char	*defaultURLCallback(void *context, const char *url, char *buffer, size_t buffer_size);

extern void	bemCSSImportString(bem_stylesheet *css, bem_dictionary *properties, const char *str);
extern void	bemCSSSelectorAddStatement(bem_stylesheet *css, bem_stylesheet_selector *selector, bem_match match, const char *name, const char *value);
extern void	bemCSSSelectorDelete(bem_stylesheet_selector *selector);
extern void	bemCSSSelectorHash(bem_stylesheet_selector *selector, bem_sha3_256 hash);
extern bem_stylesheet_selector *bemCSSSelectorNew(bem_stylesheet *css, bem_stylesheet_selector *previous, bem_element element, bem_relation relation);

extern void	bemRuleCollectionAdd(bem_stylesheet *css, bem_rule_collection *collection, bem_rule_set *rule);
extern void	bemRuleCollectionClear(bem_rule_collection *collection, int delete_rules);
extern bem_rule_set *bemRuleCollectionFindHash(bem_rule_collection *collection, const bem_sha3_256 hash);
extern void	bemRuleDelete(bem_rule_set *rule);
extern bem_rule_set *bemRuleNew(bem_stylesheet *css, const bem_sha3_512 hash, bem_stylesheet_selector *selector, bem_dictionary *properties);

extern void	bemCSSDelete(bem_stylesheet *css);
extern bem_stylesheet *bemCSSNew(bem_memory_pool *pool);
extern bool	bemCSSImport(bem_stylesheet *css, bem_file *file);
extern bool	bemCSSImportDefault(bem_stylesheet *css);
extern void	bemCSSSetErrorCallback(bem_stylesheet *css, bem_error_callback callback, void *context);
extern void	bemCSSSetURLCallback(bem_stylesheet *css, bem_url_callback callback, void *context);
extern int bemCSSSetMedia(bem_stylesheet *css, const char *type, int color_bits, int grayscale_bits, float width, float height);

extern bem_dictionary *bemDictionaryCopy(const bem_dictionary *dictionary);
extern void	bemDictionaryDelete(bem_dictionary *dictionary);
extern size_t bemDictionaryGetCount(const bem_dictionary *dictionary);
extern const char *bemDictionaryGetIndexKeyValue(const bem_dictionary *dictionary, size_t index, const char **key);
extern const char *bemDictionaryGetKeyValue(const bem_dictionary *dictionary, const char *key);
extern bem_dictionary *bemDictionaryNew(bem_memory_pool *pool);
extern void	bemDictionaryRemoveKey(bem_dictionary *dictionary, const char *key);
extern void	bemDictionarySetKeyValue(bem_dictionary *dictionary, const char *key, const char *value);

extern void	bemFileDelete(bem_file *file);
extern int bemFileGetc(bem_file *file);
extern bem_file	*bemFileNewBuffer(bem_memory_pool *pool, const void *buffer, size_t bytes);
extern bem_file	*bemFileNewString(bem_memory_pool *pool, const char *str);
extern bem_file	*bemFileNewURL(bem_memory_pool *pool, const char *url, const char *base_url);
extern size_t bemFileRead(bem_file *file, void *buffer, size_t bytes);
extern size_t bemFileSeek(bem_file *file, size_t offset);
extern void	bemFileUngetc(bem_file *file, int ch);

extern bool	bemNodeComputeCSSTextFont(bem_node *node, const bem_dictionary *properties, bem_text *text);
extern bem_node	*bemNodeNewUnknown(bem_node *parent, const char *unknown);

extern void	bemImageDelete(bem_image *image);
extern const char *bemImageGetFormat(bem_image *image);
extern int bemImageGetHeight(bem_image *image);
extern bem_size bemImageGetSize(bem_image *image);
extern int bemImageGetWidth(bem_image *image);
extern bem_image *bemImageNew(bem_memory_pool *pool, bem_file *file);

extern void	bemPoolDelete(bem_memory_pool *pool);
extern const char *bemPoolGetLastError(bem_memory_pool *pool);
extern const char *bemPoolGetString(bem_memory_pool *pool, const char *str);
extern const char *bemPoolGetURL(bem_memory_pool *pool, const char *url, const char *base_url);
extern bem_memory_pool *bemPoolNew(void);
extern void	bemPoolSetErrorCallback(bem_memory_pool *pool, bem_error_callback callback, void *context);
extern void	bemPoolSetURLCallback(bem_memory_pool *pool, bem_url_callback callback, void *context);

extern void	bemSHA3Final(bem_sha3 *context, unsigned char *hash, size_t hash_length);
extern void	bemSHA3Init(bem_sha3 *context);
extern void	bemSHA3Update(bem_sha3 *context, const void *data, size_t data_length);

extern void	bemFontAddCached(bem_memory_pool *pool, bem_font *font, const char *url);
extern int bemFontComputeExtents(const bem_font *font, float size, const char *str, bem_rectangle *extents);
extern void	bemFontDelete(bem_font *font);
extern bem_font	*bemFontFindCached(bem_memory_pool *pool, const char *family, bem_font_stretch stretch, bem_font_style style, bem_font_variant variant, bem_font_weight weight);
extern int bemFontGetAscent(bem_font *font);
extern bem_rectangle *bemFontGetBounds(bem_font *font, bem_rectangle *bounds);
extern bem_font	*bemFontGetCached(bem_memory_pool *pool, size_t index);
extern size_t bemFontGetCachedCount(bem_memory_pool *pool);
extern int bemFontGetCapHeight(bem_font *font);
extern const int *bemFontGetCMap(bem_font *font, size_t *num_cmap);
extern const char *bemFontGetCopyright(bem_font *font);
extern int bemFontGetDescent(bem_font *font);
extern bem_rectangle *bemFontGetExtents(bem_font *font, float size, const char *str, bem_rectangle *extents);
extern const char *bemFontGetFamily(bem_font *font);
extern size_t bemFontGetFontAmount(bem_font *font);
extern const char *bemFontGetPostScriptName(bem_font *font);
extern bem_font_style bemFontGetStyle(bem_font *font);
extern const char *bemFontGetVersion(bem_font *font);
extern bem_font_weight bemFontGetWeight(bem_font *font);
extern int bemFontGetWidth(bem_font *font, int ch);
extern int bemFontGetXHeight(bem_font *font);
extern bool bemFontIsFixedPitch(bem_font *font);
extern bem_font	*bemFontNew(bem_memory_pool *pool, bem_file *file, size_t index);

extern const char *bemElementString(bem_element element);
extern bem_element bemElementValue(const char *str);
extern void	bemHTMLDelete(bem_document *html);
extern bem_node	*bemHTMLFindNode(bem_document *html, bem_node *current, bem_element element, const char *id);
extern bem_stylesheet *bemHTMLGetCSS(bem_document *html);
extern const char *bemHTMLGetDOCTYPE(bem_document *html);
extern bem_node	*bemHTMLGetRootNode(bem_document *html);
extern bool	bemHTMLImport(bem_document *html, bem_file_t *file);
extern bem_document	*bemHTMLNew(bem_memory_pool *pool, bem_stylesheet *css);
extern bem_node	*bemHTMLNewRootNode(bem_document *html, const char *doctype);
extern void	bemHTMLSetErrorCallback(bem_document *html, bem_error_callback callback, void *cbdata);
extern void	bemHTMLSetURLCallback(bem_document *html, bem_url_callback callback, void *cbdata);
extern size_t bemNodeAttributeGetCount(bem_node *node);
extern const char *bemNodeAttributeGetIndexNameValue(bem_node *node, size_t index, const char **name);
extern const char *bemNodeAttributeGetNameValue(bem_node *node, const char *name);
extern void	bemNodeAttributeRemove(bem_node *node, const char *name);
extern void	bemNodeAttributeSetNameValue(bem_node *node, const char *name, const char *value);
extern bool	bemNodeComputeCSSBox(bem_node *node, bem_compute compute, bem_box *box);
extern char	*bemNodeComputeCSSContent(bem_node *node, bem_compute compute);
extern bem_display bemNodeComputeCSSDisplay(bem_node *node, bem_compute compute);
extern bool	bemNodeComputeCSSMedia(bem_node *node, bem_compute compute, bem_media *media);
extern const bem_dict *bemNodeComputeCSSProperties(bem_node *node, bem_compute compute);
extern bool bemNodeComputeCSSTable(bem_node *node, bem_compute compute, bem_table *table);
extern bool	bemNodeComputeCSSText(bem_node *node, bem_compute compute, bem_text *text);
extern void	bemNodeDelete(bem_document *html, bem_node *node);
extern const char *bemNodeGetComment(bem_node *node);
extern bem_element bemNodeGetElement(bem_node *node);
extern bem_node	*bemNodeGetFirstChildNode(bem_node *node);
extern bem_node	*bemNodeGetLastChildNode(bem_node *node);
extern bem_node	*bemNodeGetNextSiblingNode(bem_node *node);
extern bem_node	*bemNodeGetParentNode(bem_node *node);
extern bem_node	*bemNodeGetPreviousSiblingNode(bem_node *node);
extern const char *bemNodeGetString(bem_node *node);
extern bem_node	*bemNodeNewComment(bem_node *parent, const char *c);
extern bem_node	*bemNodeNewElement(bem_node *parent, bem_element element);
extern bem_node	*bemNodeNewString(bem_node *parent, const char *s);

extern bool	bemFileError(bem_file *file, const char *message, ...);

extern bool	bemPoolError(bem_memory_pool *pool, int line_number, const char *message, ...);
extern bool	bemPoolErrorv(bem_memory_pool *pool, int line_number, const char *message, va_list ap);

extern void	bemPoolDeleteFonts(bem_memory_pool *pool);

static int bemCompareMatches(bem_match *a, bem_match *b);
static const bem_dict *bemCreateProperties(bem_node *node, bem_compute compute);
static bool bemGetColor(bem_memory_pool *pool, const char *value, bem_color *color);
static float bemGetLength(bem_memory_pool *pool, const char *value, float max_value, float multiplier, bem_stylesheet *css, bem_text *text);
static int bemMatchNode(bem_node *node, bem_stylesheet_selector *selector, const char *pseudo_class);
static int bemMatchRule(bem_node *node, bem_rule_set *rule, const char *pseudo_class);
static double bemStrtod(bem_memory_pool *pool, const char *str, char **end);

static void	bemAddRule(bem_stylesheet *css, bem_stylesheet_selector *selector, bem_dictionary *properties);
static int bemEvaluateMedia(bem_stylesheet *css, bem_file *file, bem_type *type, char *buffer, size_t buffer_size);
static char	*bemRead(bem_file *file, bem_type *type, char *buffer, size_t buffer_size);
static bem_dictionary *bemReadProperties(bem_stylesheet *css, bem_file *file, bem_dictionary *properties);
static bem_stylesheet_selector *bemReadSelector(bem_stylesheet *css, bem_file *file, bem_type *type, char *buffer, size_t buffer_size);
static char	*bemReadValue(bem_file *file, char *buffer, size_t buffer_size);

static int bemCompareRules(bem_rule_set **a, bem_rule_set **b);
static int bemComparePairs(bem_pair *a, bem_pair *b);

static void	bemAddFont(bem_memory_pool *pool, bem_font *font, const char *url, bool delete_it);
static int bemCompareInfo(bem_font_info *a, bem_font_info *b);
static void	bemGetCname(char *cname, size_t cname_size);
static void	bemLoadAllFonts(bem_memory_pool *pool);
static bool	bemLoadCache(bem_memory_pool *pool, const char *cname, struct stat *cinfo);
static time_t bemLoadFonts(bem_memory_pool *pool, const char *d, bool scan_only);
static void	bemSaveCache(bem_memory_pool *pool, const char *cname);
static void	bemSortFonts(bem_memory_pool *pool);

static bem_node	*bemHtmlWalkNext(bem_node *current);
static int	bemHtmlParseAttribute(bem_file *file, int ch, bem_node *node);
static bool	bemHtmlParseComment(bem_file *file, bem_node **parent);
static bool	bemHtmlParseDoctype(bem_file *file, bem_document *html, bem_node **parent);
static bool	bemHtmlParseElement(bem_file *file, int ch, bem_document *html, bem_node **parent);
static bool	bemHtmlParseUnknown(bem_file *file, bem_node **parent, const char *unknown);
static void	bemHtmlDelete(bem_node *node);
static bem_node *bemHtmlNew(bem_node *parent, bem_element element, const char *str);
static void	bemHtmlRemove(bem_node *node);

static int bemCompareStrings(char **a, char **b);

static const char *bemCopyName(bem_memory_pool *pool, bem_off_names *names, unsigned name_id);
static int	bemReadCmap(bem_file *file, bem_off_table *table, int **cmap);
static bool	bemReadHead(bem_file *file, bem_off_table *table, bem_off_head *head);
static bool	bemReadHhea(bem_file *file, bem_off_table *table, bem_off_hhea *hhea);
static bem_font_metric *bemReadHmtx(bem_file *file, bem_off_table *table, bem_off_hhea *hhea);
static int	bemReadMaxp(bem_file *file, bem_off_table *table);
static bool	bemReadNames(bem_file *file, bem_off_table *table, bem_off_names *names);
static bool	bemReadOs2(bem_file *file, bem_off_table *table, bem_off_os_2 *os_2);
static bool	bemReadPost(bem_file *file, bem_off_table *table, bem_off_post *post);
static int	bemReadShort(bem_file *file);
static bool	bemReadTable(bem_file *file, size_t index, bem_off_table *table, size_t *num_fonts);
static unsigned	bemReadUlong(bem_file *file);
static int	bemReadUshort(bem_file *file);
static unsigned	bemSeekTable(bem_file *file, bem_off_table *table, unsigned tag, unsigned offset);
