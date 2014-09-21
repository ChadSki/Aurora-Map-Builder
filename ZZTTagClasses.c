//  ZZTTagClasses.c

/*
 
 Copyright (c) 2014, Paul Whitcomb
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of Paul Whitcomb nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */

#include <stdio.h>
#include "ZZTTagClasses.h"

const char *translateHaloClassToName(uint32_t className) { //from Halo Editing Kit
    if(className == *(uint32_t *)&ACTR) return "actor";
    else if(className == *(uint32_t *)&ACTV) return "actor variant";
    else if(className == *(uint32_t *)&ANT) return "antenna";
    else if(className == *(uint32_t *)&ANTR) return "model animations";
    else if(className == *(uint32_t *)&BIPD) return "biped";
    else if(className == *(uint32_t *)&BITM) return "bitmap";
    else if(className == *(uint32_t *)&BOOM) return "spheroid";
    else if(className == *(uint32_t *)&CDMG) return "continuous damage effect";
    else if(className == *(uint32_t *)&COLL) return "model collision geometry";
    else if(className == *(uint32_t *)&COLO) return "color table";
    else if(className == *(uint32_t *)&CONT) return "contrail";
    else if(className == *(uint32_t *)&CTRL) return "device control";
    else if(className == *(uint32_t *)&DECA) return "decal";
    else if(className == *(uint32_t *)&DELA) return "ui widget definition";
    else if(className == *(uint32_t *)&DEVC) return "input device defaults";
    else if(className == *(uint32_t *)&DEVI) return "device";
    else if(className == *(uint32_t *)&DOBC) return "detail object collection";
    else if(className == *(uint32_t *)&EFFE) return "effect";
    else if(className == *(uint32_t *)&ELEC) return "electricity";
    else if(className == *(uint32_t *)&EQIP) return "equipment";
    else if(className == *(uint32_t *)&FLAG) return "flag";
    else if(className == *(uint32_t *)&FOG ) return "fog";
    else if(className == *(uint32_t *)&FONT) return "font";
    else if(className == *(uint32_t *)&FOOT) return "material effects";
    else if(className == *(uint32_t *)&GARB) return "garbage";
    else if(className == *(uint32_t *)&GLW) return "glow";
    else if(className == *(uint32_t *)&GRHI) return "grenade hud interface";
    else if(className == *(uint32_t *)&HMT ) return "hud message text";
    else if(className == *(uint32_t *)&HUD) return "hud number";
    else if(className == *(uint32_t *)&HUDG) return "hud globals";
    else if(className == *(uint32_t *)&ITEM) return "item";
    else if(className == *(uint32_t *)&ITMC) return "item collection";
    else if(className == *(uint32_t *)&JPT) return "damage effect";
    else if(className == *(uint32_t *)&LENS) return "lens flare";
    else if(className == *(uint32_t *)&LIFI) return "device light fixture";
    else if(className == *(uint32_t *)&LIGH) return "light";
    else if(className == *(uint32_t *)&LSND) return "sound looping";
    else if(className == *(uint32_t *)&MACH) return "device machinery";
    else if(className == *(uint32_t *)&MATG) return "game globals";
    else if(className == *(uint32_t *)&METR) return "meter";
    else if(className == *(uint32_t *)&MGS2) return "light volume";
    else if(className == *(uint32_t *)&MOD2) return "gearbox model";
    else if(className == *(uint32_t *)&MODE) return "model";
    else if(className == *(uint32_t *)&MPLY) return "multiplayer scenario description";
    else if(className == *(uint32_t *)&NGPR) return "network game preferences";
    else if(className == *(uint32_t *)&OBJE) return "object";
    else if(className == *(uint32_t *)&PART) return "particle";
    else if(className == *(uint32_t *)&PCTL) return "particle system";
    else if(className == *(uint32_t *)&PHYS) return "physics";
    else if(className == *(uint32_t *)&PLAC) return "placeholder";
    else if(className == *(uint32_t *)&PPHY) return "point physics";
    else if(className == *(uint32_t *)&PROJ) return "projectile";
    else if(className == *(uint32_t *)&RAIN) return "weather";
    else if(className == *(uint32_t *)&SBSP) return "scenario structure binary space partition";
    else if(className == *(uint32_t *)&SCEN) return "scenery";
    else if(className == *(uint32_t *)&SCEX) return "shader transparent chicago extended";
    else if(className == *(uint32_t *)&SCHI) return "shader transparent chicago";
    else if(className == *(uint32_t *)&SCNR) return "scenario";
    else if(className == *(uint32_t *)&SENV) return "shader environment";
    else if(className == *(uint32_t *)&SGLA) return "shader transparent glass";
    else if(className == *(uint32_t *)&SHDR) return "shader";
    else if(className == *(uint32_t *)&SKY ) return "sky";
    else if(className == *(uint32_t *)&SMET) return "shader transparent meter";
    else if(className == *(uint32_t *)&SND) return "sound";
    else if(className == *(uint32_t *)&SNDE) return "sound environment";
    else if(className == *(uint32_t *)&SOSO) return "shader model";
    else if(className == *(uint32_t *)&SOTR) return "shader transparent generic";
    else if(className == *(uint32_t *)&SOUL) return "ui widget collection";
    else if(className == *(uint32_t *)&SPLA) return "shader transparent plasma";
    else if(className == *(uint32_t *)&SSCE) return "sound scenery";
    else if(className == *(uint32_t *)&STR) return "string list";
    else if(className == *(uint32_t *)&SWAT) return "shader transparent water";
    else if(className == *(uint32_t *)&TAGC) return "tag collection";
    else if(className == *(uint32_t *)&TRAK) return "camera track";
    else if(className == *(uint32_t *)&UDLG) return "unit dialogue";
    else if(className == *(uint32_t *)&UNHI) return "unit hud interface";
    else if(className == *(uint32_t *)&UNIT) return "unit";
    else if(className == *(uint32_t *)&USTR) return "unicode string list";
    else if(className == *(uint32_t *)&VCKY) return "virtual keyboard";
    else if(className == *(uint32_t *)&VEHI) return "vehicle";
    else if(className == *(uint32_t *)&WEAP) return "weapon";
    else if(className == *(uint32_t *)&WIND) return "wind";
    else if(className == *(uint32_t *)&WPHI) return "weapon hud interface";
    else return "unknown";
}