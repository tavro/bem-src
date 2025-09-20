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

bem_image *bemImageNew(bem_memory_pool *pool, bem_file *file)
{
    bem_image *image;
    unsigned char buffer[2048];
    size_t bytes;

    if (!pool || !file)
        return NULL;

    if ((image = (bem_image *)calloc(1, sizeof(bem_image))) == NULL)
        return NULL;

    image->pool = pool;

    // TODO: bytes = bemFileRead(file, buffer, sizeof(buffer));

    if (bytes > 27 && !memcmp(buffer, "\211PNG\015\012\032\012\000\000\000\015IHDR", 16))
    {
        image->format = "image/png";
        image->width = (buffer[16] << 24) | (buffer[17] << 16) | (buffer[18] << 8) | buffer[19];
        image->height = (buffer[20] << 24) | (buffer[21] << 16) | (buffer[22] << 8) | buffer[23];
    }
    else if (bytes > 12 && (!memcmp(buffer, "GIF87a", 6) || !memcmp(buffer, "GIF89a", 6)))
    {
        image->format = "image/gif";
        image->width = (buffer[7] << 8) | buffer[6];
        image->height = (buffer[9] << 8) | buffer[8];
    }
    else if (bytes > 3 && !memcmp(buffer, "\377\330\377", 3))
    {
        unsigned char *buffer_ptr = buffer + 2, *buffer_end = buffer + bytes;
        size_t length;

        image->format = "image/jpeg";

        while (buffer_ptr < buffer_end)
        {
            if (*buffer_ptr == 0xff)
            {
                buffer_ptr++;

                if (buffer_ptr >= buffer_end)
                {
                    /*
                    TODO:
                    if ((bytes = bemFileRead(file, buffer, sizeof(buffer))) == 0)
                        break;
                    */

                    buffer_ptr = buffer;
                    buffer_end = buffer + bytes;
                }

                if (*buffer_ptr == 0xff)
                    continue;

                if ((buffer_ptr + 16) >= buffer_end)
                {
                    bytes = (size_t)(buffer_end - buffer_ptr);

                    memmove(buffer, buffer_ptr, bytes);
                    buffer_ptr = buffer;
                    buffer_end = buffer + bytes;

                    /*
                    TODO:
                    if ((bytes = bemFileRead(file, buffer_end, sizeof(buffer) - bytes)) == 0)
                        break;
                    */

                    buffer_end += bytes;
                }
                length = (size_t)((buffer_ptr[1] << 8) | buffer_ptr[2]);

                if (*buffer_ptr == 0xe0 && length >= 16 && !memcmp(buffer_ptr + 3, "JFIF", 5))
                {
                    if (buffer_ptr[10] == 1)
                    {
                        image->units = RESOLUTION_PER_INCH;
                        image->x_resolution = (buffer_ptr[11] << 8) | buffer_ptr[12];
                        image->y_resolution = (buffer_ptr[13] << 8) | buffer_ptr[14];
                    }
                    else if (buffer_ptr[10] == 2)
                    {
                        image->units = RESOLUTION_PER_CM;
                        image->x_resolution = (buffer_ptr[11] << 8) | buffer_ptr[12];
                        image->y_resolution = (buffer_ptr[13] << 8) | buffer_ptr[14];
                    }
                }
                else if ((*buffer_ptr >= 0xc0 && *buffer_ptr <= 0xc3) ||
                         (*buffer_ptr >= 0xc5 && *buffer_ptr <= 0xc7) ||
                         (*buffer_ptr >= 0xc9 && *buffer_ptr <= 0xcb) ||
                         (*buffer_ptr >= 0xcd && *buffer_ptr <= 0xcf))
                {

                    image->width = (buffer_ptr[6] << 8) | buffer_ptr[7];
                    image->height = (buffer_ptr[4] << 8) | buffer_ptr[5];
                    break;
                }

                buffer_ptr++;
                bytes = (size_t)(buffer_end - buffer_ptr);

                while (length >= bytes)
                {
                    length -= bytes;

                    /*
                    TODO:
                    if ((bytes = bemFileRead(file, buffer, sizeof(buffer))) == 0)
                        break;
                    */

                    buffer_ptr = buffer;
                    buffer_end = buffer + bytes;
                }

                if (length > bytes)
                    break;

                buffer_ptr += length;
            }
        }

        if (image->width == 0 || image->height == 0)
        {
            free(image);
            return NULL;
        }
    }
    else
    {
        free(image);
        return NULL;
    }

    return (image);
}

void bemFileDelete(bem_file *file)
{
    if (!file)
        return;

    if (file->file_pointer)
        gzclose(file->file_pointer);

    free(file);
}

bool bemFileError(bem_file *file, const char *message, ...)
{
    bool result;
    char sane_url[1024], *sane_pointer, temp[1024];
    const char *url_pointer;
    va_list ap;

    if (file->url)
    {
        for (sane_pointer = sane_url, url_pointer = file->url; *url_pointer && sane_pointer < (sane_url + sizeof(sane_url) - 1); url_pointer++)
        {
            if (*url_pointer < ' ' || *url_pointer == '%')
            {
                *sane_pointer++ = '_';
            }
            else
            {
                *sane_pointer++ = *url_pointer;
            }
        }
        *sane_pointer = '\0';

        if (file->line_number)
        {
            snprintf(temp, sizeof(temp), "%s:%d: %s", sane_url, file->line_number, message);
        }
        else if (file->url)
        {
            snprintf(temp, sizeof(temp), "%s: %s", sane_url, message);
        }
        message = temp;
    }
    else if (file->line_number)
    {
        snprintf(temp, sizeof(temp), "%d: %s", file->line_number, message);
        message = temp;
    }

    va_start(ap, message);
    result = bemPoolErrorv(file->pool, file->line_number, message, ap);
    va_end(ap);

    return result;
}

int bemFileGetc(bem_file *file)
{
    int ch;

    if (file->buffer_pointer)
    {
        if (file->buffer_pointer < file->buffer_end)
        {
            ch = (int)*(file->buffer_pointer)++;
        }
        else
        {
            ch = EOF;
        }
    }
    else
    {
        ch = gzgetc(file->file_pointer);
    }

    if (ch == '\n')
        file->line_number++;

    return ch;
}

bem_file *bemFileNewBuffer(bem_memory_pool *pool, const void *buffer, size_t bytes)
{
    bem_file *file;

    if ((file = calloc(1, sizeof(bem_file))) != NULL)
    {
        file->pool = pool;
        file->buffer = buffer;
        file->buffer_pointer = file->buffer;
        file->buffer_end = file->buffer + bytes;
        file->line_number = 1;
    }

    return file;
}

bem_file *bemFileNewString(bem_memory_pool *pool, const char *str)
{
    return bemFileNewBuffer(pool, str, strlen(str));
}

bem_file *bemFileNewURL(bem_memory_pool *pool, const char *url, const char *base_url)
{
    bem_file *file;
    const char *file_name;

    if ((file_name = bemPoolGetURL(pool, url, base_url)) == NULL)
        return NULL;

    if ((file = calloc(1, sizeof(bem_file))) != NULL)
    {
        file->pool = pool;
        file->url = file_name;
        file->file_pointer = gzopen(file_name, "rb");
        file->line_number = 1;

        if (!file->file_pointer)
        {
            perror(file_name);
            free(file);
            file = NULL;
        }
    }

    return file;
}

size_t bemFileRead(bem_file *file, void *buffer, size_t bytes)
{
    ssize_t read_bytes;

    if (!file || !buffer || bytes == 0)
        return 0;

    if (file->buffer_pointer)
    {
        if ((size_t)(file->buffer_end - file->buffer_pointer) < bytes)
            bytes = (size_t)(file->buffer_end - file->buffer_pointer);

        if (bytes > 0)
        {
            memcpy(buffer, file->buffer_pointer, bytes);
            file->buffer_pointer += bytes;
        }

        return bytes;
    }
    else if ((read_bytes = gzread(file->file_pointer, buffer, (unsigned)bytes)) < 0)
    {
        return 0;
    }

    return (size_t)read_bytes;
}

size_t bemFileSeek(bem_file *file, size_t offset)
{
    ssize_t seek_offset;

    if (!file)
        return 0;

    if (file->buffer_pointer)
    {
        if (offset > (size_t)(file->buffer_end - file->buffer))
            offset = (size_t)(file->buffer_end - file->buffer);

        file->buffer_pointer = file->buffer + offset;

        return offset;
    }

    if ((seek_offset = gzseek(file->file_pointer, (long)offset, SEEK_SET)) < 0)
        return 0;

    return (size_t)seek_offset;
}

void bemFileUngetc(bem_file *file, int ch)
{
    if (file->buffer_pointer && file->buffer_pointer > file->buffer)
    {
        file->buffer_pointer--;
    }
    else if (file->file_pointer)
    {
        gzungetc(ch, file->file_pointer);
    }

    if (ch == '\n')
        file->line_number--;
}

int main(int argc, char *argv[])
{
    // TODO: Fix compiler warnings
    return 0;
}
