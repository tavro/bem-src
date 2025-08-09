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

typedef enum {
    ELEMENT_UNKNOWN = -2,
    ELEMENT_STRING = -1,
    ELEMENT_WILDCARD,
    ELEMENT_COMMENT,
    ELEMENT_DOCTYPE,

    ELEMENT_ // TODO: add all tags
} bem_element;

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
