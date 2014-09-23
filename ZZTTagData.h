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

#include "ZZTMap.h"
#include "ZZTTagClasses.h"

#ifndef Aurora_ZZTTagData_h
#define Aurora_ZZTTagData_h

typedef struct {
    uint32_t bsp_file_offset;
    uint32_t bsp_tag_size;
    uint32_t bsp_memory_offset;
    uint32_t zero;
    Dependency bsp_dependency;
} scnr_tag_bsps;

typedef struct scnr_tag_structs {
    Dependency dontUse;             //0x0
    Dependency wontUse;             //0x10
    Dependency cantUse;             //0x20
    Reflexive skies;                //0x30  D16
    char padding1[0x4];             //0x3C
    Reflexive child_scenarios;      //0x40  D32
    char padding2[0xA0];            //0x4C
    Reflexive predicted_resources;  //0xEC  T8
    Reflexive functions;            //0xF8
    char padding3[0x8];             //0x104
    uint32_t something_unknown;     //0x10C
    uint32_t unknown_pointer;       //0x110
    uint32_t unknown_again;         //0x114
    Reflexive comments;             //0x118 S
    char padding4[0xE0];            //0x124
    Reflexive objectNames;          //0x204
    Reflexive scenery;              //0x210
    Reflexive sceneryPalette;       //0x21C D48
    Reflexive bipeds;               //0x228
    Reflexive bipedPalette;         //0x234 D48
    Reflexive vehicle;              //0x240
    Reflexive vehiclePalette;       //0x24C D48
    Reflexive equipment;            //0x258
    Reflexive equipmentPalette;     //0x264 D48
    Reflexive weapons;              //0x270
    Reflexive weaponPalette;        //0x27C D48
    Reflexive deviceGroups;         //0x288
    Reflexive machines;             //0x294
    Reflexive machinePalette;       //0x2A0 D48
    Reflexive controls;             //0x2AC
    Reflexive controlPalette;       //0x2B8 D48
    Reflexive lightFixtures;        //0x2C4
    Reflexive lightFixturesPalette; //0x2D0 D48
    Reflexive soundScenery;         //0x2DC
    Reflexive soundSceneryPalette;  //0x2E8 D48
    char padding5[0x54];            //0x2F4
    Reflexive startingProfile;      //0x348 DX
    Reflexive startingLocations;    //0x354
    Reflexive triggerVolumes;       //0x360
    Reflexive recordedAnimations;   //0x36C
    Reflexive netgameFlags;         //0x378 D148+0x14
    Reflexive netgameEquipment;     //0x384 D144+0x50
    Reflexive startingEquipment;    //0x390 DX
    Reflexive bspTriggerVolumes;    //0x39C
    Reflexive decals;               //0x3A8
    Reflexive decalPalette;         //0x3B4 D16
    Reflexive dobcPalette;          //0x3C0 D48
    char padding6[0x54];            //0x3CC
    Reflexive actorPalette;         //0x420 D16
    Reflexive encounters;           //0x42C SS
    Reflexive commandLists;         //0x438 S
    Reflexive aiAnimationReferences;//0x444 D60+0x20
    Reflexive aiScriptReferences;   //0x450
    Reflexive aiRecordingReferences;//0x45C
    Reflexive aiConversations;      //0x468 SSDX
    uint32_t script_syntax_data;    //0x474
    uint32_t unknown_script_stuff;  //0x478
    uint32_t unknown_script_stuff2; //0x47C
    uint32_t script_pointer;        //0x480
    uint32_t unknown_script_stuff3; //0x484
    uint32_t script_string_data;    //0x488
    uint32_t unknown_script_stuff4; //0x48C
    uint32_t unknown_script_stuff5; //0x490
    uint32_t script_pointer2;       //0x494
    uint32_t unknown_script_stuff6; //0x498
    Reflexive scripts;              //0x49C
    Reflexive globals;              //0x4A8
    Reflexive references;           //0x4B4 D40+0x18
    char padding9[0x24];            //0x4C0
    Reflexive cutsceneFlags;        //0x4E4
    Reflexive cutsceneCameraPoints; //0x4F0
    Reflexive cutsceneTitles;       //0x4FC
    char padding10[0x6C];           //0x508
    Dependency customObjectNames;   //0x574
    Dependency ingameHelpText;      //0x584
    Dependency hudMessage;          //0x594
    Reflexive bsps;                 //0x5A4 D32+0x10
} scnr_tag_structs;

typedef struct bitm_tag_bitmap {
    uint32_t signature; //0x0
    uint16_t width; //0x4
    uint16_t height; //0x6
    uint16_t depth; //0x8
    uint16_t type; //0xA
    uint16_t format; //0xC
    uint16_t flags; //0xE ((>> 8) & 1) == 0 means internalized
    uint16_t x; //0x10
    uint16_t y; //0x12
    uint16_t mipmapCount; //0x14
    uint16_t padding; //0x16
    uint32_t pixelOffset; //0x18
    uint32_t pixelCount; //0x1C
    uint32_t uk8; //0x20
    uint32_t uk9; //0x24
    uint32_t uk10; //0x28
    TagID bitmap; //0x2C
} bitm_tag_bitmap; //reflexive is offset 0x60 in bitm tags

typedef struct snd_range {
    char padding[0x3C];
    Reflexive permutations;
} snd_range; //reflexive is offset 0x98 in snd! tags

typedef struct snd_range_permutation {
    char name[0x20]; //0x0
    float skipFraction; //0x20
    float gain; //0x24
    uint16_t compression; //0x28
    uint16_t nextPermutation; //0x2A
    char padding[0x8]; //0x2C
    TagID tagId1; //0x34
    char notsure[0x4]; //0x38
    TagID tagId2; //0x3C
    uint32_t samples; //0x40
    uint32_t flags; //0x44 (& 1) == 0 means internalized
    uint32_t offset; //0x48
    char padding1[0x8]; //0x44
    uint32_t mouthdata; //0x54
    char padding2[0x10]; //0x58
    uint32_t subtitleData; //0x68
    char padding3[0x10];
} snd_range_permutation;


typedef struct mod2_geo_index_offset {
    uint32_t count; //size in bytes = 0x4 + 0x2 x count
    uint32_t offsetA;
    uint32_t offsetB;
} mod2_geo_index_offset;

typedef struct mod2_geo_vertex_offset {
    uint32_t count; //size in bytes = 0x44 x count
    char padding[0x8];
    uint32_t offset;
} mod2_geo_vertex_offset;

typedef struct mod2_geo {
    char padding[0x24];
    Reflexive parts;
} mod2_geo; //reflexive is offset 0xD0 in mod2 tag

typedef struct mod2_geo_part {
    char padding[0x48];
    mod2_geo_index_offset index;
    char padding1[0x4];
    mod2_geo_vertex_offset vertex;
    char padding2[0x1C];
} mod2_geo_part;

#endif
    
    
#ifdef __cplusplus
}
#endif
