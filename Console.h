#ifndef CONSOLE_H
#define CONSOLE_H

#include "Common.h"

typedef struct {
	int width;
	int height;
	const char* title;
	int font_width;
	int font_height;
	SDL_Window* window;
	SDL_Texture* font;
	SDL_Renderer* renderer;
	SDL_Color background;
	SDL_Color font_colour;
} Console;

void initSDL();

static void initConsole(Console* console);
static void createWindow(Console* console);
static void createRenderer(Console* console);
static void loadFont(Console* console, const char* path);

Console* createConsole(int width, int height, const char* title, const char* _path);
void clearConsole(Console* console);
void updateConsole(Console* console);
void freeConsole(Console* console);

void putChar(Console* console, char c, int x, int y);


#endif 

