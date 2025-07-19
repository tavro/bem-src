// Needed for strdup on linux
#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

HTMLNode* parser(char* inputString) {
	int inputStringLength = strlen(inputString);
	int start = 0, end = 0;
	int i;

	char tagName[32] = {0};
	int tagStart = 1, tagEnd = 0;

	for (i = tagStart; i < inputStringLength; i++) {
		if (inputString[i] == '>') {
			tagEnd = i - 1;
			break;
		}
	}

	strncpy(tagName, &inputString[tagStart], tagEnd - tagStart + 1);
	tagName[tagEnd - tagStart + 1] = '\0';
	start = i + 1;

	// remove blank spaces
	while (inputString[start] == ' ') {
		start++;
	}

	for (i = start; i < inputStringLength; i++) {
		if (inputString[i] == '<') {
			end = i - 1;
			break;
		}
	}

	char* content = (char*)malloc(end - start + 2);
	strncpy(content, &inputString[start], end - start + 1);
	content[end - start + 1] = '\0';

	HTMLNode* document = createDocument();
	HTMLNode* element = createElement(tagName);
	HTMLNode* text = createText(content);
	appendChild(element, text);
	appendChild(document, element);

	free(content);
	return document;
}

void printNode(HTMLNode* node, int indent) {
	for (int i = 0; i < indent; i++) printf("	");

	switch(node->type) {
		case DOCUMENT:
			printf("Document\n");
			break;
		case ELEMENT:
			printf("Element: <%s>\n", node->name);
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
	char input[] = "<p>This is a test string</p>";

	HTMLNode* DOM = parser(input);
 	printNode(DOM, 0);
	freeNodeTree(DOM);

	return 0;
}
