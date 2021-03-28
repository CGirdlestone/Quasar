
#include "Common.h"
#include "Editor.h"

void initEditor(Editor* editor, const char* _filename)
{
	editor->x = 0;
	editor->y = 0; 
	editor->x_offset = 6;
	editor->y_offset = 2;
	editor->frame_x = 0;
	editor->frame_y = 0;
	editor->running = true;
	editor->command_mode = COMMAND_MODE;
	editor->shift_key = false;
	editor->ctrl_key = false;
	for (int i = 0; i < MAX_COMMAND_CHARS; i++) {
		editor->command[i] = '\0';
	}
	editor->num_command_chars = 0;
	editor->filename = _filename;
}

