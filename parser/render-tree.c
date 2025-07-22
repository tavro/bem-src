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

#include "html-parser.h"
#include "css-parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct RenderNode {
	HTMLNode *domNode;
	CSSDeclaration *style;
	struct RenderNode **children;
	size_t childCount;
} RenderNode;

int hasAttribute(HTMLNode *node, const char *attrName, const char *expectedValue, int allowPartialMatch) {
	for (HTMLAttribute *attr = node->attributes; attr; attr = attr->next) {
		if (strcmp(attr->name, attrName) == 0) {
			if (allowPartialMatch) {
				const char *val = attr->value;
				const char *found = strstr(val, expectedValue);
				while (found) {
					int isStart = (found == val || found[-1] == ' ');
					int isEnd = (found[strlen(expectedValue)] == '\0' || found[strlen(expectedValue)] == ' ');
					if (isStart && isEnd) return 1;
					found = strstr(found + 1, expectedValue);
				}

				return 0;
			} else {
				return strcmp(attr->value, expectedValue) == 0;
			}
		}
	}

	return 0;
}

int matchSelector(const char *selector, HTMLNode *node) {
	if (!selector || !node || node->type != ELEMENT || !node->name) return 0;

	if (selector[0] == '#') {
		return hasAttribute(node, "id", selector + 1, 0);
	} else if (selector[0] == '.') {
		return hasAttribute(node, "class", selector + 1, 1);
	}

	return strcmp(selector, node->name) == 0;
}

CSSDeclaration* computeStyle(HTMLNode *node, CSSOM *cssom, int *outCount) {
	CSSDeclaration *result = NULL;
	CSSDeclaration **tail = &result;
	int count = 0;

	for (CSSRule *rule = cssom->rules; rule; rule = rule->next) {
		if (matchSelector(rule->selector, node)) {
			for (CSSDeclaration *declaration = rule->declarations; declaration; declaration = declaration->next) {
				CSSDeclaration *copy = malloc(sizeof(CSSDeclaration));
				copy->property = strdup(declaration->property);
				copy->value = strdup(declaration->value);
				copy->next = NULL;

				*tail = copy;
				tail = &copy->next;
				count++;
			}
		}
	}

	if (outCount) *outCount = count;
	return result;
}

RenderNode* buildRenderTreeRecursive(HTMLNode *node, CSSOM *cssom) {
	if (!node) return NULL;

	RenderNode *renderNode = malloc(sizeof(RenderNode));
	renderNode->domNode = node;
	renderNode->style = computeStyle(node, cssom, NULL);
	renderNode->childCount = node->childCount;
	renderNode->children = malloc(sizeof(RenderNode*) * node->childCount);

	for (size_t i = 0; i < node->childCount; i++) {
		renderNode->children[i] = buildRenderTreeRecursive(node->children[i], cssom);
	}

	return renderNode;
}

RenderNode* buildRenderTree(HTMLNode *dom, CSSOM *cssom) {
	return buildRenderTreeRecursive(dom, cssom);
}

void printIndent(int depth) {
	for (int i = 0; i < depth; i++) {
		printf("    ");
	}
}

void printStyle(CSSDeclaration *style) {
	for (CSSDeclaration *declaration = style; declaration; declaration = declaration->next) {
		printf("%s: %s; ", declaration->property, declaration->value);
	}
}

void printRenderTree(RenderNode *node, int depth) {
	if (!node) return;
	printIndent(depth);

	if (node->domNode->name)
		printf("<%s>", node->domNode->name);
	else if (node->domNode->type == DOCUMENT) 
		printf("RENDER TREE:");
	else
		printf("\"%s\"", node->domNode->content);

	if (node->style) {
		printf("[ style: ");
		printStyle(node->style);
		printf("]");
	}

	printf("\n");

	for (size_t i = 0; i < node->childCount; i++) {
		printRenderTree(node->children[i], depth + 1);
	}
}

void freeRenderTree(RenderNode *node) {
	if (!node) return;

	for (size_t i = 0; i < node->childCount; i++) {
		freeRenderTree(node->children[i]);
	}

	free(node->children);
	free(node);
}

int main() {
	char htmlInput[] = "<body><div id=\"main\" class=\"container test\"><p>Hello <b class=\"test\">world</b>!</p><p>Hello world <b>again</b>!</p></div><br/></body>";
	char cssInput[] = ".test { color: red; } #main { color: black; } .container { font-size: 16px; } body { color: black; background-color: white; } div { width: 50%; margin: auto; } p { font-size: 24px; }";

	HTMLNode* DOM = parser(htmlInput);
 	// printNode(DOM, 0);

	CSSOM *cssom = parseCSS(cssInput);
	// printCSSOM(cssom);
	
	RenderNode *renderTree = buildRenderTree(DOM, cssom);
	printRenderTree(renderTree, 0);

	freeRenderTree(renderTree);
	freeNodeTree(DOM);
	freeCSSOM(cssom);

	return 0;
}
