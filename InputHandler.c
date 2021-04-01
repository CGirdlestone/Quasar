
#include "InputHandler.h"
#include "Utils.h"

static void moveUp(Console* console, Editor* editor, Line* line)
{
	if (editor->y == 0) {
		return;
	}

	editor->y--;

	editor->x = editor->x > line->head->num_chars ? line->head->num_chars - 1 : editor->x;
}

static void moveDown(Console* console, Editor* editor, Line* line)
{
	if (line->tail == NULL) {
		return;
	}

	editor->y++;

	editor->x = editor->x > line->tail->num_chars ? line->tail->num_chars - 1 : editor->x;

	if (editor->x < 0) {
		editor->x = 0;
	}

	if (line->tail->num_chars < console->width / console->font_width) {
		editor->frame_x = 0;
	}
}

static void moveLeft(Console* console, Editor* editor, Line* line)
{
	if (editor->x == 0) {
		if (line->head != NULL) {
			moveUp(console, editor, line);
			editor->x = line->head->num_chars - 1;
		}
	}
	else {
		editor->x--;
	}
}

static void moveRight(Console* console, Editor* editor, Line* line)
{
	if (editor->x == line->num_chars) {
		if (line->tail != NULL) {
			moveDown(console, editor, line);
		}
	}
	else {
		if (line->buffer[editor->x] == '\n') {
			if (line->tail != NULL) {
				editor->y++;
				editor->x = 0;
			}
		}
		else {
			editor->x++;
		}
	}
}

void moveFrameUp(Console* console, Editor* editor)
{
	if (editor->y < editor->frame_y) {
		editor->frame_y--;
	}
}

void moveFrameDown(Console* console, Editor* editor)
{
	if ((editor->y - editor->frame_y) == (console->height / console->font_height - editor->y_offset)) {
		editor->frame_y++;
	}
}

void moveFrameRight(Console* console, Editor* editor)
{
	if ((editor->x - editor->frame_x) >= (console->width / console->font_width - editor->x_offset)) {
		editor->frame_x++;
	}
}

void moveFrameLeft(Console* console, Editor* editor) 
{
	if (editor->x < editor->frame_x && editor->frame_x > 0) {
		editor->frame_x--;
	}
}

void updateFramePosition(Console* console, Editor* editor)
{
	moveFrameUp(console, editor);
	moveFrameDown(console, editor);
	moveFrameLeft(console, editor);
	moveFrameRight(console, editor);
}

void handleInput(Console* console, Editor* editor, Line* line, SDL_Event* e)
{
	if (editor->command_mode == COMMAND_MODE) {
		commandModeInput(console, editor, line, e);
	}
	else if (editor->command_mode == COMMAND_INPUT) {
		commandTextInput(editor, line, e);
	}
	else if (editor->command_mode == TEXT_MODE) {
		textInputMode(console, editor, line, e);
	}
}

static void commandModeInput(Console* console, Editor* editor, Line* line, SDL_Event* e)
{
	while (SDL_PollEvent(e)) {
		if (e->type == SDL_QUIT) {
			editor->running = false;
			break;
		}
		else if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.sym == SDLK_SEMICOLON && editor->shift_key) {
				editor->command_mode = COMMAND_INPUT; 
				char c = ':';
				editor->command[editor->num_command_chars++] = c;
				return;
			}
			switch (e->key.keysym.sym){
			case SDLK_i: editor->command_mode = TEXT_MODE; break;
			case SDLK_o: insertLine(line); moveDown(console, editor, line); editor->command_mode = TEXT_MODE; break;
			case SDLK_UP: moveUp(console, editor, line); break;
			case SDLK_DOWN: moveDown(console, editor, line); break;
			case SDLK_LEFT: moveLeft(console, editor, line); break;
			case SDLK_RIGHT: moveRight(console, editor, line); break;
			case SDLK_LSHIFT: editor->shift_key = true; break;
			case SDLK_RSHIFT: editor->shift_key = true; break;
			}
		}
		else if (e->type == SDL_KEYUP) {
			switch (e->key.keysym.sym) {
			case SDLK_LSHIFT: editor->shift_key = false; break;
			case SDLK_RSHIFT: editor->shift_key = false; break;
			}
		}
	}
	updateFramePosition(console, editor);
}

static void commandTextInput(Editor* editor, Line* line, SDL_Event* e)
{
	while (SDL_PollEvent(e)) {
		if (e->type == SDL_QUIT) {
			editor->running = false;
			break;
		}
		else if (e->type == SDL_KEYDOWN) {
			if ((int)e->key.keysym.sym >= 97 && (int)e->key.keysym.sym <= 122) {
				addCharCommand(editor, e->key.keysym.sym);
				return;
			}
			switch (e->key.keysym.sym) {
			case SDLK_RETURN: {
				executeCommand(editor, line);
				resetCommand(editor); editor->command_mode = COMMAND_MODE; 
				break;
			}
			case SDLK_LSHIFT: editor->shift_key = true; break;
			case SDLK_RSHIFT: editor->shift_key = true; break;
			case SDLK_BACKSPACE: {
				editor->num_command_chars = editor->num_command_chars - 1 < 1 ? 1 : editor->num_command_chars - 1;
				editor->command[editor->num_command_chars] = '\0'; 
				break;
			}
			}
		}
		else if (e->type == SDL_KEYUP) {
			switch (e->key.keysym.sym) {
			case SDLK_LSHIFT: editor->shift_key = false; break;
			case SDLK_RSHIFT: editor->shift_key = false; break;
			}
		}
	}
}

static void textInputMode(Console* console, Editor* editor, Line* line, SDL_Event* e)
{
	while (SDL_PollEvent(e)) {
		if (e->type == SDL_QUIT) {
			editor->running = false;
			break;
		}
		else if (e->type == SDL_KEYDOWN) {
			if ((int)e->key.keysym.sym >= 33 && (int)e->key.keysym.sym <= 122) {
				if (!editor->shift_key) {
					addChar(editor, line, e->key.keysym.sym, editor->x);
				}
				else {
					if ((int)e->key.keysym.sym >= 97 && (int)e->key.keysym.sym <= 122) {
						addChar(editor, line, (char)((int)e->key.keysym.sym - 32), editor->x);
					}
					switch (e->key.keysym.sym) {
					case SDLK_LEFTBRACKET: addChar(editor, line, '{', editor->x); break;
					case SDLK_RIGHTBRACKET: addChar(editor, line, '}', editor->x); break;
					case SDLK_SEMICOLON: addChar(editor, line, ':', editor->x); break;
					case SDLK_1: addChar(editor, line, '!', editor->x); break;
					case SDLK_2: addChar(editor, line, '"', editor->x); break;
					case SDLK_3: addChar(editor, line, '£', editor->x); break;
					case SDLK_4: addChar(editor, line, '$', editor->x); break;
					case SDLK_5: addChar(editor, line, '%', editor->x); break;
					case SDLK_6: addChar(editor, line, '^', editor->x); break;
					case SDLK_7: addChar(editor, line, '&', editor->x); break;
					case SDLK_8: addChar(editor, line, '*', editor->x); break;
					case SDLK_9: addChar(editor, line, '(', editor->x); break;
					case SDLK_0: addChar(editor, line, ')', editor->x); break;
					case SDLK_EQUALS: addChar(editor, line, '+', editor->x); break;
					case SDLK_MINUS: addChar(editor, line, '_', editor->x); break;
					case SDLK_QUOTE: addChar(editor, line, '@', editor->x); break;
					case SDLK_HASH: addChar(editor, line, '~', editor->x); break;
					case SDLK_SLASH: addChar(editor, line, '?', editor->x); break;
					case SDLK_PERIOD: addChar(editor, line, '>', editor->x); break;
					case SDLK_COMMA: addChar(editor, line, '<', editor->x); break;
					case SDLK_BACKSLASH: addChar(editor, line, '|', editor->x); break;
					}
				}
				editor->x++; editor->x = editor->x < line->num_chars ? editor->x : line->num_chars;
			}

			switch (e->key.keysym.sym) {
			case SDLK_UP: moveUp(console, editor, line); break;
			case SDLK_DOWN: moveDown(console, editor, line); break;
			case SDLK_LEFT: moveLeft(console, editor, line); break;
			case SDLK_RIGHT: moveRight(console, editor, line); break;
			case SDLK_BACKSPACE: {
				if (!editor->ctrl_key) {
					backspace(editor, line, editor->x - 1); 
					editor->x--;
					editor->x = editor->x < 0 ? 0 : editor->x;
				}
				else {
					int x = seekPreviousSpace(editor, line);

					for(int i = 0; i < x - 1; i++) {
						backspace(editor, line, editor->x - 1);
						editor->x--; 
						editor->x = editor->x < 0 ? 0 : editor->x;
					}
				}
				break;
			}
			case SDLK_DELETE: 
				backspace(editor, line, editor->x); 
				editor->x = editor->x < 0 ? 0 : editor->x; 
				break;
			case SDLK_SPACE: 
				addChar(editor, line, ' ', editor->x); 
				editor->x++; 
				editor->x = editor->x < line->num_chars ? editor->x : line->num_chars; 
				break;
			case SDLK_RETURN: addChar(editor, line, '\n', editor->x); break;
			case SDLK_LSHIFT: editor->shift_key = true; break;
			case SDLK_RSHIFT: editor->shift_key = true; break;
			case SDLK_LCTRL: editor->ctrl_key = true; break;
			case SDLK_RCTRL: editor->ctrl_key = true; break;
			case SDLK_TAB: {
				for (int i = 0; i < 4; i++) {
					addChar(editor, line, ' ', editor->x); editor->x++; editor->x = editor->x < line->num_chars ? editor->x : line->num_chars;
				}
				break;
			}
			case SDLK_ESCAPE: editor->command_mode = COMMAND_MODE; break;
			}
		}
		else if (e->type == SDL_KEYUP) {
			switch (e->key.keysym.sym) {
			case SDLK_LSHIFT: editor->shift_key = false; break;
			case SDLK_RSHIFT: editor->shift_key = false; break;
			case SDLK_LCTRL: editor->ctrl_key = false; break;
			case SDLK_RCTRL: editor->ctrl_key = false; break;
			}
		}
	}

	updateFramePosition(console, editor);
}

void addCharCommand(Editor* editor, char c) {
	int i = editor->num_command_chars++;
	editor->command[i] = c;
}

void resetCommand(Editor* editor)
{
	for (int i = 0; i < MAX_COMMAND_CHARS; i++) {
		editor->command[i] = '\0';
	}
	editor->num_command_chars = 0;
}

void executeCommand(Editor* editor, Line* line)
{
	Line* head = NULL;
	head = line;

	while (line->head != NULL) {
		head = line->head;
		line = head;
	} 

	int i = 1;
	while (editor->command[i] != '\0') {
		char c = editor->command[i];
		switch (c) {
		case 'q': editor->running = false; resetCommand(editor); return;
		case 'w': {
				FILE* file = NULL;

				if (fopen_s(&file, editor->filename, "w") != 0) {
					fprintf(stderr, "Failed to open file.");
					exit(1);
				}

				save(head, file);
					
				fclose(file);
			}
			i++;
			break; 
		}
	}

	resetCommand(editor);
}

static int seekPreviousSpace(Editor* editor, Line* line)
{
	int x = 0;
	while (true) {
		if (line->num_chars == 0) {
			return x;
		}

		if (x == line->num_chars) {
			return line->num_chars;
		}

		if (line->buffer[line->num_chars - x] == ' ') {
			return x;
		}
		x++;
	}

	return x;
}
