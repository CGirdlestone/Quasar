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
	SDL_Color keyword_colour;
	SDL_Color number_colour;
	SDL_Color string_colour;
	SDL_Color comment_colour;
} Console;

void initSDL();

static void initConsole(Console* console);
static void createWindow(Console* console);
static void createRenderer(Console* console);
static void loadFont(Console* console, const char* path);
static void loadSyntaxHighlightColours(Console* console, lua_State*);

Console* createConsole(int width, int height, const char* title, const char* _path);
void clearConsole(Console* console);
void updateConsole(Console* console);
void freeConsole(Console* console);

void putChar(Console* console, unsigned char c, int x, int y);
void putColourChar(Console* console, unsigned char c, int x, int y, SDL_Color colour);


#endif 

