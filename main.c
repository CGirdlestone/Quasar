
#pragma warning(disable:4996)
#include "Common.h"
#include "Console.h"
#include "Line.h"
#include "Utils.h"
#include "InputHandler.h"
#include "Renderer.h"


int main(int argc, char* argv[])
{
	initSDL();
	
	lua_State* vm = luaL_newstate();
	luaL_dofile(vm, "config.lua");
	lua_getglobal(vm, "window_width");
	int width = lua_tointeger(vm, -1);
	lua_getglobal(vm, "window_height");
	int height = lua_tointeger(vm, -1);
	lua_close(vm);

	Console* console = createConsole(width, height, "Text Editor");
	
	Editor editor;
	initEditor(&editor, argv[1]);
	
	Line* head = NULL;
	FILE* file = NULL;

	file = fopen(editor.filename, "ab+");

	if (file == NULL) {
		head = malloc(sizeof(Line));
		initLine(head);
	}
	else {
		head = load(file);
		fclose(file);
	}


	while (editor.running) {
		clearConsole(console);

		Line* line = NULL;
		line = head;

		Line* current_line = NULL;
		current_line = getLine(head, editor.y);

		SDL_Event e;
		handleInput(console, &editor, current_line, &e);

		drawEditor(console, &editor, line);

		updateConsole(console);
	}

	freeConsole(console);
	freeAllLines(head);
}