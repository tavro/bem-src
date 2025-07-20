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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
	DOCUMENT,
	ELEMENT,
	TEXT,
	COMMENT
} HTMLNodeType;

typedef struct HTMLAttribute {
	char* name;
	char* value;

	struct HTMLAttribute* next;
} HTMLAttribute;

typedef struct HTMLNode {
	HTMLNodeType type;

	char* name;
	char* content;

	HTMLAttribute* attributes;

	struct HTMLNode** children;
	size_t childCount;
	size_t childCapacity;

	struct HTMLNode* parent;
} HTMLNode;

HTMLNode* createNode(HTMLNodeType type) {
	HTMLNode* node = (HTMLNode*)malloc(sizeof(HTMLNode));

	node->type = type;
	node->name = NULL;
	node->content = NULL;
	node->attributes = NULL;
	node->children = NULL;
	node->childCount = 0;
	node->childCapacity = 0;
	node->parent = NULL;

	return node;
}

HTMLNode* createElement(const char* name) {
	HTMLNode* node = createNode(ELEMENT);

	node->name = strdup(name);
	
	return node;
}

HTMLNode* createText(const char* text) {
	HTMLNode* node = createNode(TEXT);

	node->content = strdup(text);

	return node;
}

HTMLNode* createDocument() {
	return createNode(DOCUMENT);
}

void appendChild(HTMLNode* parent, HTMLNode* child) {
	if (parent->childCapacity == 0) {
		parent->childCapacity = 4;
		parent->children = (HTMLNode**)malloc(sizeof(HTMLNode*) * parent->childCapacity);
	} else if (parent->childCount >= parent->childCapacity) {
		parent->childCapacity *= 2;
		parent->children = (HTMLNode**)realloc(parent->children, sizeof(HTMLNode*) * parent->childCapacity);
	}

	parent->children[parent->childCount++] = child;
	child->parent = parent;
}

void addAttribute(HTMLNode* node, const char* name, const char* value) {
	HTMLAttribute* attribute = (HTMLAttribute*)malloc(sizeof(HTMLAttribute));

	attribute->name = strdup(name);
	attribute->value = strdup(value);
	attribute->next = node->attributes;

	node->attributes = attribute;
}

void freeNodeTree(HTMLNode* node) {
	if (!node) return;

	free(node->name);
	free(node->content);

	HTMLAttribute* attribute = node->attributes;
	while(attribute) {
		HTMLAttribute* next = attribute->next;

		free(attribute->name);
		free(attribute->value);
		free(attribute);

		attribute = next;
	}

	for (size_t i = 0; i < node->childCount; i++) {
		freeNodeTree(node->children[i]);
	}

	free(node->children);
	free(node);
}

char* skipWhitespace(char* s) {
	while (isspace(*s)) s++;
	return s;
}

char* parseTagName(char* s, char* tagName) {
	s = skipWhitespace(s);

	int i = 0;
	while (*s && (isalnum(*s) || *s == '-' || *s == '_')) {
		tagName[i++] = *s++;
	}
	tagName[i] = '\0';

	return s;
}

char* parseAttributes(char* s, HTMLNode* node) {
	s = skipWhitespace(s);

	while (*s && *s != '>' && *s != '/') {
		char attributeName[64], attributeValue[256];

		int i = 0;
		while (*s && *s != '=' && !isspace(*s)) {
			attributeName[i++] = *s++;
		}
		attributeName[i] = '\0';

		s = skipWhitespace(s);
		if (*s == '=') {
			s++;
			s = skipWhitespace(s);
			if (*s == '"' || *s == '\'') {
				char quote = *s++;
				i = 0;
				while (*s && *s != quote) {
					attributeValue[i++] = *s++;
				}
				attributeValue[i] = '\0';

				if (*s == quote) s++;
				addAttribute(node, attributeName, attributeValue);
			}
		}

		s = skipWhitespace(s);
	}

	return s;
}

char* parseNode(char* s, HTMLNode* parent) {
	s = skipWhitespace(s);

	while (*s) {
		if (strncmp(s, "</", 2) == 0) { // end tag
			while (*s && *s != '>') s++;
			if (*s == '>') s++;
			return s;
		} else if (*s == '<') { // start tag
			s++;
			if (*s == '!') { // skip comments
				while (*s && *s != '>') s++;
				if (*s == '>') s++;
				continue;
			}

			char tagName[64];
			s = parseTagName(s, tagName);

			HTMLNode* node = createElement(tagName);
			s = parseAttributes(s, node);

			bool selfClosing = false;
			if (*s == '/') {
				selfClosing = true;
				s++;
			}
			if (*s == '>') s++;

			appendChild(parent, node);

			if (!selfClosing) {
				s = parseNode(s, node);
			}
		} else { // text
			char text[1024];
			int i = 0;
			while (*s && *s != '<') {
				text[i++] = *s++;
			}
			text[i] = '\0';

			char* trimmed = skipWhitespace(text);
			if (*trimmed != '\0') {
				HTMLNode* textNode = createText(trimmed);
				appendChild(parent, textNode);
			}
		}
	}

	return s;
}

HTMLNode* parser(char* inputString) {
	HTMLNode* document = createDocument();
	parseNode(inputString, document);
	return document;
}

void printNode(HTMLNode* node, int indent) {
	for (int i = 0; i < indent; i++) printf("	");

	switch(node->type) {
		case DOCUMENT:
			printf("Document\n");
			break;
		case ELEMENT:
			printf("Element: <%s", node->name);
			HTMLAttribute* attribute = node->attributes;
			while (attribute) {
				printf(" %s=\"%s\"", attribute->name, attribute->value);
				attribute = attribute->next;
			}
			printf(">\n");
			break;
		case TEXT:
			printf("Text: \"%s\"\n", node->content);
			break;
		case COMMENT:
			printf("Text: %s\n", node->content);
			break;
	}

	for (size_t i = 0; i < node->childCount; i++) {
		printNode(node->children[i], indent + 1);
	}
}

int main() {
	char input[] = "<div id=\"main\" class=\"container\"><p>Hello <b>world</b>!</p></div><br/>";

	HTMLNode* DOM = parser(input);
 	printNode(DOM, 0);
	freeNodeTree(DOM);

	return 0;
}
