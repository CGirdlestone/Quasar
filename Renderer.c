
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

		for (int i = 0; ; ) {
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
				while (i < k){
					if (i < editor->frame_x) { i++; continue; };
					if (isDigit) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, console->number_colour);
					}
					else if (isKeyword) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, console->keyword_colour);
					}
					else if (isString) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, console->string_colour);
					}
					else if (isComment) {
						putColourChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset, console->comment_colour);
					}
					i++;
				}
				continue;
			}
			else {
				if (i < k) {
					while (i < k) {
						if (i < editor->frame_x) { i++; continue; };
						putChar(console, line->buffer[i], (i - editor->frame_x) + editor->x_offset, j + editor->y_offset);
						i++;
					}
				}
				else {
					if (i < editor->frame_x) { i++; continue; };
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
		if (line_num[i] == '\0') break;
		putChar(console, line_num[i], i, line_number + editor->y_offset);
	}
	char vertical_bar = (unsigned char)(179);
	putChar(console, vertical_bar, editor->x_offset - 1, line_number + editor->y_offset);
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
	case 'a': return memcmp(&buffer[i], "auto", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("auto");
	case 'b': return memcmp(&buffer[i], "break", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("break");
	case 'c': 
		switch (buffer[i + 1]) {
		case 'a': return memcmp(&buffer[i], "case", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("case");
		case 'h': return memcmp(&buffer[i], "char", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("char");
		case 'o': 
			switch (buffer[i + 2]) {
			case 'n': 
				switch (buffer[i + 3]) {
				case 't': return memcmp(&buffer[i], "continue", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("continue");
				case 's': return memcmp(&buffer[i], "const", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("const");
				default: return false;
				}
				break;
			default: return false;
			}
			break;
		default: return false;
		}
		break;
	case 'd': 
		switch (buffer[i + 1]) {
		case 'o': 
			if (k - i == 2) {
				return true;
			}

			switch (buffer[i + 2]) {
			case 'u': return memcmp(&buffer[i], "double", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("double");
			default: return false;
			}
			break;
		case 'e': return memcmp(&buffer[i], "default", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("default");
		default: return false;
		}
		break;
	case 'e': 
		switch (buffer[i + 1]) {
		case 'l': return memcmp(&buffer[i], "else", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("else");
		case 'n': return memcmp(&buffer[i], "enum", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("enum");
		case 'x': return memcmp(&buffer[i], "extern", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("extern");
		}
	case 'f':
		switch (buffer[i + 1]) {
		case 'l': return memcmp(&buffer[i], "float", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("float");
		case 'o': return memcmp(&buffer[i], "for", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("for");
		default: return false;
		}
		break;
	case 'g': return memcmp(&buffer[i], "goto", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("goto");
	case 'i': 
		switch (buffer[i + 1]) {
		case 'f': return memcmp(&buffer[i], "if", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("if");
		case 'n': return memcmp(&buffer[i], "int", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("int");
		default: return false;
		}
		break;
	case 'l': return memcmp(&buffer[i], "long", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("long");
	case 'o': return memcmp(&buffer[i], "or", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("or");
	case 'r': 
		switch (buffer[i + 1]) {
		case 'e':
			switch (buffer[i + 2]) {
			case 't': return memcmp(&buffer[i], "return", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("return");
			case 'g': return memcmp(&buffer[i], "register", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("register");
			default: return false;
			}
			break;
		default: return false;
		}
		break;
	case 's':
		switch (buffer[i + 1]) {
		case 'h': return memcmp(&buffer[i], "short", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("short");
		case 'w': return memcmp(&buffer[i], "switch", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("switch");
		case 'i':
			switch (buffer[i + 2]) {
			case 'g': return memcmp(&buffer[i], "signed", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("signed");
			case 'z': return memcmp(&buffer[i], "sizeof", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("sizeof");
			default: return false;
			}
			break;
		case 't':
			switch (buffer[i + 2]) {
			case 'a': return memcmp(&buffer[i], "static", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("static");
			case 'r': return memcmp(&buffer[i], "struct", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("struct");
			default: return false;
			}
			break;
		default: return false;
		}
		break;
	case 't': return memcmp(&buffer[i], "typedef", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("typedef");
	case 'u': 
		switch (buffer[i + 1]) {
		case 'n': 
			switch (buffer[i + 2]) {
			case 'i': return memcmp(&buffer[i], "union", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("union");
			case 's': return memcmp(&buffer[i], "unsigned", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("unsigned");
			default: return false;
			}
			break;
		default: return false;
		}
		break;
	case 'v': 
		switch (buffer[i + 1]) {
		case 'o':
			switch (buffer[i + 2]) {
			case 'i': return memcmp(&buffer[i], "void", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("void");
			case 'l': return memcmp(&buffer[i], "volatile", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("volatile");
			default: return false;
			}
			break;
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
	case 'a':
		switch (buffer[i + 1]) {
		case 'b': return memcmp(&buffer[i], "abstract", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("abstract");
		case 'w': return memcmp(&buffer[i], "await", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("await");
		case 's':
			if (k - i == 2) { return true; };
			return memcmp(&buffer[i], "async", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("async");
		}
	case 'b':
		switch (buffer[i + 1]) {
		case 'r': return memcmp(&buffer[i], "break", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("break");
		case 'e': return memcmp(&buffer[i], "become", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("become");
		case 'o': return memcmp(&buffer[i], "box", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("box");
		default: return false;
		}
	case 'c':
		switch (buffer[i + 1]) {
		case 'r': return memcmp(&buffer[i], "crate", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("crate");
		case 'o':
			switch (buffer[i + 2]) {
			case 'n':
				switch (buffer[i + 3]) {
				case 't': return memcmp(&buffer[i], "continue", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("continue");
				case 's': return memcmp(&buffer[i], "const", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("const");
				default: return false;
				}
			default: return false;
			}
		default: return false;
		}
	case 'd':
		switch (buffer[i + 1]) {
		case 'y': return memcmp(&buffer[i], "dyn", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("dyn");
		case 'o': return memcmp(&buffer[i], "do", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("do");
		default: return false;
		}
	case 'e':
		switch (buffer[i + 1]) {
		case 'l': return memcmp(&buffer[i], "else", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("else");
		case 'n': return memcmp(&buffer[i], "enum", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("enum");
		case 'x': return memcmp(&buffer[i], "extern", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("extern");
		}
	case 'f':
		switch (buffer[i + 1]) {
		case 'a': return memcmp(&buffer[i], "false", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("false");
		case 'o': return memcmp(&buffer[i], "for", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("for");
		case 'n': return memcmp(&buffer[i], "fn", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("fn");
		case 'i': return memcmp(&buffer[i], "final", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("final");
		default: return false;
		}
	case 'i':
		switch (buffer[i + 1]) {
		case 'f': return memcmp(&buffer[i], "if", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("if");
		case 'm': return memcmp(&buffer[i], "impl", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("impl");
		case 'n': return memcmp(&buffer[i], "in", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("in");
		default: return false;
		}
	case 'l':
		switch (buffer[i + 1]) {
		case 'e': return memcmp(&buffer[i], "let", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("let");
		case 'o': return memcmp(&buffer[i], "loop", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("loop");
		default: return false;
		}
	case 'm':
		switch (buffer[i + 1]) {
		case 'a':
			switch (buffer[i + 2]) {
			case 'c':return memcmp(&buffer[i], "macro", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("macro");
			case 't':return memcmp(&buffer[i], "match", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("match");
			default: return false;
			}
		case 'o':
			switch (buffer[i + 2]) {
			case 'd': return memcmp(&buffer[i], "mod", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("mod");
			case 'v':return memcmp(&buffer[i], "move", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("move");
			default: return false;
			}
		case 'u':return memcmp(&buffer[i], "mut", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("mut");
		}
	case 'o': return memcmp(&buffer[i], "override", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("override");
	case 'p': return memcmp(&buffer[i], "priv", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("priv");
	case 'r':
		switch (buffer[i + 1]) {
		case 'e':
			switch (buffer[i + 2]) {
			case 't': return memcmp(&buffer[i], "return", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("return");
			case 'f': return memcmp(&buffer[i], "ref", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("ref");
			default: return false;
			}
		default: return false;
		}
	case 's':
		switch (buffer[i + 1]) {
		case 'e': return memcmp(&buffer[i], "self", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("self");
		case 't':
			switch (buffer[i + 2]) {
			case 'a': return memcmp(&buffer[i], "static", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("static");
			case 'r': return memcmp(&buffer[i], "struct", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("struct");
			default: return false;
			}
		case 'u': return memcmp(&buffer[i], "super", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("super");
		default: return false;
		}
	case 'S': return memcmp(&buffer[i], "Self", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("Self");
	case 't':
		switch (buffer[i + 1]) {
		case 'y': 
			if (k - i == 4) {
				return memcmp(&buffer[i], "type", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("type");
			}
			return memcmp(&buffer[i], "typeof", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("typeof");
		case 'r':
			switch (buffer[i + 1]) {
			case 'a': return memcmp(&buffer[i], "trait", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("trait");
			case 'u': return memcmp(&buffer[i], "true", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("true");
			case 'y': return memcmp(&buffer[i], "try", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("try");
			default: return false;
			}
		default: return false;
		}
	case 'u':
		switch (buffer[i + 1]) {
		case 'n':
			switch (buffer[i + 2]) {
			case 's':
				switch (buffer[i + 3]) {
				case 'a': return memcmp(&buffer[i], "unsafe", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("unsafe");
				case 'i':  return memcmp(&buffer[i], "unsized", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("unsized");
				default: return false;
				}
			default: return false;
			}
		case 's':  return memcmp(&buffer[i], "use", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("use");
		default: return false;
		}
	case 'v':  return memcmp(&buffer[i], "virtual", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("virtual");
	case 'w':
		switch (buffer[i + 1]) {
		case 'h':
			switch (buffer[i + 2]) {
			case'i': return memcmp(&buffer[i], "while", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("while");
			case 'e':  return memcmp(&buffer[i], "where", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("where");
			default: return false;
			}
		default: return false;
		}
	case 'y': return memcmp(&buffer[i], "yield", ((size_t)k - (size_t)i)) == 0 && ((size_t)k - (size_t)i) == strlen("yield");
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
