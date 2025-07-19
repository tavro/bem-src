#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void parser(char* inputString) {
	int inputStringLength = strlen(inputString);
	int start = 0, end = 0;
	int i, j;

	for (i = 0; i < inputStringLength; i++) {
		if (inputString[i] == '>') {
			start = i + 1;
			break;
		}
	}

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

	for (j = start; j <= end; j++) {
		printf("%c", inputString[j]);
	}

	printf("\n");
}

int main() {
	char input[] = "<p>This is a test string</p>";
	parser(input);
	return 0;
}
