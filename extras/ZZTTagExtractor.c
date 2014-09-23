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

#include "ZZTTagExtractor.h"
#include <stdlib.h>
#include <string.h>

static void insertTagIntoTagArray(AuroraTagArray *tagArray, AuroraTag tag) {
    AuroraTag *newTags = malloc(sizeof(AuroraTag) * (tagArray->tagCount + 1));
    memcpy(newTags,tagArray->tags,sizeof(AuroraTag) * tagArray->tagCount);
    memcpy(newTags + tagArray->tagCount,&tag,sizeof(tag));
    free(tagArray->tags);
    tagArray->tags = newTags;
    tagArray->tagCount ++;
}

static void recursivelyInsertTagsFromTagArray(const AuroraTagArray tagArray, AuroraTagArray *newTagArray, uint32_t tag, uint32_t *translateTagIndex) {
    if(translateTagIndex[tag]) return;
    AuroraTag newTag = cloneTag(tagArray.tags[tag]);
    insertTagIntoTagArray(newTagArray, newTag);
    translateTagIndex[tag] = newTagArray->tagCount;
    for(uint32_t i=0;i<newTag.referenceCount;i++) {
        recursivelyInsertTagsFromTagArray(tagArray, newTagArray, newTag.references[i].tagIndex, translateTagIndex);
    }
}

AuroraTagArray recursivelyExtractTagsFromTagArray(const AuroraTagArray tagArray, uint32_t primaryTag) {
    AuroraTagArray extractedTags;
    memset(&extractedTags,0,sizeof(extractedTags));
    
    uint32_t *translateTagIndex = calloc(sizeof(uint32_t),tagArray.tagCount);
    
    recursivelyInsertTagsFromTagArray(tagArray, &extractedTags, primaryTag, translateTagIndex);
    
    for(uint32_t i=0;i<extractedTags.tagCount;i++) {
        for(uint32_t q=0;q<extractedTags.tags[i].referenceCount;q++) {
            extractedTags.tags[i].references[q].tagIndex = translateTagIndex[extractedTags.tags[i].references[q].tagIndex] - 1;
        }
    }
    
    
    return extractedTags;
}
