
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
	
	SDL_Color numberColour = { 0x00, 0xcf, 0xe2 };
	SDL_Color keywordColour = { 0xca, 0x00, 0xe2 };
	SDL_Color stringColour = { 0xe2, 0x8d, 0x00 };
	SDL_Color commentColour = { 0x55, 0x44, 0x44 };

	do {
		drawLineNumber(console, editor, j);

		for (int i = editor->frame_x; ; ) {
			int k = i;
			char c = line->buffer[i];
			
			if (c == '\n' || c == '\0') {
				break;
			}

			if (editor->lang == L_NONE) {
				putChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset);
				i++;
			}
			
			bool isDigit = false;
			bool isKeyword = false;
			bool isString = false;
			bool isComment = false;

			if (c >= '0' && c <= '9') {
				number(line->buffer, &k);
				isDigit = true;
			} else  if (c >= 'a' && c <= 'z') {
				identifier(line->buffer, &k);
				switch (editor->lang) {
				case L_C: isKeyword = checkKeywordC(line->buffer, i, k); break;
				case L_CPP: isKeyword = checkKeywordCPP(line->buffer, i, k); break;
				case L_RUST: isKeyword = checkKeywordRust(line->buffer, i, k); break;
				case L_PY: isKeyword = checkKeywordPython(line->buffer, i, k); break;
				case L_PO: isKeyword = checkKeywordPolarite(line->buffer, i, k); break;
				}
				
			}
			else if (c == '"') {
				string(line->buffer, &k);
				isString = true;
			}
			else if (checkComment(line->buffer, k, editor->lang)) {
				comment(line->buffer, &k);
				isComment = true;
			}

			if (i < k && (isDigit || isKeyword || isString || isComment)) {
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
					else if (isComment) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, commentColour);
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

bool checkKeywordPolarite(char* buffer, int i, int k)
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

bool checkKeywordC(char* buffer, int i, int k)
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

bool checkKeywordCPP(char* buffer, int i, int k)
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

bool checkKeywordRust(char* buffer, int i, int k)
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

bool checkKeywordPython(char* buffer, int i, int k)
{
	char c = buffer[i];
	switch (c) {
	case 'a': 
		switch (buffer[i + 1]) {
		case 'n': return memcmp(&buffer[i], "and", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("and");
		case 's': 
			if (k - i == 2) {
				// 'as' keyword
				return true;
			}
			switch (buffer[i + 2]) {
			case 's': return memcmp(&buffer[i], "assert", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("assert");
			case 'y': return memcmp(&buffer[i], "async", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("async");
			default: return false;
			}
			break;
		default: return false;
		}
		break;
	case 'b': return memcmp(&buffer[i], "break", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("break");
	case 'c': return memcmp(&buffer[i], "continue", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("continue");
	case 'd': 
		switch (buffer[i + 1]) {
		case 'e':
			switch (buffer[i + 2]) {
			case 'f': return memcmp(&buffer[i], "def", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("def");
			case 'l': return memcmp(&buffer[i], "del", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("del");
			default: return false;
			}
			break;
		default: return false;
		}
		break;
	case 'e': 
		switch (buffer[i + 1]) {
		case 'l': 
			switch (buffer[i + 2]) {
			case 'i': return memcmp(&buffer[i], "elif", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("elif");
			case 's': return memcmp(&buffer[i], "else", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("else");
			default: return false;
			}
			break;
		case 'x': return memcmp(&buffer[i], "except", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("except");
		default: return false;
		}
		break;
	case 'f':
		switch (buffer[i + 1]) {
		case 'i': return memcmp(&buffer[i], "finally", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("finally");
		case 'o': return memcmp(&buffer[i], "for", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("for");
		case 'r': return memcmp(&buffer[i], "from", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("from");
		default: return false;
		}
		break;
	case 'F': return memcmp(&buffer[i], "False", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("False");
	case 'g': return memcmp(&buffer[i], "global", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("global");
	case 'i': 
		switch (buffer[i + 1]) {
		case 'f': return memcmp(&buffer[i], "if", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("if");
		case 'm': return memcmp(&buffer[i], "import", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("import");
		case 'n': return memcmp(&buffer[i], "in", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("in");
		case 's': return memcmp(&buffer[i], "is", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("is");
		default: return false;
		}
		break;
	case 'l': return memcmp(&buffer[i], "lambda", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("lambda");
	case 'n': 
		switch (buffer[i + 1]) {
		case 'o':
			switch (buffer[i + 2]) {
			case 'n': return memcmp(&buffer[i], "nonlocal", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("nonlocal");
			case 't': return memcmp(&buffer[i], "not", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("not");
			default: return false;
			}
			break;
		default: return false;
		}
		break;
	case 'N': return memcmp(&buffer[i], "None", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("None");
	case 'o': return memcmp(&buffer[i], "or", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("or");
	case 'p': return memcmp(&buffer[i], "pass", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("pass");
	case 'r': 
		switch (buffer[i + 1]) {
		case 'a': return memcmp(&buffer[i], "raise", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("raise");
		case 'e': return memcmp(&buffer[i], "return", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("return");
		}
		break;
	case 'T': return memcmp(&buffer[i], "True", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("True");
	case 't': return memcmp(&buffer[i], "try", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("try");
	case 'w': 
		switch (buffer[i + 1]) {
		case 'h': return memcmp(&buffer[i], "while", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("while");
		case 'i': return memcmp(&buffer[i], "with", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("with");
		}
		break;
	case 'y': return memcmp(&buffer[i], "yield", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("yield");
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
	while (buffer[*k] != '"' && buffer[*k] != '\0' && buffer[*k] != '\n') {
		(*k)++;
	}
	if (buffer[*k] == '"') {
		(*k)++; // skip past the closing quote 
	}
}

void comment(char* buffer, int* k)
{
	while (buffer[*k] != '\n') {
		(*k)++;
	}
}

bool checkComment(char* buffer, int k, Language lang)
{
	switch (lang) {
	case L_PO: return buffer[k] == '#';
	case L_PY: return buffer[k] == '#';
	case L_C: return buffer[k] == '\\' && buffer[k + 1] == '\\';
	case L_CPP: return buffer[k] == '\\' && buffer[k + 1] == '\\';
	case L_RUST: return buffer[k] == '\\' && buffer[k + 1] == '\\';
	default: return false;
	}
}
