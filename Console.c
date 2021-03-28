#include <string.h>
#include "Console.h"

void initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to initialise SDL.");
		exit(1);
	}

	int flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
	if (!(IMG_Init(flags) & flags)) {
		fprintf(stderr, "Failed to initialise SDL_Image.");
		exit(1);
	}
}

static void initConsole(Console* console)
{
	console->width = 0;
	console->height = 0;
	console->title = "";
	console->window = NULL;
	console->font = NULL;
}

static void createWindow(Console* console)
{
	console->window = SDL_CreateWindow(console->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, console->width, console->height, SDL_WINDOW_SHOWN);

	if (console->window == NULL) {
		fprintf(stderr, "Failed to create SDL Window.");
		exit(1);
	}
}

static void createRenderer(Console* console)
{
	console->renderer = SDL_CreateRenderer(console->window, -1, SDL_RENDERER_ACCELERATED);

	if (console->renderer == NULL) {
		fprintf(stderr, "Failed to create SDL Renderer.");
		exit(1);
	}
	lua_State* vm = luaL_newstate();
	luaL_dofile(vm, "config.lua");

	lua_getglobal(vm, "background_colour");

	lua_pushstring(vm, "red");
	lua_gettable(vm, -2);
	int red = lua_tointeger(vm, -1);
	console->background.r = red;
	lua_pop(vm, 1);

	lua_pushstring(vm, "green");
	lua_gettable(vm, -2);
	int green = lua_tointeger(vm, -1);
	console->background.g = green;
	lua_pop(vm, 1);

	lua_pushstring(vm, "blue");
	lua_gettable(vm, -2);
	int blue = lua_tointeger(vm, -1);
	console->background.b = blue;
	lua_pop(vm, 1);

	lua_getglobal(vm, "font_colour");

	lua_pushstring(vm, "red");
	lua_gettable(vm, -2);
	red = lua_tointeger(vm, -1);
	console->font_colour.r = red;
	lua_pop(vm, 1);

	lua_pushstring(vm, "green");
	lua_gettable(vm, -2);
	green = lua_tointeger(vm, -1);
	console->font_colour.g = green;
	lua_pop(vm, 1);

	lua_pushstring(vm, "blue");
	lua_gettable(vm, -2);
	blue = lua_tointeger(vm, -1);
	console->font_colour.b = blue;
	lua_pop(vm, 1);

	lua_close(vm);

	SDL_SetRenderDrawColor(console->renderer, red, green, blue, 0xFF);
}

static void loadFont(Console* console, const char* path)
{
	SDL_Surface* surf = IMG_Load(path);

	if (surf == NULL) {
		fprintf(stderr, "Couldn't load font.");
		exit(1);
	}

	uint32_t* pixels = (uint32_t*)surf->pixels;
	SDL_SetColorKey(surf, SDL_TRUE, pixels[0]);

	console->font = SDL_CreateTextureFromSurface(console->renderer, surf);

	if (console->font == NULL) {
		fprintf(stderr, "Couldn't load font.");
		exit(1);
	}

	SDL_FreeSurface(surf);
}

Console* createConsole(int width, int height, const char* title)
{
	Console* console = (Console*)malloc(sizeof(Console));

	if (console == NULL) {
		fprintf(stderr, "Cannot allocate enough memory for program.");
		exit(1);
	}

	initConsole(console);

	lua_State* vm = luaL_newstate();
	luaL_dofile(vm, "config.lua");
	lua_getglobal(vm, "font");
	const char* filepath = lua_tostring(vm, -1);
	const char* directory = "C:\\Users\\craig\\Documents\\TextEditor\\";
	char* absolute = malloc(sizeof(char) * (strlen(filepath) + strlen(directory) + 1));
	if (absolute == NULL) {
		lua_close(vm);
		fprintf(stderr, "Failed to allocate memory to program.");
		exit(1);
	}
	memcpy(absolute, directory, strlen(directory));
	memcpy(absolute + strlen(directory), filepath, strlen(filepath));
	absolute[strlen(filepath) + strlen(directory)] = '\0';
	
	lua_getglobal(vm, "font_width");
	int font_width = lua_tointeger(vm, -1);
	lua_getglobal(vm, "font_height");
	int font_height = lua_tointeger(vm, -1);
	lua_close(vm);

	console->width = width * font_width;
	console->height = height * font_height;
	console->font_width = font_width;
	console->font_height = font_height;
	console->title = title;

	createWindow(console);
	createRenderer(console);

	loadFont(console, absolute);

	return console;
}

void clearConsole(Console* console)
{
	SDL_RenderClear(console->renderer);
}

void updateConsole(Console* console)
{
	SDL_RenderPresent(console->renderer);
}

void freeConsole(Console* console)
{
	SDL_DestroyTexture(console->font);
	SDL_DestroyRenderer(console->renderer);
	SDL_DestroyWindow(console->window);
	free(console);
}

void putChar(Console* console, char c, int x, int y)
{
	SDL_Rect dstrect;
	dstrect.x = x * TILE_SIZE_WIDTH;
	dstrect.y = y * TILE_SIZE_HEIGHT;
	dstrect.w = TILE_SIZE_WIDTH;
	dstrect.h = TILE_SIZE_HEIGHT;

	SDL_Rect srcrect;
	srcrect.x = (int)c % TILE_SET_WIDTH * TILE_SIZE_WIDTH;
	srcrect.y = (int)c / TILE_SET_HEIGHT * TILE_SIZE_HEIGHT;
	srcrect.w = TILE_SIZE_WIDTH;
	srcrect.h = TILE_SIZE_HEIGHT;

	SDL_SetTextureColorMod(console->font, console->font_colour.r, console->font_colour.g, console->font_colour.b);
	SDL_RenderCopy(console->renderer, console->font, &srcrect, &dstrect);
	SDL_SetRenderDrawColor(console->renderer, console->background.r, console->background.g, console->background.b, SDL_ALPHA_OPAQUE);
}