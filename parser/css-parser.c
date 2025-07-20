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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

char *strStrip(char *str) {
	while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}

char *subStr(const char *src, int start, int end) {
	int len = end - start;
	char *dest = malloc(len + 1);

	strncpy(dest, src + start, len);
	dest[len] = '\0';

	return dest;
}

CSSDeclaration *parseDeclarations(const char *block) {
	CSSDeclaration *head = NULL, *tail = NULL;

	const char *p = block;
	while (*p) {
		while (isspace(*p)) p++;
        const char *propStart = p;
        while (*p && *p != ':' && *p != '}') p++;
        if (*p != ':') break;

        int propLen = p - propStart;
        char *rawProp = subStr(propStart, 0, propLen);
        char *prop = strdup(strStrip(rawProp));
        free(rawProp);

        p++;
        const char *valStart = p;
        while (*p && *p != ';' && *p != '}') p++;
        int valLen = p - valStart;
        char *rawVal = subStr(valStart, 0, valLen);
        char *val = strdup(strStrip(rawVal));
        free(rawVal);

        CSSDeclaration *declaration = malloc(sizeof(CSSDeclaration));
        declaration->property = prop;
        declaration->value = val;
        declaration->next = NULL;

        if (!head) head = declaration;
        else tail->next = declaration;
        tail = declaration;

        if (*p == ';') p++;
	}

	return head;
}

CSSOM *parseCSS(const char *css) {
	CSSOM *cssom = malloc(sizeof(CSSOM));
	cssom->rules = NULL;

	CSSRule *lastRule = NULL;

	const char *p = css;
	while (*p) {
		while (isspace(*p)) p++;
        const char *selectorStart = p;
        while (*p && *p != '{') p++;
        if (*p == '\0') break;

        char *rawSel = subStr(selectorStart, 0, p - selectorStart);
        char *selector = strdup(strStrip(rawSel));
        free(rawSel);

        p++;
        const char *blockStart = p;
        while (*p && *p != '}') p++;
        if (*p == '\0') break;

        char *block = subStr(blockStart, 0, p - blockStart);
        CSSDeclaration *declarations = parseDeclarations(block);
        free(block);

        CSSRule *rule = malloc(sizeof(CSSRule));
        rule->selector = selector;
        rule->declarations = declarations;
        rule->next = NULL;

        if (!cssom->rules) cssom->rules = rule;
        else lastRule->next = rule;
        lastRule = rule;

        if (*p == '}') p++;
	}

	return cssom;
}

void printCSSOM(CSSOM *cssom) {
	CSSRule *rule = cssom->rules;
	while (rule) {
		printf("Selector: %s\n", rule->selector);

		CSSDeclaration *declaration = rule->declarations;
		while (declaration) {
			printf("  %s: %s;\n", declaration->property, declaration->value);
			declaration = declaration->next;
		}
		rule = rule->next;
		printf("\n");
	}
}

void freeCSSOM(CSSOM *cssom) {
	CSSRule *rule = cssom->rules;
	while (rule) {
		CSSDeclaration *declaration = rule->declarations;
		while (declaration) {
			CSSDeclaration *next = declaration->next;

			free(declaration->property);
			free(declaration->value);
			free(declaration);

			declaration = next;
		}

		CSSRule *next = rule->next;
		free(rule->selector);
		free(rule);
		rule = next;
	}

	free(cssom);
}

int main() {
	char input[] = "body { color: black; background-color: white; } h1 { font-size: 24px; font-weight: bold; }";

	CSSOM *cssom = parseCSS(input);
	printCSSOM(cssom);
	freeCSSOM(cssom);

	return 0;
}
