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
#include "ZZTMapDecompiler.h"
#include "ZZTTagData.h"

static void addReferenceToTag(AuroraTagReference reference,AuroraTag *tag) {
    AuroraTagReference *new_references = malloc(sizeof(AuroraTagReference) * (tag->referenceCount + 1));
    memcpy(new_references,tag->references,sizeof(AuroraTagReference) * tag->referenceCount);
    memcpy(new_references + tag->referenceCount, &reference,sizeof(reference));
    free(tag->references);
    tag->references = new_references;
    tag->referenceCount ++;
}

static void addTagIDDependencyToTag(uint32_t offset,AuroraTag *tag) {
    TagID *dependency = (TagID *)(tag->tagData + offset);
    AuroraTagReference reference;
    reference.offset = offset;
    if(dependency->tagTableIndex == 0xFFFF) return;
    reference.type = AURORA_TAG_REFERENCE_TAGID;
    reference.tagIndex = dependency->tagTableIndex;
    addReferenceToTag(reference, tag);
}

static void addDependencyToTag(uint32_t offset, AuroraTag *tag) {
    Dependency *dependency = (Dependency *)(tag->tagData + offset);
    AuroraTagReference reference;
    reference.offset = offset;
    if(dependency->tagId.tagTableIndex == 0xFFFF) return;
    reference.type = AURORA_TAG_REFERENCE_DEPENDENCY;
    reference.tagIndex = dependency->tagId.tagTableIndex;
    addReferenceToTag(reference, tag);
}

static void addReflexiveDependencyToTag(Reflexive reflexive, AuroraTag *tag, uint32_t structSize, uint32_t referenceOffset) {
    uint32_t structOffset = reflexive.pointer - tag->memoryOffset;
    for(uint32_t i=0;i<reflexive.count;i++) {
        addDependencyToTag(structOffset + referenceOffset + i * structSize, tag);
    }
}

static void addRawDataToTag(const void *rawData, uint32_t length, AuroraTag *tag) {
    void *newRawData = malloc(tag->rawDataSize + length);
    memcpy(newRawData,tag->rawData,tag->rawDataSize);
    memcpy(newRawData + tag->rawDataSize, rawData, length);
    tag->rawDataSize += length;
    free(tag->rawData);
    tag->rawData = newRawData;
}

uint32_t translateMemoryOffsetFileOffset(uint32_t memoryOffset, uint32_t magic, uint32_t fileOffset) {
    return memoryOffset - magic + fileOffset;
}

uint32_t translateMemoryOffset(uint32_t memoryOffset, uint32_t magic) {
    return translateMemoryOffsetFileOffset(memoryOffset, magic,0);
}

AuroraTagArray decompileMapFromBuffer(const void *map) {
    AuroraTagArray tagArray;
    HaloMapHeader *mapHeader = (HaloMapHeader *)map;
    HaloMapIndex *mapIndex = (HaloMapIndex *)(map + mapHeader->indexOffset);
    
    uint32_t mapMagic = HALO_META_MEMORY_OFFSET - mapHeader->indexOffset;
    
    AuroraTag *tags = calloc(sizeof(AuroraTag), mapIndex->tagCount);
    tagArray.tags = tags;
    tagArray.tagCount = mapIndex->tagCount;
    
    const HaloMapTag *mapTags = map + translateMemoryOffset(mapIndex->tagIndexOffset, mapMagic);
    
    const void *verticesOffset = map + mapIndex->vertexOffset;
    const void *indicesOffset = verticesOffset + mapIndex->vertexSize;
    
    uint32_t scnrTagBspsCount = 0;
    for(uint32_t i=0;i<mapIndex->tagCount;i++) {
        if(memcmp(mapTags[i].classA,SCNR,4) == 0) {
            const scnr_tag_structs *scenarioTagData = map + translateMemoryOffset(mapTags[i].dataOffset, mapMagic);
            scnrTagBspsCount += scenarioTagData->bsps.count;
        }
    }
    scnr_tag_bsps *scenarioTagBsps = malloc(sizeof(scnr_tag_bsps) * scnrTagBspsCount);
    uint32_t location = 0;
    for(uint32_t i=0;i<mapIndex->tagCount;i++) {
        if(memcmp(mapTags[i].classA,SCNR,4) == 0) {
            const scnr_tag_structs *scenarioTagData = map + translateMemoryOffset(mapTags[i].dataOffset, mapMagic);
            const scnr_tag_bsps *currentScenarioTagBsps = map + translateMemoryOffset(scenarioTagData->bsps.pointer, mapMagic);
            memcpy(scenarioTagBsps + location, currentScenarioTagBsps, sizeof(scnr_tag_bsps) * scenarioTagData->bsps.count);
            scnrTagBspsCount += scenarioTagData->bsps.count;
        }
    }
    
    
    
    for(uint32_t i=0;i<mapIndex->tagCount;i++) {
        const char *tagPath = map + translateMemoryOffset(mapTags[i].nameOffset, mapMagic);
        if((void *)tagPath < map + mapHeader->indexOffset || (void *)tagPath >= map + mapHeader->length) {
            tags[i].path = calloc(sizeof(char),30);
        }
        else {
            tags[i].path = calloc(strlen(tagPath) + 1, 1);
            strcpy(tags[i].path,tagPath);
        }
        
        memcpy(tags[i].classA, mapTags[i].classA, 4);
        memcpy(tags[i].classB, mapTags[i].classB, 4);
        memcpy(tags[i].classC, mapTags[i].classC, 4);
        
        /** Store tag data */
        if(mapTags[i].notInsideMap == 1) {
            tags[i].indexed = 1;
            tags[i].index = mapTags[i].dataOffset;
            continue; // No more need to read this tag if there's no data.
        }
        else if(memcmp(tags[i].classA,SBSP,4) == 0) {
            for(uint32_t j=0;j<scnrTagBspsCount;j++) {
                if(scenarioTagBsps[j].bsp_dependency.tagId.tagTableIndex == i) {
                    tags[i].tagDataSize = scenarioTagBsps[j].bsp_tag_size;
                    tags[i].tagData = malloc(tags[i].tagDataSize);
                    tags[i].memoryOffset = scenarioTagBsps[j].bsp_memory_offset;
                    const void *bspData = map + scenarioTagBsps[j].bsp_file_offset;
                    memcpy(tags[i].tagData,bspData,tags[i].tagDataSize);
                    break;
                }
            }
        }
        else {
            uint32_t tagSize = (0x40440000 + mapHeader->metaSize) - mapTags[i].dataOffset;
            for(uint32_t j=0;j<mapIndex->tagCount;j++) {
                if(mapTags[j].dataOffset <= mapTags[i].dataOffset) continue;
                uint32_t difference = mapTags[j].dataOffset - mapTags[i].dataOffset;
                if(difference < tagSize) {
                    tagSize = difference;
                }
            }
            tags[i].tagDataSize = tagSize;
            tags[i].tagData = malloc(tagSize);
            memcpy(tags[i].tagData, map + translateMemoryOffset(mapTags[i].dataOffset, mapMagic), tagSize);
            tags[i].memoryOffset = mapTags[i].dataOffset;
        }
        
        /** Scan for references */
        if(memcmp(tags[i].classA,SCNR,4) == 0) {
            scnr_tag_structs *scnrTag = tags[i].tagData;
            addDependencyToTag(0x0, tags + i);
            addDependencyToTag(0x10, tags + i);
            addDependencyToTag(0x20, tags + i);
            addReflexiveDependencyToTag(scnrTag->skies, tags + i, 16, 0x0);
            addReflexiveDependencyToTag(scnrTag->child_scenarios, tags + i, 32, 0x0);
            uint32_t resourcesOffset = scnrTag->predicted_resources.pointer - tags[i].memoryOffset;
            for(uint32_t j=0;j<scnrTag->predicted_resources.count;j++) {
                addTagIDDependencyToTag(resourcesOffset + 0x4 + 0x8 * j, tags + i);
            }
            addReflexiveDependencyToTag(scnrTag->sceneryPalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->bipedPalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->vehiclePalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->equipmentPalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->weaponPalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->machinePalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->controlPalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->lightFixturesPalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->soundSceneryPalette, tags + i, 48, 0x0);
            
            uint32_t startingProfileOffset = scnrTag->startingProfile.pointer - tags[i].memoryOffset;
            for(uint32_t j=0;j<scnrTag->startingProfile.count;j++) {
                uint32_t location = startingProfileOffset + j * 104;
                addDependencyToTag(location + 0x28, tags + i);
                addDependencyToTag(location + 0x3C, tags + i);
            }
            
            addReflexiveDependencyToTag(scnrTag->netgameFlags, tags + i, 148, 0x14);
            addReflexiveDependencyToTag(scnrTag->netgameEquipment, tags + i, 144, 0x50);
            
            uint32_t startingEquipmentOffset = scnrTag->startingEquipment.pointer - tags[i].memoryOffset;
            for(uint32_t j=0;j<scnrTag->startingEquipment.count;j++) {
                uint32_t location = startingEquipmentOffset + j * 204;
                addDependencyToTag(location + 0x3C, tags + i);
                addDependencyToTag(location + 0x4C, tags + i);
                addDependencyToTag(location + 0x5C, tags + i);
                addDependencyToTag(location + 0x6C, tags + i);
                addDependencyToTag(location + 0x7C, tags + i);
                addDependencyToTag(location + 0x8C, tags + i);
            }
            
            addReflexiveDependencyToTag(scnrTag->decalPalette, tags + i, 16, 0x0);
            addReflexiveDependencyToTag(scnrTag->dobcPalette, tags + i, 48, 0x0);
            addReflexiveDependencyToTag(scnrTag->actorPalette, tags + i, 16, 0x0);
            
            addReflexiveDependencyToTag(scnrTag->aiAnimationReferences, tags + i, 60, 0x20);
            
            void *aiConversations = tags[i].tagData + (scnrTag->aiConversations.pointer - tags[i].memoryOffset);
            for(uint32_t q=0;q<scnrTag->aiConversations.count;q++) {
                Reflexive lines = *(Reflexive *)(aiConversations + q * 116 + 0x5C);
                uint32_t linesOffset = lines.pointer - tags[i].memoryOffset;
                for(uint32_t j=0;j<lines.count;j++) {
                    uint32_t location = linesOffset + j * 124;
                    addDependencyToTag(location + 0x1C, tags + i);
                    addDependencyToTag(location + 0x2C, tags + i);
                    addDependencyToTag(location + 0x3C, tags + i);
                    addDependencyToTag(location + 0x4C, tags + i);
                    addDependencyToTag(location + 0x5C, tags + i);
                    addDependencyToTag(location + 0x6C, tags + i);
                }
            }
            
            addReflexiveDependencyToTag(scnrTag->references, tags + i, 40, 0x18);
            
            addReflexiveDependencyToTag(scnrTag->bsps, tags + i, 32, 0x10);
        }
        else if(memcmp(tags[i].classA, BITM, 4) == 0) {
            Reflexive bitmapsReflexive = *(Reflexive *)(tags[i].tagData + 0x60);
            bitm_tag_bitmap *bitmaps = tags[i].tagData + (bitmapsReflexive.pointer - tags[i].memoryOffset);
            for(uint32_t j=0;j<bitmapsReflexive.count;j++) {
                addTagIDDependencyToTag(bitmapsReflexive.pointer - tags[i].memoryOffset + 0x2C, tags + i);
                if(((bitmaps[j].flags >> 8) & 1) == 0) {
                    uint32_t oldCount = tags[i].rawDataSize;
                    addRawDataToTag(map + bitmaps[j].pixelOffset, bitmaps[j].pixelCount, tags + i);
                    bitmaps[j].pixelOffset = oldCount;
                }
            }
        }
        else if(memcmp(tags[i].classA, SND, 4) == 0) {
            Reflexive soundRangeReflexive = *(Reflexive *)(tags[i].tagData + 0x98);
            snd_range *soundRanges = tags[i].tagData + (soundRangeReflexive.pointer - tags[i].memoryOffset);
            for(uint32_t q=0;q<soundRangeReflexive.count;q++) {
                snd_range_permutation *soundRangePermutations = tags[i].tagData + (soundRanges[q].permutations.pointer - tags[i].memoryOffset);
                for(uint32_t j=0;j<soundRanges[q].permutations.count;j++) {
                    addTagIDDependencyToTag(soundRanges[q].permutations.pointer - tags[i].memoryOffset + 0x34, tags + i);
                    addTagIDDependencyToTag(soundRanges[q].permutations.pointer - tags[i].memoryOffset + 0x3C, tags + i);
                    if((soundRangePermutations[j].flags & 1) == 0) {
                        uint32_t oldCount = tags[i].rawDataSize;
                        addRawDataToTag(map + soundRangePermutations[j].offset, soundRangePermutations[j].samples, tags + i);
                        soundRangePermutations[j].offset = oldCount;
                    }
                }
            }
        }
        else if(memcmp(tags[i].classA, MOD2, 4) == 0) {
            Reflexive geoReflexive = *(Reflexive *)(tags[i].tagData + 0xD0);
            mod2_geo *geos = tags[i].tagData + (geoReflexive.pointer - tags[i].memoryOffset);
            for(uint32_t q=0;q<geoReflexive.count;q++) {
                mod2_geo_part *parts = tags[i].tagData + (geos[q].parts.pointer - tags[i].memoryOffset);
                for(uint32_t j=0;j<geos[q].parts.count;j++) {
                    /** Vertices */
                    void *newVertices = malloc(tags[i].verticesLength + parts[j].vertex.count * 0x44);
                    memcpy(newVertices,tags[i].vertices,tags[i].verticesLength);
                    memcpy(newVertices + tags[i].verticesLength,verticesOffset + parts[j].vertex.offset, parts[j].vertex.count * 0x44);
                    parts[j].vertex.offset = tags[i].verticesLength;
                    tags[i].verticesLength += 0x44 * parts[j].vertex.count;
                    free(tags[i].vertices);
                    tags[i].vertices = newVertices;
                    
                    /** Indices */
                    void *newIndices = malloc(tags[i].indicesLength + parts[j].index.count * 0x2 + 0x4);
                    memcpy(newIndices, tags[i].indices, tags[i].indicesLength);
                    memcpy(newIndices + tags[i].indicesLength, indicesOffset + parts[j].index.offsetA, parts[j].index.count * 0x2 + 0x4);
                    parts[j].index.offsetA = tags[i].indicesLength;
                    parts[j].index.offsetB = tags[i].indicesLength;
                    tags[i].indicesLength += 0x2 * parts[j].index.count + 0x4;
                    free(tags[i].indices);
                    tags[i].indices = newIndices;
                }
            }
            Reflexive shaderReflexive = *(Reflexive *)(tags[i].tagData + 0xDC);
            addReflexiveDependencyToTag(shaderReflexive, tags + i, 0x20, 0x0);
        }
        else if(memcmp(tags[i].classA,SBSP,4) == 0) {
            addDependencyToTag(0x18, tags + i);
            addReflexiveDependencyToTag(*(Reflexive *)(tags[i].tagData + 0xBC), tags + i, 20, 0x0);
            
            Reflexive *lightmaps = tags[i].tagData + 0x11C;
            void *lightmapsOffset = tags[i].tagData + (lightmaps->pointer - tags[i].memoryOffset);
            for(uint32_t q=0;q<lightmaps->count;q++) {
                addReflexiveDependencyToTag(*(Reflexive *)(lightmapsOffset + 32 * q + 0x14), tags + i, 256, 0x0);
            }
            addReflexiveDependencyToTag(*(Reflexive *)(tags[i].tagData + 0x134), tags + i, 16, 0x0);
            
            Reflexive *clusters = tags[i].tagData + 0x14C;
            void *clustersOffset = tags[i].tagData + (clusters->pointer - tags[i].memoryOffset);
            
            for(uint32_t q=0;q<clusters->count;q++) {
                addReflexiveDependencyToTag(*(Reflexive *)(clustersOffset + 104 * q + 0x50), tags + i, 64, 0x24);
                Reflexive *lightmapsShaderPalette = clustersOffset + 104 * q + 0x28;
                uint32_t lightmapsShaderPaletteOffset = lightmapsShaderPalette->pointer - tags[i].memoryOffset;
                for(uint32_t j=0;j<lightmapsShaderPalette->count;j++) {
                    addTagIDDependencyToTag(lightmapsShaderPaletteOffset + 0x4 + j * 8, tags + i);
                }
            }
            
            addReflexiveDependencyToTag(*(Reflexive *)(tags[i].tagData + 0x148), tags + i, 136, 0x20);
            
            Reflexive *weatherPalette = tags[i].tagData + 0x1CC;
            uint32_t weatherPaletteOffset = weatherPalette->pointer - tags[i].memoryOffset;
            for(uint32_t j=0;j<weatherPalette->count;j++) {
                uint32_t location = weatherPaletteOffset + j * 240;
                addDependencyToTag(location + 0x20, tags + i);
                addDependencyToTag(location + 0x80, tags + i);
            }
            
            addReflexiveDependencyToTag(*(Reflexive *)(tags[i].tagData + 0x214), tags + i, 116, 0x20);
            addReflexiveDependencyToTag(*(Reflexive *)(tags[i].tagData + 0x220), tags + i, 80, 0x20);
        }
        else {
            uint32_t iteration = 1;
            if(sizeof(Dependency) > tags[i].tagDataSize) continue;
            for(uint32_t j=0;(j + sizeof(Dependency)) < (tags[i].tagDataSize); j+=iteration) {
                Dependency *dependency = tags[i].tagData + j;
                if(dependency->tagId.tagTableIndex < mapIndex->tagCount &&
                   dependency->zero == 0 &&
                   dependency->tagId.tableIndex == mapTags[dependency->tagId.tagTableIndex].identity.tableIndex &&
                   memcmp(dependency->mainClass,mapTags[dependency->tagId.tagTableIndex].classA,4) == 0) {
                    addDependencyToTag(j, tags + i);
                }
            }
        }
        if(memcmp(tags[i].classA, OBJE, 4) == 0 || memcmp(tags[i].classB, OBJE, 4) == 0 || memcmp(tags[i].classC, OBJE, 4) == 0) {
            Reflexive *predictedResources = (Reflexive *)(tags[i].tagData + 0x170);
            uint32_t resourcesOffset = predictedResources->pointer - tags[i].memoryOffset;
            for(uint32_t j=0;j<predictedResources->count;j++) {
                addTagIDDependencyToTag(resourcesOffset + 0x4 + 0x8 * j, tags + i);
            }
        }
    }
    return tagArray;
}