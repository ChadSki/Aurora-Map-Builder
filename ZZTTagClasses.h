// ZZTTagClasses.h

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

#include <stdint.h>

#define ACTR "rtca"
#define ACTV "vtca"
#define ANT "!tna"
#define ANTR "rtna"
#define BIPD "dpib"
#define BITM "mtib"
#define BOOM "moob"
#define CDMG "gmdc"
#define COLL "lloc"
#define COLO "oloc"
#define CONT "tnoc"
#define CTRL "lrtc"
#define DECA "aced"
#define DELA "aLeD"
#define DEVC "cved"
#define DEVI "ived"
#define DOBC "cbod"
#define EFFE "effe"
#define ELEC "cele"
#define EQIP "piqe"
#define FLAG "galf"
#define FOG  " gof"
#define FONT "tnof"
#define FOOT "toof"
#define GARB "brag"
#define GLW "!wlg"
#define GRHI "ihrg"
#define HMT  " tmh"
#define HUD "#duh"
#define HUDG "gduh"
#define ITEM "meti"
#define ITMC "cmti"
#define JPT "!tpj"
#define LENS "snel"
#define LIFI "ifil"
#define LIGH "hgil"
#define LSND "dnsl"
#define MACH "hcam"
#define MATG "gtam"
#define METR "rtem"
#define MGS2 "2sgm"
#define MOD2 "2dom"
#define MODE "edom"
#define MPLY "ylpm"
#define NGPR "rpgn"
#define OBJE "ejbo"
#define PART "trap"
#define PCTL "ltcp"
#define PHYS "syhp"
#define PLAC "calp"
#define PPHY "yhpp"
#define PROJ "jorp"
#define RAIN "niar"
#define SBSP "psbs"
#define SCEN "necs"
#define SCEX "xecs"
#define SCHI "ihcs"
#define SCNR "rncs"
#define SENV "vnes"
#define SGLA "algs"
#define SHDR "rdhs"
#define SKY  " yks"
#define SMET "tems"
#define SND "!dns"
#define SNDE "edns"
#define SOSO "osos"
#define SOTR "rtos"
#define SOUL "luoS"
#define SPLA "alps"
#define SSCE "ecss"
#define STR "#rts"
#define SWAT "taws"
#define TAGC "cgat"
#define TRAK "kart"
#define UDLG "gldu"
#define UNHI "ihnu"
#define UNIT "tinu"
#define USTR "rtsu"
#define VCKY "ykcv"
#define VEHI "ihev"
#define WEAP "paew"
#define WIND "dniw"
#define WPHI "ihpw"

#ifndef deathstar_ZZTTagClasses_h
#define deathstar_ZZTTagClasses_h
const char *translateHaloClassToName(uint32_t className);
#endif