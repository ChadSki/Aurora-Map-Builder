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



// General map structure header file.

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Aurora_ZZTMap_h
#define Aurora_ZZTMap_h

#define HALO_META_MEMORY_OFFSET 0x40440000
#define TAG_INDEX_ZERO (uint16_t)0xE174

typedef struct TagID {
    uint16_t tagTableIndex;
    uint16_t tableIndex;
} TagID;

typedef struct HaloMapHeader {
	char integrityHead[4];
	uint32_t version;
	uint32_t length;
	uint32_t zero;
	uint32_t indexOffset;
	uint32_t metaSize;
	char padding[0x8];
	char name[0x20];
	char builddate[0x20];
	uint16_t type;
    uint16_t padding2;
    uint32_t crc32;
    char zeroes2[0x794];
    char integrityFoot[4];
} HaloMapHeader;

typedef struct HaloMapIndex {
	uint32_t tagIndexOffset;
	TagID scenarioTag;
	uint32_t mapId;
	uint32_t tagCount;
	uint32_t partCountA;
	uint32_t vertexOffset;
	uint32_t partCountB;
	uint32_t vertexSize;
	uint32_t modelSize;
	char tagsString[4];
} HaloMapIndex;

typedef struct HaloMapTag {
	char classA[4];
	char classB[4];
	char classC[4];
	TagID identity;
	uint32_t nameOffset;
	uint32_t dataOffset;
    uint32_t notInsideMap;
	char padding[0x4];
} HaloMapTag;

typedef struct Dependency {
    char mainClass[4];
    uint32_t nameOffset;
    uint32_t zero;
    TagID tagId;
} Dependency;

typedef struct Reflexive {
    uint32_t count;
    uint32_t pointer;
    uint32_t padding;
} Reflexive;

typedef struct PredictedResource {
    uint32_t type;
    TagID tag;
} PredictedResource;



#endif
    
#ifdef __cplusplus
}
#endif
