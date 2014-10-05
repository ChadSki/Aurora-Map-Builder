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

#include "ZZTTagImporter.h"
#include "../ZZTMap.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAG_NOT_FOUND 0xFFFFFFFF

static uint32_t searchForTagInTagArray(const AuroraTagArray *tagArray, const AuroraTag tagToSearch) {
    for(uint32_t i=0;i<tagArray->tagCount;i++) {
        if(memcmp(tagArray->tags[i].classA,tagToSearch.classA,4) == 0 && strcmp(tagArray->tags[i].path,tagToSearch.path) == 0) {
            return i;
        }
    }
    return TAG_NOT_FOUND;
}

void importTagsIntoTagArray(AuroraTagArray *tagArray, const AuroraTagArray tagsToImport, bool recursive, bool allowDuplicates, uint32_t mainTag) {
    AuroraTagArray tagsToAdd;
    
    if(recursive) {
        if(allowDuplicates) {
            tagsToAdd = cloneTagArray(tagsToImport);
            for(uint32_t i=0;i<tagsToAdd.tagCount;i++) {
                if(searchForTagInTagArray(tagArray, tagsToAdd.tags[i]) != TAG_NOT_FOUND) {
                    char *path = calloc(strlen(tagsToAdd.tags[i].path) + 10,sizeof(char));
                    sprintf(path,"%s %u",tagsToAdd.tags[i].path,tagArray->tagCount + i);
                    free(tagsToAdd.tags[i].path);
                    tagsToAdd.tags[i].path = path;
                }
                for(uint32_t j=0;j<tagsToAdd.tags[i].referenceCount;j++) {
                    tagsToAdd.tags[i].references[j].tagIndex += tagArray->tagCount;
                }
            }
        }
        else {
            bool *destroyedTags = calloc(sizeof(bool),tagsToImport.tagCount);
            AuroraTagArray tagsToImportCopy = cloneTagArray(tagsToImport);
            tagsToAdd.tags = malloc(sizeof(AuroraTag) * tagsToImport.tagCount);
            tagsToAdd.tagCount = 0;
            for(uint32_t i=0;i<tagsToImport.tagCount;i++) {
                uint32_t tagFound = searchForTagInTagArray(tagArray, tagsToImport.tags[i]);
                if(tagFound == TAG_NOT_FOUND) {
                    tagsToAdd.tags[tagsToAdd.tagCount] = tagsToImportCopy.tags[i];
                    for(uint32_t j=0;j<tagsToImport.tagCount;j++) {
                        for(uint32_t k=0;k<tagsToImport.tags[j].referenceCount;k++) {
                            if(tagsToImport.tags[j].references[k].tagIndex == i) {
                                tagsToImportCopy.tags[j].references[k].tagIndex = tagArray->tagCount + tagsToAdd.tagCount;
                            }
                        }
                    }
                    tagsToAdd.tagCount ++;
                }
                else {
                    for(uint32_t j=0;j<tagsToImport.tagCount;j++) {
                        for(uint32_t k=0;k<tagsToImport.tags[j].referenceCount;k++) {
                            if(tagsToImport.tags[j].references[k].tagIndex == i) {
                                tagsToImportCopy.tags[j].references[k].tagIndex = tagFound;
                            }
                        }
                    }
                    destroyedTags[i] = true;
                }
            }
            for(uint32_t i=0;i<tagsToImport.tagCount;i++) {
                if(destroyedTags[i]) destroyTag(tagsToImportCopy.tags[i]);
            }
            free(destroyedTags);
        }
    }
    else {
        bool isADuplicate = searchForTagInTagArray(tagArray, tagsToImport.tags[mainTag]) != TAG_NOT_FOUND;
        if(!allowDuplicates && isADuplicate) {
            return;
        }
        tagsToAdd.tagCount = 1;
        tagsToAdd.tags = malloc(sizeof(AuroraTag));
        tagsToAdd.tags[0] = cloneTag(tagsToImport.tags[mainTag]);
        for(uint32_t i=0;i<tagsToAdd.tags->referenceCount;i++) {
            AuroraTag tagCheck = tagsToImport.tags[tagsToAdd.tags->references[i].tagIndex];
            tagsToAdd.tags->references[i].tagIndex = searchForTagInTagArray(tagArray, tagCheck);
        }
        if(isADuplicate) {
            char *path = calloc(strlen(tagsToAdd.tags->path) + 8,sizeof(char));
            sprintf(path,"%s %u",tagsToAdd.tags->path,tagArray->tagCount);
            free(tagsToAdd.tags->path);
            tagsToAdd.tags->path = path;
        }
        AuroraTagReference *references = malloc(tagsToAdd.tags->referenceCount * sizeof(AuroraTagReference));
        uint32_t currentReference = 0;
        for(uint32_t i=0;i<tagsToAdd.tags->referenceCount;i++) {
            if(tagsToAdd.tags->references[i].tagIndex == TAG_NOT_FOUND) {
                void *dataPointer = tagsToAdd.tags->tagData + tagsToAdd.tags->references[i].offset;
                if(tagsToAdd.tags->references[i].type == AURORA_TAG_REFERENCE_DEPENDENCY) {
                    Dependency *dependency = dataPointer;
                    dependency->tagId.tableIndex = 0xFFFF;
                    dependency->tagId.tagTableIndex = 0xFFFF;
                }
                else if(tagsToAdd.tags->references[i].type == AURORA_TAG_REFERENCE_TAGID) {
                    TagID *tagId = dataPointer;
                    tagId->tableIndex = 0xFFFF;
                    tagId->tagTableIndex = 0xFFFF;
                }
            }
            else {
                references[currentReference] = tagsToAdd.tags->references[i];
                currentReference++;
            }
        }
        free(tagsToAdd.tags->references);
        tagsToAdd.tags->references = references;
        tagsToAdd.tags->referenceCount = currentReference;
    }
    uint32_t tagCount = tagArray->tagCount + tagsToAdd.tagCount;
    AuroraTag *tags = malloc(sizeof(AuroraTag) * tagCount);
    memcpy(tags,tagArray->tags,tagArray->tagCount * sizeof(AuroraTag));
    memcpy(tags + tagArray->tagCount,tagsToAdd.tags,tagsToAdd.tagCount * sizeof(AuroraTag));
    tagArray->tagCount = tagCount;
    tagArray->tags = tags;
}
