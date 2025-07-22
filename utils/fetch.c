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

// Needed for strdup on linux
#define _POSIX_C_SOURCE 200809L

#include "fetch.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h> // TODO: Implement my own cURL at some point

static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	char *ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL) {
		printf("Not enough memory\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

int downloadURL(const char *url, struct MemoryStruct *chunk) {
	CURL *curlHandle;
	CURLcode res;

	chunk->memory = malloc(1);
	chunk->size = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curlHandle = curl_easy_init();
	if (!curlHandle) {
		fprintf(stderr, "curl_easy_init() failed\n");
		return 1;
	}

	curl_easy_setopt(curlHandle, CURLOPT_URL, url);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void *)chunk);
	curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	res = curl_easy_perform(curlHandle);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curlHandle);
		return 2;
	}

	curl_easy_cleanup(curlHandle);
	curl_global_cleanup();
	return 0;
}

char *extractInlineCSS(const char *html) {
	const char *pos = html;
	size_t total_len = 0;
	char *css = NULL;

	while ((pos = strstr(pos, "<style")) != NULL) {
		const char *start = strstr(pos, ">");
		if (!start) break;
		start++;

		const char *end = strstr(start, "</style>");
		if (!end) break;

		size_t len = end - start;
		css = realloc(css, total_len + len + 1);
		memcpy(css + total_len, start, len);
		total_len += len;
		css[total_len] = '\0';

		pos = end + 8;
	}

	return css;
}

int extractCSSLinks(const char *html, char links[][1024], int maxLinks) {
	int count = 0;
	const char *pos = html;

	while ((pos = strstr(pos, "<link")) != NULL && count < maxLinks) {
		const char *rel = strstr(pos, "rel=");
		if (!rel) {
			pos += 5;
			continue;
		}

		if (strstr(rel, "stylesheet") == NULL) {
			pos += 5;
			continue;
		}

		const char *href = strstr(pos, "href=");
		if (!href) {
			pos += 5;
			continue;
		}

		const char *startQuote = strchr(href, '"');
		if (!startQuote) {
			pos += 5;
			continue;
		}
		startQuote++;

		const char *endQuote = strchr(startQuote, '"');
		if (!endQuote) {
			pos += 5;
			continue;
		}

		size_t len = endQuote - startQuote;
		if (len > 1023) len = 1023;
		strncpy(links[count], startQuote, len);
		links[count][len] = 0;

		count++;
		pos = endQuote + 1;
	}

	return count;
}

char *strAppend(char *base, const char *append) {
	size_t baseLen = base ? strlen(base) : 0;
	size_t appendLen = append ? strlen(append) : 0;

	char *newStr = realloc(base, baseLen + appendLen + 1);
	if (!newStr) {
		free(base);
		return NULL;
	}

	memcpy(newStr + baseLen, append, appendLen);
	newStr[baseLen + appendLen] = '\0';

	return newStr;
}

void resolveURL(const char *baseURL, const char *link, char *outURL, size_t maxLen) {
	if (strncmp(link, "http://", 7) == 0 || strncmp(link, "https://", 8) == 0) {
		strncpy(outURL, link, maxLen);
		outURL[maxLen - 1] = 0;
	} else {
		const char *lastSlash = strrchr(baseURL, '/');
		if (!lastSlash) {
			strncpy(outURL, baseURL, maxLen);
			outURL[maxLen - 1] = 0;
			return;
		}

		size_t baseLen = lastSlash - baseURL + 1;
		if (baseLen + strlen(link) >= maxLen) {
			outURL[0] = 0;
			return;
		}

		strncpy(outURL, baseURL, baseLen);
		strcpy(outURL + baseLen, link);
	}
}

int fetchHTMLAndCSS(const char *url, char **htmlContent, char **cssContent) {
	struct MemoryStruct htmlChunk;

	int res = downloadURL(url, &htmlChunk);
	if (res != 0) {
		fprintf(stderr, "Failed to download HTML from %s\n", url);
		return res;
	}

	*htmlContent = htmlChunk.memory;
	char *inlineCSS = extractInlineCSS(*htmlContent);
	char cssLinks[MAX_CSS_LINKS][1024];
	int numLinks = extractCSSLinks(*htmlContent, cssLinks, MAX_CSS_LINKS);

	char *allCSS = NULL;
	if (inlineCSS) {
		allCSS = strdup(inlineCSS);
		free(inlineCSS);
	} else {
		allCSS = strdup("");
	}

	for (int i = 0; i < numLinks; i++) {
		char cssURL[2048];
		resolveURL(url, cssLinks[i], cssURL, sizeof(cssURL));

		struct MemoryStruct cssChunk;
		int cssRes = downloadURL(cssURL, &cssChunk);
		if (cssRes == 0) {
			allCSS = strAppend(allCSS, cssChunk.memory);
			free(cssChunk.memory);
		}
	}
	*cssContent = allCSS;

	return 0;
}

char *extractMetaData(char **html) {
	const char *startHead = strstr(*html, "<head");
	if (!startHead) return NULL;

	const char *startContent = strchr(startHead, '>');
	if (!startContent) return NULL;
	startContent++;

	const char *endHead = strstr(startContent, "</head>");
	if (!endHead) return NULL;

	size_t headLen = endHead - startContent;
	char *headContent = strndup(startContent, headLen);
	if (!headContent) return NULL;

	char *meta = malloc(1);
	meta[0] = '\0';
	size_t metaLen = 0;

	const char *pos = headContent;
	while (*pos) {
		const char *styleTag = strstr(pos, "<style");
		const char *linkTag = strstr(pos, "<link");

		const char *nextTag = NULL;
		if (styleTag && (!linkTag || styleTag < linkTag)) {
			nextTag = styleTag;
		} else if (linkTag) {
			nextTag = linkTag;
		}

		if (nextTag) {
			size_t chunkLen = nextTag - pos;
			meta = realloc(meta, metaLen + chunkLen + 1);
			memcpy(meta + metaLen, pos, chunkLen);
			metaLen += chunkLen;
			meta[metaLen] = '\0';

			if (nextTag == styleTag) {
				const char *endStyle = strstr(nextTag, "</style>");
				if (endStyle) pos = endStyle + 8;
				else break;
			} else {
				const char *endLink = strchr(nextTag, '>');
				if (endLink) pos = endLink + 1;
				else break;
			}
		} else {
			size_t remaining = strlen(pos);
			meta = realloc(meta, metaLen + remaining + 1);
			memcpy(meta + metaLen, pos, remaining);
			metaLen += remaining;
			meta[metaLen] = '\0';
			break;
		}
	}

	free(headContent);

	size_t preLen = startHead - *html;
	const char *afterHead = strstr(endHead, ">");
	if (!afterHead) afterHead = endHead + 7;

	afterHead++;
	size_t postLen = strlen(afterHead);

	char *newHTML = malloc(preLen + postLen + 1);
	memcpy(newHTML, *html, preLen);
	memcpy(newHTML + preLen, afterHead, postLen);
	newHTML[preLen + postLen] = '\0';

	free(*html);
	*html = newHTML;

	return meta;
}
