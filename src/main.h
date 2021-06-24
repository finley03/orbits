// file for globally accessible varaibles and parameters
// should not be included in files containing definitions
// of these classes
// instead these files should include "util.h"

#ifndef MAIN_H
#define MAIN_H

#include "util.h"

// includes important for all files

#include "screen.h"
#include "shaders.h"
#include "time.h"

// external declarations of global object pointers

extern Screen* screen;
extern Timer* frameTimer;

#endif