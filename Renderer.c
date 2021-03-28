
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
	do {
		char line_num[] = {'\0', '\0' , '\0' , '\0' , '\0' };
		sprintf_s(line_num, 5, "%i", j + 1 + editor->frame_y);
		for (int i = 0; i < 5; i++) {
			putChar(console, line_num[i], i, j + editor->y_offset);
		}
		putChar(console, ' ', 5, j + editor->y_offset);

		for (int i = 0; i < line->num_chars; i++) {
			if (line->buffer[i] == '\n') {
				continue;
			}
			else if (line->buffer[i] == '\0') {
				continue;
			}
			putChar(console, line->buffer[i], i + editor->x_offset, j + editor->y_offset);
		}
		line = line->tail;
		j++;
	} while (line != NULL);

	putChar(console, '_', editor->x + editor->x_offset, (editor->y + editor->y_offset - editor->frame_y));
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