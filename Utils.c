
#include "Utils.h"

void save(Line* head, FILE* file)
{
	do {
		int i = 0;
		do {
			char c = head->buffer[i++];
			fputc(c, file);
		} while (i < head->num_chars);
		head = head->tail;
	} while (head != NULL);
}

Line* load(FILE* file)
{
	Line* head = malloc(sizeof(Line));

	if (head == NULL) {
		fprintf(stderr, "Could not allocate enough memory to program.");
	}

	initLine(head);

	char c;

	do {
		do {
			c = fgetc(file);

			if (c == EOF) {
				// handle the final line which may not have a newline at the end.
				while (head->head != NULL) {
					head = head->head;
				}

				return head;
			}

			head->buffer[head->num_chars++] = c;
		} while (c != '\n');

		Line* line = malloc(sizeof(Line));

		if (line == NULL) {
			fprintf(stderr, "Could not allocate enough memory to program.");
		}

		initLine(line);

		head->tail = line;
		line->head = head;
		head = line;

	} while (c != EOF);

	while (head->head != NULL) {
		head = head->head;
	}

	return head;
}