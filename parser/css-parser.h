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

#ifndef CSS_PARSER_H
#define CSS_PARSER_H

typedef struct CSSDeclaration {
	char *property;
	char *value;
	struct CSSDeclaration *next;
} CSSDeclaration;

typedef struct CSSRule {
	char *selector;
	CSSDeclaration *declarations;
	struct CSSRule *next;
} CSSRule;

typedef struct CSSOM {
	CSSRule *rules;
} CSSOM;

char *strStrip(char *str);
char *subStr(const char *src, int start, int end);

CSSDeclaration *parseDeclarations(const char *block);
CSSOM *parseCSS(const char *css);

void printCSSOM(CSSOM *cssom);
void freeCSSOM(CSSOM *cssom);

#endif