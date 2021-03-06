
#pragma warning(disable:4996)
#include <string.h>
#include "Common.h"
#include "Console.h"
#include "Line.h"
#include "Utils.h"
#include "InputHandler.h"
#include "Renderer.h"


int main(int argc, char* argv[])
{
	const char* path = "C:\\Users\\craig\\Documents\\TextEditor\\";

	initSDL();
	
	const char* config = "config.lua";
	char* lua_path = malloc(sizeof(char) * (strlen(path) + strlen(config) + 1));
	if (lua_path == NULL) {
		fprintf(stderr, "Cannot allocate enough memory to program");
		exit(1);
	}
	memcpy(lua_path, path, strlen(path));
	memcpy(lua_path + strlen(path), config, strlen(config));
	lua_path[strlen(path) + strlen(config)] = '\0';

	lua_State* vm = luaL_newstate();
	luaL_dofile(vm, lua_path);
	lua_getglobal(vm, "window_width");
	int width = lua_tointeger(vm, -1);
	lua_getglobal(vm, "window_height");
	int height = lua_tointeger(vm, -1);
	lua_close(vm);

	Console* console = createConsole(width, height, "Text Editor", path);
	
	Editor editor;
	initEditor(&editor, argv[1]);

	int k = strlen(argv[1]) - 1;
	size_t size = 1;
	while (argv[1][k - 1] != '.') {
		k--;
		size++;
	}

	char* extension = malloc(sizeof(char) * (size + 1));
	if (extension == NULL) {
		fprintf(stderr, "Could not allocate enough memory to program.\n");
		exit(1);
	}
	memcpy(extension, argv[1] + k, size);
	extension[size] = '\0';
	setLanguage(&editor, extension);
	free(extension);
	
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