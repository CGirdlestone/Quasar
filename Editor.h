#ifndef EDITOR_H
#define EDITOR_H

#define MAX_COMMAND_CHARS 80

typedef enum {
	COMMAND_MODE,
	COMMAND_INPUT,
	TEXT_MODE,
} EditorState;

typedef enum {
	L_NONE,
	L_C,
	L_CPP,
	L_RUST,
	L_PY,
	L_PO,
} Language;

typedef struct {
	int x;
	int y;
	int x_offset;
	int y_offset;
	int frame_x;
	int frame_y;
	bool running;
	EditorState command_mode;
	bool shift_key;
	bool ctrl_key;
	const char* command[MAX_COMMAND_CHARS];
	int num_command_chars;
	const char* filename;
	Language lang;
} Editor;

void initEditor(Editor* editor, const char* filename);
void setLanguage(Editor* editor, const char* language);

#endif
