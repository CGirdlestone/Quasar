#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "Common.h"
#include "Console.h"
#include "Line.h"

void handleInput(Console* console, Editor* editor, Line* line, SDL_Event* event);
static void commandModeInput(Console* console,Editor* editor, Line* line, SDL_Event* event);
static void commandTextInput(Console* console, Editor* editor, Line* line, SDL_Event* event);
static void textInputMode(Console* console, Editor* editor, Line* line, SDL_Event* event);
static void moveUp(Console* console, Editor* editor, Line* line);
static void moveDown(Console* console, Editor* editor, Line* line);
static void moveLeft(Console* console, Editor* editor, Line* line);
static void moveRight(Console* console, Editor* editor, Line* line);
static void moveFrameUp(Console* console, Editor* editor);
static void moveFrameDown(Console* console, Editor* editor);
static void moveFrameRight(Console* console, Editor* editor);
static void moveFrameLeft(Console* console, Editor* editor);
static void updateFramePosition(Console* console, Editor* editor);

void executeCommand(Editor* editor, Line* line);
void resetCommand(Editor* editor);
void addCharCommand(Editor* editor, char c);

static int seekPreviousSpace(Editor* editor, Line* line);

#endif
