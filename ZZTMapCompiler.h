/**
The MIT License (MIT)

Copyright (c) 2014 Paul Whitcomb

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/



#include "ZZTMapFormat.h"

#ifndef Aurora_ZZTMapCompiler_h
#define Aurora_ZZTMapCompiler_h

typedef enum MapType {
    MAP_TYPE_SINGLE_PLAYER = 0,
    MAP_TYPE_MULTIPLAYER = 1,
    MAP_TYPE_USER_INTERFACE = 2,
} MapType;

typedef enum GameVersion {
    GAME_VERSION_RETAIL = 0x7,
    GAME_VERSION_CUSTOM_EDITION = 0x261
} GameVersion;

void *compileMapFromTagArray(const AuroraTagArray tagArray, uint32_t primaryScenarioTagIndex, uint16_t mapType, uint32_t gameVersion, const char *mapName);

#endif
