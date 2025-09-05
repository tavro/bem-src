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
        // TODO: ptr->value = bemPoolGetString(dictionary->pool, value);
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

    // TODO: ptr->key = bemPoolGetString(dictionary->pool, key);
    // TODO: ptr->value = bemPoolGetString(dictionary->pool, value);

    qsort(dictionary->pairs, dictionary->pair_amount, sizeof(bem_pair), (bem_comparison_function)bemComparePairs);
}

static int bemComparePairs(bem_pair *a, bem_pair *b)
{
    return strcasecmp(a->key, b->key);
}

int main(int argc, char *argv[])
{
    return 0;
}
