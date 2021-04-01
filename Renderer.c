
#include <string.h>
#include "Renderer.h"

void drawEditor(Console* console, Editor* editor, Line* line)
{
	int len = (int)strlen(editor->filename);
	
	for (int i = 0; i < len; i++) {
		putChar(console, editor->filename[i], i, 0);
	}

	if (editor->command_mode == COMMAND_MODE) {
		drawCommandMode(console, editor);
	}
	else if (editor->command_mode == COMMAND_INPUT){
		drawCommandTextMode(console, editor);
	}

	drawTextInputMode(console, editor, line);
}

static void drawTextInputMode(Console* console, Editor* editor, Line* line)
{
	int j = 0;
	line = getLine(line, editor->frame_y);
	
	SDL_Color numberColour = { 0xb0, 0xc4, 0xde };
	SDL_Color keywordColour = { 0xcd, 0x5c, 0x5c };
	SDL_Color stringColour = { 0x99, 0x66, 0x00 };

	do {
		drawLineNumber(console, editor, j);

		for (int i = editor->frame_x; ; ) {
			int k = i;
			char c = line->buffer[i];
			
			if (c == '\n' || c == '\0') {
				break;
			}
			
			bool isDigit = false;
			bool isKeyword = false;
			bool isString = false;

			if (c >= '0' && c <= '9') {
				number(line->buffer, &k);
				isDigit = true;
			} else  if (c >= 'a' && c <= 'z') {
				identifier(line->buffer, &k);
				isKeyword = checkKeyword(line->buffer, i, k);
			}
			else if (c == '"') {
				string(line->buffer, &k);
				isString = true;
			}

			if (i < k && (isDigit || isKeyword || isString)) {
				while (i < k) {
					if (isDigit) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, numberColour);
					}
					else if (isKeyword) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, keywordColour);
					}
					else if (isString) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, stringColour);
					}
					i++;
				}
				continue;
			}
			else {
				if (i < k) {
					while (i < k) {
						putChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset);
						i++;
					}
				}
				else {
					putChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset);
					i++;
				}
			}
		}

		line = line->tail;
		j++;
	} while (line != NULL);

	putChar(console, '_', (editor->x + editor->x_offset - editor->frame_x), (editor->y + editor->y_offset - editor->frame_y));
}

static void drawCommandMode(Console* console, Editor* editor)
{
	putChar(console, '_', editor->x + editor->x_offset, (editor->y + editor->y_offset - editor->frame_y));
}

static void drawCommandTextMode(Console* console, Editor* editor)
{
	for (int i = 0; i < editor->num_command_chars; i++) {
		putChar(console, editor->command[i], i, 1);
	}
}

void drawLineNumber(Console* console, Editor* editor, int line_number)
{
	char line_num[] = { '\0', '\0' , '\0' , '\0' , '\0' };
	sprintf_s(line_num, 5, "%i", line_number + 1 + editor->frame_y);
	for (int i = 0; i < 5; i++) {
		putChar(console, line_num[i], i, line_number + editor->y_offset);
	}
	putChar(console, ' ', 5, line_number + editor->y_offset);
}

bool checkKeyword(char* buffer, int i, int k)
{
	char c = buffer[i];
	switch (c) {
	case 'a': return memcmp(&buffer[i], "and", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("and");
	case 'b': return memcmp(&buffer[i], "break", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("break");
	case 'c': return memcmp(&buffer[i], "continue", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("continue");
	case 'e': return memcmp(&buffer[i], "else", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("else");
	case 'f': 
		switch (buffer[i + 1]) {
		case 'u': return memcmp(&buffer[i], "func", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("func");
		case 'o': return memcmp(&buffer[i], "for", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("for");
		default: return false;
		}
		break;
	case 'i': return memcmp(&buffer[i], "if", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("if");
	case 'l': return memcmp(&buffer[i], "let", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("let");
	case 'o': return memcmp(&buffer[i], "or", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("or");
	case 'r': return memcmp(&buffer[i], "return", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("return");
	case 't': 
		switch (buffer[i + 1]) {
		case 'r': return memcmp(&buffer[i], "true", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("true");
		case 'h': return memcmp(&buffer[i], "then", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("then");
		default: return false;
		}
		break;
	case 'w': return memcmp(&buffer[i], "while", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("while");
	default: return false;
	}
}

void number(char* buffer, int* k)
{
	while (buffer[*k] >= '0' && buffer[*k] <= '9') {
		(*k)++;
	}
}

void identifier(char* buffer, int* k)
{
	while (buffer[*k] >= 'a' && buffer[*k] <= 'z') {
		(*k)++;
	}
}

void string(char* buffer, int* k)
{
	(*k)++; // skip over the opening quote
	while (buffer[*k] != '"') {
		(*k)++;
	}
	(*k)++; // skip past the closing quote 
}
