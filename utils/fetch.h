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

#ifndef FETCH_H
#define FETCH_H

#include <stdlib.h>

struct MemoryStruct {
	char *memory;
	size_t size;
};

#define MAX_CSS_LINKS 32

static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

int downloadURL(const char *url, struct MemoryStruct *chunk);

char *extractInlineCSS(const char *html);

int extractCSSLinks(const char *html, char links[][1024], int maxLinks);

char *strAppend(char *base, const char *append);

void resolveURL(const char *baseURL, const char *link, char *outURL, size_t maxLen);

int fetchHTMLAndCSS(const char *url, char **htmlContent, char **cssContent);

char *extractMetaData(char **html);

#endif
