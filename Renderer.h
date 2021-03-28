#ifndef RENDERER_H
#define RENDERER_H

#include "Console.h"
#include "Editor.h"
#include "Line.h"

void drawEditor(Console* console, Editor* editor, Line* line);

static void drawTextInputMode(Console* console, Editor* editor, Line* line);
static void drawCommandMode(Console* console, Editor* editor);
static void drawCommandTextMode(Console* console, Editor* editor);

#endif
