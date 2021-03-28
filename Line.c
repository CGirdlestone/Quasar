
#include "Line.h"
#include <string.h>

void initLine(Line* line)
{
	line->head = NULL;
	line->tail = NULL;
	line->num_chars = 0;
	for (int i = 0; i < MAX_LINE_LENGTH; i++) {
		line->buffer[i] = '\0';
	}
}

void insertLine(Line* head)
{
	Line* line = (Line*)malloc(sizeof(Line));

	if (line == NULL) {
		fprintf(stderr, "Couldn't allocate enough memory to program.");
		exit(1);
	}

	initLine(line);

	Line* tail = head->tail;

	if (tail == NULL) {
		line->head = head;
		head->tail = line;
	}
	else {
		line->head = head;
		line->tail = head->tail;
		head->tail = line;
		tail->head = line;
	}
}

void deleteLine(Line* line)
{
	Line* head = line->head;
	Line* tail = line->tail;

	if (tail == NULL) {
		head->tail = NULL;
		freeLine(line);
	}
	else {
		head->tail = tail;
		tail->head = head;
		freeLine(line);
	}
}

void addChar(Editor* editor, Line* line, char c, int buffer_index)
{
	if (line->num_chars < MAX_LINE_LENGTH) {
		if (c == '\n') {
			// split line and copy over everything to the right of the buffer index if buffer_index != line->num_chars
			if (buffer_index < line->num_chars) {
				splitLine(line, buffer_index);
				line->buffer[buffer_index] = c;
				line->num_chars++;
				editor->y++;
				editor->x = 0;
			}
			else {
				line->buffer[buffer_index] = c;
				line->num_chars++;
				insertLine(line);
				editor->y++;
				editor->x = 0;
			}
		}
		else {
			// insert the character and shift everything to the right of the buffer index
			// if the line->num_chars is then equal to MAX_LINE_LENGTH, insert a new line
			shiftCharsRight(line, buffer_index, 1);
			line->buffer[buffer_index] = c;
			line->num_chars++;
			if (line->num_chars == MAX_LINE_LENGTH - 1) {
				line->buffer[line->num_chars] = '\n';
				insertLine(line);
				editor->y++;
				editor->x = 0;
			}
		}
	}
}

void mergePrevious(Line* line)
{
	Line* head = line->head;
	if (line->num_chars == 0) { return; }

	int j = 0;
	for (int i = head->num_chars - 1; i < MAX_LINE_LENGTH; i++) {
		head->buffer[i] = line->buffer[j++];
		head->num_chars++;
		if (line->buffer[j] == '\0') { break; }
	}
}

static void shiftCharsLeft(Line* line, int index, int shift_val)
{
	for (int i = index; i < line->num_chars; i++) {
		line->buffer[i - shift_val] = line->buffer[i];
	}
	line->buffer[line->num_chars - 1] = '\0';
}

static void shiftCharsRight(Line* line, int index, int shift_val)
{
	for (int i = MAX_LINE_LENGTH - 1; i > index; i--) {
		line->buffer[i] = line->buffer[i - shift_val];
	}
}

void backspace(Editor* editor, Line* line, int buffer_index)
{
	// check for buffer index less than zero - backspace removes the character to the left of the cursor.
	if (line->head != NULL && buffer_index < 0) {
		editor->y--;
		editor->x = line->head->num_chars;
		mergePrevious(line);
		deleteLine(line);
		return;
	}
	else if (line->head == NULL && buffer_index < 0) {
		// this is the start of the document, so do nothing
		return;
	}

	if (buffer_index == line->num_chars) {
		line->buffer[buffer_index] = '\0';
		line->num_chars--;
		return;
	}
	else {
		shiftCharsLeft(line, buffer_index + 1, 1);
		line->num_chars--;
		return;
	}
}

void splitLine(Line* head, int buffer_index)
{
	Line* line = malloc(sizeof(Line));
	
	if (line == NULL) {
		fprintf(stderr, "Could not allocate memory to program.");
		exit(1);
	}

	initLine(line);

	int num_chars_to_copy = head->num_chars - buffer_index;
	memcpy(line->buffer, head->buffer + buffer_index, MAX_LINE_LENGTH - buffer_index);

	line->num_chars = num_chars_to_copy;
	head->num_chars -= num_chars_to_copy;
	for (int i = 0; i < num_chars_to_copy; i++) {
		head->buffer[buffer_index + i] = '\0';
	}

	Line* tail = head->tail;

	if (tail == NULL) {
		head->tail = line;
		line->head = head;
	}
	else {
		head->tail = line;
		line->head = head;
		line->tail = tail;
		tail->head = line;
	}

}

Line* getLine(Line* head, int line_number)
{
	if (line_number == 0) {
		return head;
	}

	int i = 0;
	while (head->tail != NULL) {
		head = head->tail;
		i++;
		if (i == line_number) {
			return head;
		}
	}

	return NULL;
}

void freeLine(Line* line)
{
	free(line);
}

void freeAllLines(Line* head)
{
	Line* line = head;

	while (line->tail != NULL) {
		line = line->tail;
	}

	while (line->head != NULL) {
		Line* head = line->head;
		freeLine(line);
		line = head;
	}

	freeLine(line);
}