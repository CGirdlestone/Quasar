
#include <string.h>
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
	editor->lang = L_NONE;
}

void setLanguage(Editor* editor, const char* language)
{
	if (memcmp(language, "c", strlen("c")) == 0) {
		editor->lang = L_C;
	}
	else if (memcmp(language, "cpp", strlen("cpp")) == 0 || memcmp(language, "cxx", strlen("cxx")) == 0) {
		editor->lang = L_CPP;
	}
	else if (memcmp(language, "rs", strlen("rust")) == 0) {
		editor->lang = L_RUST;
	}
	else if (memcmp(language, "py", strlen("py")) == 0) {
		editor->lang = L_PY;
	}
	else if (memcmp(language, "po", strlen("po")) == 0) {
		editor->lang = L_PO;
	}
	else {
		editor->lang = L_NONE;
	}
}

