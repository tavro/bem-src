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

int main(int argc, char *argv[])
{
    return 0;
}
