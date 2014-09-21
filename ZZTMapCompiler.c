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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ZZTMapCompiler.h"
#include "ZZTTagData.h"
#include "crc32/crc32.h"

void *compileMapFromTagArray(const AuroraTagArray tagArrayOld, uint32_t primaryScenarioTagIndex, uint16_t mapType, uint32_t gameVersion, const char *mapName) {
    /** Duplicate tag array so we don't modify it */
    AuroraTagArray tagArray;
    tagArray.tagCount = tagArrayOld.tagCount;
    tagArray.tags = malloc(sizeof(AuroraTag) * tagArray.tagCount);
    memcpy(tagArray.tags,tagArrayOld.tags,sizeof(AuroraTag) * tagArray.tagCount);
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        if(tagArray.tags[i].tagData != NULL) {
            void *oldData = tagArray.tags[i].tagData;
            tagArray.tags[i].tagData = malloc(tagArray.tags[i].tagDataSize);
            memcpy(tagArray.tags[i].tagData,oldData,tagArray.tags[i].tagDataSize);
        }
    }
    
    HaloMapIndex index;
    
    // Blank out the header.
    HaloMapHeader header;
    memset(&header,0x0,sizeof(header));
    
    // Map integrity stuff.
    memcpy(header.integrityHead,"daeh",4);
    memcpy(header.integrityFoot,"toof",4);
    
    // Game build based on game version.
    if(gameVersion == 5) {
        strcpy(header.builddate,"01.10.12.2276");
    }
    else if(gameVersion == 6) {
        strcpy(header.builddate,"01.00.00.0576");
    }
    else if(gameVersion == 7) {
        strcpy(header.builddate,"01.00.00.0564");
    }
    else if(gameVersion == 0x261) {
        strcpy(header.builddate,"01.00.00.0609");
    }
    
    // Map name stuff.
    strncpy(header.name,mapName,31);
    
    header.type = mapType;
    header.version = gameVersion;
    
    /** Tag names and tag array stuff */
    // Allocate the tag array
    const uint32_t cacheTagArrayMemoryOffset = HALO_META_MEMORY_OFFSET + sizeof(index);
    uint32_t cacheTagArrayLength = sizeof(HaloMapTag) * tagArray.tagCount;
    HaloMapTag *cacheTagArray = calloc(cacheTagArrayLength,1);
    
    // Allocate tag names (these are 8-bit character strings)
    const uint32_t tagNameMemoryOffset = cacheTagArrayMemoryOffset + cacheTagArrayLength;
    uint32_t tagNamesLength = 0;
    uint32_t maxTagNamesLength = 0;
    
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        maxTagNamesLength += strlen(tagArray.tags[i].path) + 1;
    }
    char *tagNames = calloc(sizeof(char),maxTagNamesLength);
    
    // And away we go!
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        memcpy(cacheTagArray[i].classA,tagArray.tags[i].classA,4);
        memcpy(cacheTagArray[i].classB,tagArray.tags[i].classB,4);
        memcpy(cacheTagArray[i].classC,tagArray.tags[i].classC,4);
        cacheTagArray[i].nameOffset = tagNameMemoryOffset + tagNamesLength;
        cacheTagArray[i].identity.tagTableIndex = i;
        cacheTagArray[i].identity.tableIndex = TAG_INDEX_ZERO + i;
        size_t tagNameStrLength = strlen(tagArray.tags[i].path);
        memcpy(tagNames, tagNames, tagNamesLength);
        memcpy(tagNames + tagNamesLength, tagArray.tags[i].path, tagNameStrLength + 1);
        tagNamesLength += tagNameStrLength + 1;
    }
    
    // 32-bit alignment. It looks better on a hex editor
    uint32_t alignAdd = 4 - (tagNamesLength % 4);
    if(alignAdd != 4) {
        char *tagNamesAlign = calloc(sizeof(char),tagNamesLength + alignAdd);
        memcpy(tagNamesAlign,tagNames,tagNamesLength);
        free(tagNames);
        tagNames = tagNamesAlign;
    }
    
    /** Resolve tag dependencies and adjust tag name pointers to match */
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        AuroraTag tag = tagArray.tags[i];
        for(uint32_t j=0;j<tag.referenceCount;j++) {
            if(tag.references[j].type == AURORA_TAG_REFERENCE_DEPENDENCY) {
                Dependency *dependency = tag.tagData + tag.references[j].offset;
                memcpy(dependency->mainClass,cacheTagArray[tag.references[j].tagIndex].classA,4);
                dependency->tagId.tagTableIndex = tag.references[j].tagIndex;
                dependency->tagId.tableIndex = TAG_INDEX_ZERO + tag.references[j].tagIndex;
                dependency->nameOffset = cacheTagArray[tag.references[j].tagIndex].nameOffset;
            }
            else if(tag.references[j].type == AURORA_TAG_REFERENCE_TAGID) {
                TagID *tagID = tag.tagData + tag.references[j].offset;
                tagID->tagTableIndex = tag.references[j].tagIndex;
                tagID->tableIndex = TAG_INDEX_ZERO + tag.references[j].tagIndex;
            }
        }
    }
    
    /** Bsp Stuff. No, really, I can't describe it any better than that */
    // Charge the BSPs!
    const uint32_t bspOffset = sizeof(header);
    uint32_t bspLength = 0;
    uint32_t maxBspLength = 0;
    for(uint32_t i=0; i<tagArray.tagCount; i++) {
        if(memcmp(tagArray.tags[i].classA,SBSP,4) == 0) {
            maxBspLength += tagArray.tags[i].tagDataSize;
        }
    }
    
    // Fire the BSPs!
    void *bspData = malloc(maxBspLength);
    for(uint32_t i=0; i<tagArray.tagCount; i++) {
        if(memcmp(tagArray.tags[i].classA,SBSP,4) == 0) {
            for(uint32_t q=0;q<tagArray.tagCount;q++) {
                if(memcmp(tagArray.tags[q].classA,SCNR,4) == 0) {
                    scnr_tag_structs *scenarioTagData = tagArray.tags[q].tagData;
                    scnr_tag_bsps *scenarioTagBsps = tagArray.tags[q].tagData + scenarioTagData->bsps.pointer - tagArray.tags[q].memoryOffset;
                    memcpy(bspData,bspData,bspLength);
                    memcpy(bspData + bspLength, tagArray.tags[i].tagData, tagArray.tags[i].tagDataSize);
                    uint32_t scenarioTagBspCount = scenarioTagData->bsps.count;
                    for(uint32_t j=0;j<scenarioTagBspCount;j++) {
                        if(scenarioTagBsps[j].bsp_dependency.tagId.tagTableIndex == i) {
                            scenarioTagBsps[j].bsp_file_offset = bspOffset + bspLength;
                            scenarioTagBsps[j].bsp_tag_size = tagArray.tags[i].tagDataSize;
                            scenarioTagBsps[j].bsp_memory_offset = tagArray.tags[i].memoryOffset;
                        }
                    }
                }
            }
            bspLength += tagArray.tags[i].tagDataSize;
        }
    }
    
    /** Raw Data (BITM/SND!) */
    const uint32_t rawDataOffset = bspOffset + bspLength;
    uint32_t rawDataLength = 0;
    uint32_t maxRawDataLength = 0;
    
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        maxRawDataLength += tagArray.tags[i].rawDataSize;
    }
    
    void *rawData = malloc(maxRawDataLength);
    
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        if(tagArray.tags[i].rawData == NULL) continue;
        if(memcmp(tagArray.tags[i].classA,BITM,4) == 0) {
            Reflexive bitmapReflexive = *(Reflexive *)(tagArray.tags[i].tagData + 0x60);
            bitm_tag_bitmap *bitmaps = tagArray.tags[i].tagData + (bitmapReflexive.pointer - tagArray.tags[i].memoryOffset);
            for(uint32_t j=0;j<bitmapReflexive.count;j++) {
                if(((bitmaps[j].flags >> 8) & 1) == 0) {
                    memcpy(rawData + rawDataLength, tagArray.tags[i].rawData + bitmaps[j].pixelOffset, bitmaps[j].pixelCount);
                    bitmaps[j].pixelOffset = rawDataOffset + rawDataLength;
                    rawDataLength += bitmaps[j].pixelCount;
                }
            }
        }
        if(memcmp(tagArray.tags[i].classA,SND,4) == 0) {
            Reflexive soundRangeReflexive = *(Reflexive *)(tagArray.tags[i].tagData + 0x98);
            snd_range *ranges = tagArray.tags[i].tagData + (soundRangeReflexive.pointer - tagArray.tags[i].memoryOffset);
            for(uint32_t q=0;q<soundRangeReflexive.count;q++) {
                snd_range_permutation *permutations = tagArray.tags[i].tagData + (ranges[q].permutations.pointer - tagArray.tags[i].memoryOffset);
                for(uint32_t j=0;j<ranges[q].permutations.count;j++) {
                    if((permutations[j].flags & 1) == 0) {
                        memcpy(rawData + rawDataLength, tagArray.tags[i].rawData + permutations[j].offset, permutations[j].samples);
                        permutations[j].offset = rawDataOffset + rawDataLength;
                        rawDataLength += permutations[j].samples;
                    }
                }
            }
        }
    }
    
    /** Model data */
    const uint32_t modelDataOffset = rawDataOffset + rawDataLength;
    
    uint32_t verticesSize = 0;
    uint32_t maxVerticesSize = 0;
    
    uint32_t indicesSize = 0;
    uint32_t maxIndicesSize = 0;
    
    uint32_t partCount = 0;
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        maxVerticesSize += tagArray.tags[i].verticesLength;
        maxIndicesSize += tagArray.tags[i].indicesLength;
    }
    
    void *vertices = malloc(maxVerticesSize);
    void *indices = malloc(maxIndicesSize);
    
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        if(memcmp(tagArray.tags[i].classA,MOD2,4) == 0) {
            Reflexive geometriesReflexive = *(Reflexive *)(tagArray.tags[i].tagData + 0xD0);
            mod2_geo *geo = tagArray.tags[i].tagData + (geometriesReflexive.pointer - tagArray.tags[i].memoryOffset);
            for(uint32_t q=0;q<geometriesReflexive.count;q++) {
                mod2_geo_part *parts = tagArray.tags[i].tagData + (geo[q].parts.pointer - tagArray.tags[i].memoryOffset);
                for(uint32_t j=0;j<geo[q].parts.count;j++) {
                    uint32_t vertexSize = 0x44 * parts[j].vertex.count;
                    memcpy(vertices + verticesSize, tagArray.tags[i].vertices + parts[j].vertex.offset, vertexSize);
                    parts[j].vertex.offset = verticesSize;
                    verticesSize += vertexSize;
                    
                    uint32_t indexSize = 0x4 + 0x2 * parts[j].index.count;
                    memcpy(indices + indicesSize, tagArray.tags[i].indices + parts[j].index.offsetA, indexSize);
                    parts[j].index.offsetA = indicesSize;
                    parts[j].index.offsetB = indicesSize;
                    indicesSize += indexSize;
                    
                    partCount++;
                }
            }
        }
    }
    
    /** Tag data */
    const uint32_t tagDataMemoryLocation = tagNameMemoryOffset + tagNamesLength;
    
    uint32_t tagDataLength = 0;
    uint32_t tagDataMaxLength = 0;
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        tagDataMaxLength += tagArray.tags[i].tagDataSize;
    }
    void *tagData = malloc(tagDataMaxLength);
    
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        if(tagArray.tags[i].indexed == 1) {
            cacheTagArray[i].notInsideMap = 1;
            cacheTagArray[i].dataOffset = tagArray.tags[i].index;
        }
        else if(memcmp(tagArray.tags[i].classA,SBSP,4) == 0) {
            //In case I'll ever need to modify the sbsp data.
        }
        else if(memcmp(tagArray.tags[i].classA,SCNR,4) == 0) {
            uint32_t newMemoryOffset = tagDataMemoryLocation + tagDataLength;
            int32_t difference = (int32_t)(tagArray.tags[i].memoryOffset - newMemoryOffset);
            scnr_tag_structs *scnrStuff = tagArray.tags[i].tagData;
            
#define checkAddScnrReflexive(scnrReflexive) if(scnrStuff->scnrReflexive.count != 0) scnrStuff->scnrReflexive.pointer -= difference
            
            checkAddScnrReflexive(skies);
            checkAddScnrReflexive(child_scenarios);
            checkAddScnrReflexive(predicted_resources);
            checkAddScnrReflexive(functions);
            
            for(uint32_t j=0;j<scnrStuff->comments.count;j++) {
                void *comments = (tagArray.tags[i].tagData + (scnrStuff->comments.pointer - tagArray.tags[i].memoryOffset) + j * 48);
                Reflexive *comment = (comments + 0x24);
                if(comment->count != 0) comment->pointer -= difference;
            }
            
            scnrStuff->unknown_pointer -= difference;
            scnrStuff->script_pointer -= difference;
            scnrStuff->script_pointer2 -= difference;
            
            checkAddScnrReflexive(comments);
            checkAddScnrReflexive(objectNames);
            checkAddScnrReflexive(scenery);
            checkAddScnrReflexive(sceneryPalette);
            checkAddScnrReflexive(bipeds);
            checkAddScnrReflexive(bipedPalette);
            checkAddScnrReflexive(vehicle);
            checkAddScnrReflexive(vehiclePalette);
            checkAddScnrReflexive(equipment);
            checkAddScnrReflexive(equipmentPalette);
            checkAddScnrReflexive(weapons);
            checkAddScnrReflexive(weaponPalette);
            checkAddScnrReflexive(deviceGroups);
            checkAddScnrReflexive(machines);
            checkAddScnrReflexive(machinePalette);
            checkAddScnrReflexive(controls);
            checkAddScnrReflexive(controlPalette);
            checkAddScnrReflexive(lightFixtures);
            checkAddScnrReflexive(lightFixturesPalette);
            checkAddScnrReflexive(soundScenery);
            checkAddScnrReflexive(soundSceneryPalette);
            checkAddScnrReflexive(startingProfile);
            checkAddScnrReflexive(startingLocations);
            checkAddScnrReflexive(triggerVolumes);
            checkAddScnrReflexive(recordedAnimations);
            checkAddScnrReflexive(netgameFlags);
            checkAddScnrReflexive(netgameEquipment);
            checkAddScnrReflexive(startingEquipment);
            checkAddScnrReflexive(bspTriggerVolumes);
            checkAddScnrReflexive(decals);
            checkAddScnrReflexive(decalPalette);
            checkAddScnrReflexive(dobcPalette);
            checkAddScnrReflexive(actorPalette);
            
            for(uint32_t j=0;j<scnrStuff->encounters.count;j++) {
                void *encounter = (tagArray.tags[i].tagData + (scnrStuff->encounters.pointer - tagArray.tags[i].memoryOffset) + j * 176);
                Reflexive *squads = (encounter + 0x80);
                for(uint32_t q=0;q<squads->count;q++) {
                    void *squad = (tagArray.tags[i].tagData + (squads->pointer - tagArray.tags[i].memoryOffset) + q * 232);
                    Reflexive *movePositions = squad + 0xC4;
                    if(movePositions->count != 0) movePositions->pointer -= difference;
                    
                    Reflexive *startingLocations = squad + 0xD0;
                    if(startingLocations->count != 0) startingLocations->pointer -= difference;
                }
                if(squads->count != 0) squads->pointer -= difference;
                Reflexive *platoons = (encounter + 0x8C);
                if(platoons->count != 0) platoons->pointer -= difference;
                Reflexive *firingPositions = (encounter + 0x98);
                if(firingPositions->count != 0) firingPositions->pointer -= difference;
                Reflexive *startingLocations = (encounter + 0xA4);
                if(startingLocations->count != 0) startingLocations->pointer -= difference;
            }
            
            checkAddScnrReflexive(encounters);
            
            for(uint32_t j=0;j<scnrStuff->commandLists.count;j++) {
                void *commandList = (tagArray.tags[i].tagData + (scnrStuff->commandLists.pointer - tagArray.tags[i].memoryOffset) + j * 96);
                Reflexive *commands = (commandList + 0x30);
                if(commands->count != 0) commands->pointer -= difference;
            }
            checkAddScnrReflexive(commandLists);
            checkAddScnrReflexive(aiAnimationReferences);
            checkAddScnrReflexive(aiScriptReferences);
            checkAddScnrReflexive(aiRecordingReferences);
            
            for(uint32_t j=0;j<scnrStuff->aiConversations.count;j++) {
                void *aiConversation =(tagArray.tags[i].tagData + (scnrStuff->aiConversations.pointer - tagArray.tags[i].memoryOffset) + j * 116);
                Reflexive *participants = aiConversation + 0x50;
                if(participants->count != 0) participants->pointer -= difference;
                Reflexive *lines = aiConversation + 0x5C;
                if(lines->count != 0) lines->pointer -= difference;
            }
            checkAddScnrReflexive(aiConversations);
            
            checkAddScnrReflexive(scripts);
            checkAddScnrReflexive(globals);
            checkAddScnrReflexive(references);
            checkAddScnrReflexive(cutsceneFlags);
            checkAddScnrReflexive(cutsceneCameraPoints);
            checkAddScnrReflexive(cutsceneTitles);
            checkAddScnrReflexive(bsps);
            
            cacheTagArray[i].dataOffset = newMemoryOffset;
            memcpy(tagData + tagDataLength, tagArray.tags[i].tagData, tagArray.tags[i].tagDataSize);
            tagDataLength += tagArray.tags[i].tagDataSize;
        }
        else {
            uint32_t newMemoryOffset = tagDataMemoryLocation + tagDataLength;
            int32_t difference = (int32_t)(tagArray.tags[i].memoryOffset - newMemoryOffset);
            for(uint32_t j=0;(j + sizeof(Reflexive))<tagArray.tags[i].tagDataSize;j+= 4) {
                Reflexive *reflexive = tagArray.tags[i].tagData + j;
                if(reflexive->pointer > tagArray.tags[i].memoryOffset && reflexive->pointer < (tagArray.tags[i].memoryOffset + tagArray.tags[i].tagDataSize) && reflexive->padding == 0) {
                    reflexive->pointer -= difference;
                }
            }
            cacheTagArray[i].dataOffset = newMemoryOffset;
            memcpy(tagData + tagDataLength, tagArray.tags[i].tagData, tagArray.tags[i].tagDataSize);
            tagDataLength += tagArray.tags[i].tagDataSize;
        }
    }
    
    
    
    /** Put the map together */
    uint32_t fileSize = sizeof(header);
    fileSize += bspLength;
    fileSize += rawDataLength;
    fileSize += verticesSize + indicesSize;
    
    fileSize += 4 - ((verticesSize + indicesSize) % 4);
    
    uint32_t metaSize = sizeof(index);
    metaSize += cacheTagArrayLength;
    metaSize += tagNamesLength;
    metaSize += tagDataLength;
    
    fileSize += metaSize;
    
    void *cacheFile = calloc(fileSize,1);
    
    uint32_t location = sizeof(header);
    memcpy(cacheFile + location, bspData, bspLength);
    
    location += bspLength;
    memcpy(cacheFile + location, rawData, rawDataLength);
    
    location += rawDataLength;
    memcpy(cacheFile + location, vertices, verticesSize);
    
    location += verticesSize;
    memcpy(cacheFile + location, indices, indicesSize);
    
    location += indicesSize;
    location += 4 - (location % 4);
    
    uint32_t metaOffset = location;
    header.indexOffset = metaOffset;
    header.metaSize = fileSize - metaOffset;
    header.length = fileSize;
    
    
    void *metaData = calloc(metaSize,1);
    
    index.tagIndexOffset = HALO_META_MEMORY_OFFSET + sizeof(index);
    index.vertexOffset = modelDataOffset;
    index.scenarioTag.tagTableIndex = primaryScenarioTagIndex;
    index.scenarioTag.tableIndex = TAG_INDEX_ZERO + primaryScenarioTagIndex;
    index.tagCount = tagArray.tagCount;
    index.modelSize = verticesSize + indicesSize;
    index.vertexSize = verticesSize;
    index.partCountA = partCount;
    index.partCountB = partCount;
    memcpy(index.tagsString,"sgat",4);
    
    uint32_t metaLocation = 0;
    memcpy(metaData,&index,sizeof(index));
    
    metaLocation += sizeof(index);
    memcpy(metaData + metaLocation, cacheTagArray, cacheTagArrayLength);
    
    metaLocation += cacheTagArrayLength;
    memcpy(metaData + metaLocation, tagNames, tagNamesLength);
    
    metaLocation += tagNamesLength;
    memcpy(metaData + metaLocation, tagData, tagDataLength);
    
    metaLocation += tagDataLength;
    memcpy(cacheFile + location, metaData, metaLocation);
    
    header.crc32 = 0xFFFFFFFF - crc32(0, cacheFile + sizeof(header), fileSize - sizeof(header));
    memcpy(cacheFile, &header, sizeof(header));
    
    /** Destroy allocated stuff */
    for(uint32_t i=0;i<tagArray.tagCount;i++) {
        free(tagArray.tags[i].tagData);
    }
    free(tagArray.tags);
    free(vertices);
    free(indices);
    free(rawData);
    free(bspData);
    free(cacheTagArray);
    free(tagNames);
    
    return cacheFile;
}