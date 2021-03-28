#ifndef LINE_H
#define LINE_H

#include "Common.h"
#include "Editor.h"

typedef struct __Line {
	struct __Line* head;
	struct __Line* tail;
	char buffer[MAX_LINE_LENGTH];
	int num_chars;
} Line;

void initLine(Line* line);

static void shiftCharsLeft(Line* line, int index, int shift_val);
static void shiftCharsRight(Line* line, int index, int shift_val);

void insertLine(Line* head);
void splitLine(Line* head, int buffer_index);
void deleteLine(Line* line);
void addChar(Editor* editor, Line* line, char c, int buffer_index);
void backspace(Editor* editor, Line* line, int buffer_index);
Line* getLine(Line* head, int line_number);
void freeLine(Line* line);
void mergePrevious(Line* line);

void freeAllLines(Line* head);
#endif
