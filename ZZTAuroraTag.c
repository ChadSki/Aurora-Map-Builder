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

#include "ZZTAuroraTag.h"
#include <string.h>
#include <stdlib.h>

AuroraTag cloneTag(const AuroraTag tag) {
    AuroraTag newTag;
    
    memcpy(&newTag, &tag, sizeof(AuroraTag));
    
    newTag.path = calloc(sizeof(char), strlen(tag.path) + 1);
    strcpy(newTag.path,tag.path);
    
    if(newTag.rawData) {
        newTag.rawData = malloc(tag.rawDataSize);
        memcpy(newTag.rawData,tag.rawData,tag.rawDataSize);
    }
    
    if(newTag.references) {
        newTag.references = malloc(sizeof(AuroraTagReference) * tag.referenceCount);
        memcpy(newTag.references,tag.references,tag.referenceCount * sizeof(AuroraTagReference));
    }
    
    if(newTag.tagData) {
        newTag.tagData = malloc(tag.tagDataSize);
        memcpy(newTag.tagData,tag.tagData,tag.tagDataSize);
    }
    
    if(newTag.vertices) {
        newTag.vertices = malloc(tag.verticesLength);
        memcpy(newTag.vertices, tag.vertices, tag.verticesLength);
    }
    
    if(newTag.indices) {
        newTag.indices = malloc(tag.indicesLength);
        memcpy(newTag.indices,tag.indices,tag.indicesLength);
    }
    
    return newTag;
}

AuroraTagArray cloneTagArray(const AuroraTagArray tagArray) {
    AuroraTagArray newTagArray;
    newTagArray.tagCount = tagArray.tagCount;
    newTagArray.tags = malloc(sizeof(AuroraTag) * tagArray.tagCount);
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        newTagArray.tags[i] = cloneTag(tagArray.tags[i]);
    }
    return newTagArray;
}

void destroyTag(AuroraTag tag) {
    free(tag.indices);
    free(tag.path);
    free(tag.rawData);
    free(tag.references);
    free(tag.tagData);
    free(tag.vertices);
}

void destroyTagArray(AuroraTagArray tagArray) {
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        destroyTag(tagArray.tags[i]);
    }
    free(tagArray.tags);
}
