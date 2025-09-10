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

#include "parser.h"

bool bemDefaultErrorCallback(void *context, const char *message, int line_number)
{
    (void)context;
    (void)line_number;

    fputs(message, stderr);
    putc('\n', stderr);

    return true;
}

char *bemDefaultURLCallback(void *context, const char *url, char *buffer, size_t buffer_size)
{
    (void)context;

    if (!access(url, R_OK))
    {
        strncpy(buffer, url, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }
    else if (!strncmp(url, "file:///", 8))
    {
        char *buffer_pointer, *buffer_end;

        for (url += 7, buffer_pointer = buffer, buffer_end = buffer + buffer_size - 1; *url; url++)
        {
            int ch = *url;

            if (ch == '%' && isxdigit(url[1] & 255) && isxdigit(url[2] & 255))
            {
                if (isdigit(url[1]))
                {
                    ch = (url[1] - '0') << 4;
                }
                else
                {
                    ch = (tolower(url[1]) - 'a' + 10) << 4;
                }

                if (isdigit(url[2]))
                {
                    ch |= (url[2] - '0');
                }
                else
                {
                    ch |= (tolower(url[2]) - 'a' + 10);
                }

                url += 2;
            }

            if (buffer_pointer < buffer_end)
            {
                *buffer_pointer++ = (char)ch;
            }
            else
            {
                errno = E2BIG;
                *buffer = '\0';
                return NULL;
            }
        }

        *buffer_pointer = '\0';

        if (access(buffer, R_OK))
        {
            *buffer = '\0';
            return NULL;
        }
    }
    else
    {
        errno = EINVAL;
        *buffer = '\0';
        return NULL;
    }

    return buffer;
}

bem_dictionary *bemDictionaryCopy(const bem_dictionary *dictionary)
{
    bem_dictionary *new_dictionary;

    if (!dictionary)
        return NULL;
    if ((new_dictionary = calloc(1, sizeof(bem_dictionary))) == NULL)
        return NULL;

    new_dictionary->pool = dictionary->pool;
    new_dictionary->pair_amount = dictionary->pair_amount;
    new_dictionary->pairs_size = dictionary->pairs_size;

    if ((new_dictionary->pairs = calloc(new_dictionary->pair_amount, sizeof(bem_pair))) == NULL)
    {
        free(new_dictionary);
        return NULL;
    }

    memcpy(new_dictionary->pairs, dictionary->pairs, new_dictionary->pair_amount * sizeof(bem_pair));

    return new_dictionary;
}

void bemDictionaryDelete(bem_dictionary *dictionary)
{
    if (dictionary)
    {
        if (dictionary->pairs)
            free(dictionary->pairs);
        free(dictionary);
    }
}

size_t bemDictionaryGetCount(const bem_dictionary *dictionary)
{
    return (dictionary ? dictionary->pair_amount : 0);
}

const char *bemDictionaryGetIndexKeyValue(const bem_dictionary *dictionary, size_t index, const char **key)
{
    if (!dictionary || index >= dictionary->pair_amount || !key)
        return NULL;

    *key = dictionary->pairs[index].key;

    return dictionary->pairs[index].value;
}

const char *bemDictionaryGetKeyValue(const bem_dictionary *dictionary, const char *key)
{
    bem_pair temp, *ptr;

    if (!dictionary || dictionary->pair_amount == 0)
        return NULL;

    temp.key = key;
    temp.value = NULL;

    if ((ptr = (bem_pair *)bsearch(&temp, dictionary->pairs, dictionary->pair_amount, sizeof(bem_pair), (bem_comparison_function)bemComparePairs)) != NULL)
    {
        return ptr->value;
    }

    return NULL;
}

bem_dictionary *bemDictionaryNew(bem_memory_pool *pool)
{
    bem_dictionary *dictionary;

    if ((dictionary = (bem_dictionary *)calloc(1, sizeof(bem_dictionary))) != NULL)
        dictionary->pool = pool;

    return dictionary;
}

void bemDictionaryRemoveKey(bem_dictionary *dictionary, const char *key)
{
    bem_pair temp, *ptr;
    size_t index;

    if (!dictionary || dictionary->pair_amount == 0)
        return;

    temp.key = key;
    temp.value = NULL;

    if ((ptr = (bem_pair *)bsearch(&temp, dictionary->pairs, dictionary->pair_amount, sizeof(bem_pair), (bem_comparison_function)bemComparePairs)) != NULL)
    {
        dictionary->pair_amount--;

        index = (size_t)(ptr - dictionary->pairs);

        if (index < dictionary->pair_amount)
            memmove(ptr, ptr + 1, (dictionary->pair_amount - index) * sizeof(bem_pair));
    }
}

void bemDictionarySetKeyValue(bem_dictionary *dictionary, const char *key, const char *value)
{
    bem_pair temp, *ptr = NULL;

    if (!dictionary)
    {
        return;
    }
    else if (dictionary->pair_amount == 1 && !strcmp(dictionary->pairs[0].key, key))
    {
        ptr = dictionary->pairs;
    }
    else if (dictionary->pair_amount > 1)
    {
        temp.key = key;
        ptr = (bem_pair *)bsearch(&temp, dictionary->pairs, dictionary->pair_amount, sizeof(bem_pair), (bem_comparison_function)bemComparePairs);
    }

    if (ptr)
    {
        ptr->value = bemPoolGetString(dictionary->pool, value);
        return;
    }

    if (dictionary->pair_amount >= dictionary->pairs_size)
    {
        if ((ptr = realloc(dictionary->pairs, (dictionary->pairs_size + 4) * sizeof(bem_pair))) == NULL)
            return;

        dictionary->pairs_size += 4;
        dictionary->pairs = ptr;
    }

    ptr = dictionary->pairs + dictionary->pair_amount;
    dictionary->pair_amount++;

    ptr->key = bemPoolGetString(dictionary->pool, key);
    ptr->value = bemPoolGetString(dictionary->pool, value);

    qsort(dictionary->pairs, dictionary->pair_amount, sizeof(bem_pair), (bem_comparison_function)bemComparePairs);
}

static int bemComparePairs(bem_pair *a, bem_pair *b)
{
    return strcasecmp(a->key, b->key);
}

void bemPoolDelete(bem_memory_pool *pool)
{
    if (pool)
    {
        // TODO: if (pool->font_amount > 0) bemPoolDeleteFonts(pool);

        if (pool->font_amount > 0)
        {
            size_t i;
            char **temp;

            for (i = pool->string_amount, temp = pool->strings; i > 0; i--, temp++)
                free(*temp);

            free(pool->strings);
        }

        free(pool->last_error);
        free(pool);
    }
}

bool bemPoolError(bem_memory_pool *pool, int line_number, const char *message, ...)
{
    bool result;

    va_list ap;
    va_start(ap, message);
    result = bemPoolErrorv(pool, line_number, message, ap);
    va_end(ap);

    return result;
}

bool bemPoolErrorv(bem_memory_pool *pool, int line_number, const char *message, va_list ap)
{
    char buffer[8192];

    vsnprintf(buffer, sizeof(buffer), message, ap);
    free(pool->last_error);
    pool->last_error = strdup(buffer);

    return (pool->error_callback)(pool->error_context, buffer, line_number);
}

const char *bemPoolGetLastError(bem_memory_pool *pool)
{
    return pool ? pool->last_error : NULL;
}

const char *bemPoolGetString(bem_memory_pool *pool, const char *str)
{
    char *news, **temp;

    if (!pool || !str)
    {
        return NULL;
    }
    else if (!*str)
    {
        return "";
    }

    if (pool->string_amount == 1 && !strcmp(pool->strings[0], str))
    {
        return pool->strings[0];
    }
    else if (pool->string_amount > 1)
    {
        if ((temp = bsearch(&str, pool->strings, pool->string_amount, sizeof(char *), (bem_comparison_function)bemCompareStrings)) != NULL)
        {
            return *temp;
        }
    }

    if (pool->string_amount >= pool->strings_size)
    {
        if ((temp = realloc(pool->strings, (pool->strings_size + 32) * sizeof(char *))) == NULL)
            return NULL;

        pool->strings_size += 32;
        pool->strings = temp;
    }

    temp = pool->strings + pool->string_amount;
    *temp = news = strdup(str);
    pool->string_amount++;

    if (pool->string_amount > 1)
        qsort(pool->strings, pool->string_amount, sizeof(char *), (bem_comparison_function)bemCompareStrings);

    return news;
}

const char *bemPoolGetURL(bem_memory_pool *pool, const char *url, const char *base_url)
{
    const char *mapped;
    char *ptr, temp[1024], new_url[1024];

    if (*url == '/')
    {
        if (!base_url)
        {
            return bemPoolGetString(pool, url);
        }
        else if (!strncmp(base_url, "http://", 7))
        {
            strncpy(temp, base_url, sizeof(temp) - 1);
            temp[sizeof(temp) - 1] = '\0';

            if ((ptr = strchr(temp + 7, '/')) != NULL)
                *ptr = '\0';

            snprintf(new_url, sizeof(new_url), "%s%s", temp, url);
            url = new_url;
        }
        else if (!strncmp(base_url, "https://", 8))
        {
            strncpy(temp, base_url, sizeof(temp) - 1);
            temp[sizeof(temp) - 1] = '\0';

            if ((ptr = strchr(temp + 8, '/')) != NULL)
                *ptr = '\0';

            snprintf(new_url, sizeof(new_url), "%s%s", temp, url);
            url = new_url;
        }
        else
        {
            return bemPoolGetString(pool, url);
        }
    }
    else if (strncmp(url, "http://", 7) && strncmp(url, "https://", 8))
    {
        if (!base_url)
        {
            getcwd(temp, sizeof(temp));
            snprintf(new_url, sizeof(new_url), "%s/%s", temp, url);

            return bemPoolGetString(pool, new_url);
        }
        else
        {
            strncpy(temp, base_url, sizeof(temp) - 1);
            temp[sizeof(temp) - 1] = '\0';

            if ((ptr = strrchr(temp, '/')) != NULL)
                *ptr = '\0';

            snprintf(new_url, sizeof(new_url), "%s/%s", temp, url);

            if (new_url[0] == '/')
                return (bemPoolGetString(pool, new_url));

            url = new_url;
        }
    }

    if ((mapped = (pool->url_callback)(pool->url_context, url, temp, sizeof(temp))) != NULL)
    {
        if (!pool->urls)
            pool->urls = bemDictionaryNew(pool);

        bemDictionarySetKeyValue(pool->urls, url, temp);
        mapped = bemPoolGetString(pool, temp);
    }

    return (mapped);
}

bem_memory_pool *bemPoolNew(void)
{
    bem_memory_pool *pool = (bem_memory_pool *)calloc(1, sizeof(bem_memory_pool));

    if (pool)
    {
        if ((pool->locale = localeconv()) != NULL)
        {
            if (!pool->locale->decimal_point || !strcmp(pool->locale->decimal_point, "."))
            {
                pool->locale = NULL;
            }
            else
            {
                pool->locale_decimal_length = strlen(pool->locale->decimal_point);
            }
        }

        pool->error_callback = bemDefaultErrorCallback;
        pool->url_callback = bemDefaultURLCallback;
    }

    return (pool);
}

void bemPoolSetErrorCallback(bem_memory_pool *pool, bem_error_callback callback, void *context)
{
    if (!pool)
        return;

    pool->error_callback = callback ? callback : bemDefaultErrorCallback;
    pool->error_context = context;
}

void bemPoolSetURLCallback(bem_memory_pool *pool, bem_url_callback callback, void *context)
{
    if (!pool)
        return;

    pool->url_callback = callback ? callback : bemDefaultURLCallback;
    pool->url_context = context;
}

static int bemCompareStrings(char **a, char **b)
{
    return (strcmp(*a, *b));
}

void bemImageDelete(bem_image *image)
{
    free(image);
}

const char *bemImageGetFormat(bem_image *image)
{
    return (image ? image->format : NULL);
}

int bemImageGetHeight(bem_image *image)
{
    return (image ? image->height : 0);
}

bem_size bemImageGetSize(bem_image *image)
{
    bem_size size;

    if (image)
    {
        if (image->x_resolution > 0 && image->y_resolution > 0 && image->units != RESOLUTION_NONE)
        {
            if (image->units == RESOLUTION_PER_INCH)
            {
                size.width = 72.0f * image->width / image->x_resolution;
                size.height = 72.0f * image->height / image->y_resolution;
            }
            else
            {
                size.width = 72.0f / 2.54f * image->width / image->x_resolution;
                size.height = 72.0f / 2.54f * image->height / image->y_resolution;
            }
        }
        else
        {
            size.width = 0.72f * image->width;
            size.height = 0.72f * image->height;
        }
    }
    else
    {
        size.width = 0.0f;
        size.height = 0.0f;
    }

    return (size);
}

int bemImageGetWidth(bem_image *image)
{
    return (image ? image->width : 0);
}

int main(int argc, char *argv[])
{
    // TODO: Fix compiler warnings
    return 0;
}
