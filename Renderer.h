#ifndef RENDERER_H
#define RENDERER_H

#include "Console.h"
#include "Editor.h"
#include "Line.h"

void drawEditor(Console* console, Editor* editor, Line* line);

static void drawTextInputMode(Console* console, Editor* editor, Line* line);
static void drawCommandMode(Console* console, Editor* editor);
static void drawCommandTextMode(Console* console, Editor* editor);
static void drawLineNumber(Console* console, Editor* editor, int line_number);

static bool checkKeywordPolarite(char* buffer, int i, int k);
static bool checkKeywordC(char* buffer, int i, int k);
static bool checkKeywordCPP(char* buffer, int i, int k);
static bool checkKeywordRust(char* buffer, int i, int k);
static bool checkKeywordPython(char* buffer, int i, int k);
static void number(char* buffer, int* k);
static void identifier(char* buffer, int* k);
static void string(char* buffer, int* k);
static void comment(char* buffer, int* k);
static bool checkComment(char* buffer, int k, Language lang);

#endif
