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


#ifdef __cplusplus
extern "C" {
#endif

#ifndef Aurora_ZZTMapFormat_h
#define Aurora_ZZTMapFormat_h
    
#include <stdint.h>

typedef enum AuroraTagReferenceType {
    AURORA_TAG_REFERENCE_TAGID,
    AURORA_TAG_REFERENCE_DEPENDENCY
} AuroraTagReferenceType;

typedef struct AuroraTagReference {
    uint32_t tagIndex;
    uint32_t offset;
    AuroraTagReferenceType type;
} AuroraTagReference;

typedef struct AuroraTag {
    char *path;
    char classA[4];
    char classB[4];
    char classC[4];
    uint32_t memoryOffset;
    void *tagData;
    uint32_t tagDataSize;
    void *rawData;
    uint32_t rawDataSize;
    void *vertices;
    uint32_t verticesLength;
    void *indices;
    uint32_t indicesLength;
    AuroraTagReference *references;
    uint32_t referenceCount;
    uint32_t indexed;
    uint32_t index;
} AuroraTag;

typedef struct AuroraTagArray {
    AuroraTag *tags;
    uint32_t tagCount;
} AuroraTagArray;

#endif
    
    
#ifdef __cplusplus
}
#endif
