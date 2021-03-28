#ifndef UTILS_H
#define UTILS_H

#include "Line.h"
#include "Common.h"

void save(Line* head, FILE* file);
Line* load(FILE* file);

#endif