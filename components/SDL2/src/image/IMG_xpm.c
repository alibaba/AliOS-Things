/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
 * XPM (X PixMap) image loader:
 *
 * Supports the XPMv3 format, EXCEPT:
 * - hotspot coordinates are ignored
 * - only colour ('c') colour symbols are used
 * - rgb.txt is not used (for portability), so only RGB colours
 *   are recognized (#rrggbb etc) - only a few basic colour names are
 *   handled
 *
 * The result is an 8bpp indexed surface if possible, otherwise 32bpp.
 * The colourkey is correctly set if transparency is used.
 *
 * Besides the standard API, also provides
 *
 *     SDL_Surface *IMG_ReadXPMFromArray(char **xpm)
 *
 * that reads the image data from an XPM file included in the C source.
 *
 * TODO: include rgb.txt here. The full table (from solaris 2.6) only
 * requires about 13K in binary form.
 */

#include "SDL_image.h"

#ifdef LOAD_XPM

/* See if an image is contained in a data source */
int IMG_isXPM(SDL_RWops *src)
{
    Sint64 start;
    int is_XPM;
    char magic[9];

    if ( !src )
        return 0;
    start = SDL_RWtell(src);
    is_XPM = 0;
    if ( SDL_RWread(src, magic, sizeof(magic), 1) ) {
        if ( SDL_memcmp(magic, "/* XPM */", sizeof(magic)) == 0 ) {
            is_XPM = 1;
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_XPM);
}

/* Hash table to look up colors from pixel strings */
#define STARTING_HASH_SIZE 256

struct hash_entry {
    char *key;
    Uint32 color;
    struct hash_entry *next;
};

struct color_hash {
    struct hash_entry **table;
    struct hash_entry *entries; /* array of all entries */
    struct hash_entry *next_free;
    int size;
    int maxnum;
};

static int hash_key(const char *key, int cpp, int size)
{
    int hash;

    hash = 0;
    while ( cpp-- > 0 ) {
        hash = hash * 33 + *key++;
    }
    return hash & (size - 1);
}

static struct color_hash *create_colorhash(int maxnum)
{
    int bytes, s;
    struct color_hash *hash;

    /* we know how many entries we need, so we can allocate
       everything here */
    hash = (struct color_hash *)SDL_calloc(1, sizeof(*hash));
    if (!hash)
        return NULL;

    /* use power-of-2 sized hash table for decoding speed */
    for (s = STARTING_HASH_SIZE; s < maxnum; s <<= 1)
        ;
    hash->size = s;
    hash->maxnum = maxnum;

    bytes = hash->size * sizeof(struct hash_entry **);
    /* Check for overflow */
    if ((bytes / sizeof(struct hash_entry **)) != hash->size) {
        IMG_SetError("memory allocation overflow");
        SDL_free(hash);
        return NULL;
    }
    hash->table = (struct hash_entry **)SDL_calloc(1, bytes);
    if (!hash->table) {
        SDL_free(hash);
        return NULL;
    }

    bytes = maxnum * sizeof(struct hash_entry);
    /* Check for overflow */
    if ((bytes / sizeof(struct hash_entry)) != maxnum) {
        IMG_SetError("memory allocation overflow");
        SDL_free(hash->table);
        SDL_free(hash);
        return NULL;
    }
    hash->entries = (struct hash_entry *)SDL_calloc(1, bytes);
    if (!hash->entries) {
        SDL_free(hash->table);
        SDL_free(hash);
        return NULL;
    }
    hash->next_free = hash->entries;
    return hash;
}

static int add_colorhash(struct color_hash *hash,
                         char *key, int cpp, Uint32 color)
{
    int index = hash_key(key, cpp, hash->size);
    struct hash_entry *e = hash->next_free++;
    e->color = color;
    e->key = key;
    e->next = hash->table[index];
    hash->table[index] = e;
    return 1;
}

/* fast lookup that works if cpp == 1 */
#define QUICK_COLORHASH(hash, key) ((hash)->table[*(Uint8 *)(key)]->color)

static Uint32 get_colorhash(struct color_hash *hash, const char *key, int cpp)
{
    struct hash_entry *entry = hash->table[hash_key(key, cpp, hash->size)];
    while (entry) {
        if (SDL_memcmp(key, entry->key, cpp) == 0)
            return entry->color;
        entry = entry->next;
    }
    return 0;       /* garbage in - garbage out */
}

static void free_colorhash(struct color_hash *hash)
{
    if (hash) {
        if (hash->table)
            SDL_free(hash->table);
        if (hash->entries)
            SDL_free(hash->entries);
        SDL_free(hash);
    }
}

/*
 * convert colour spec to RGB (in 0xrrggbb format).
 * return 1 if successful.
 */
static int color_to_rgb(char *spec, int speclen, Uint32 *rgb)
{
    /* poor man's rgb.txt */
    static struct { char *name; Uint32 rgb; } known[] = {
        { "none",                 0xFFFFFFFF },
        { "black",                0x000000 },
        { "white",                0xFFFFFF },
        { "red",                  0xFF0000 },
        { "green",                0x00FF00 },
        { "blue",                 0x0000FF },
/* This table increases the size of the library by 40K, so it's disabled by default */
#ifdef EXTENDED_XPM_COLORS
        { "aliceblue",            0xf0f8ff },
        { "antiquewhite",         0xfaebd7 },
        { "antiquewhite1",        0xffefdb },
        { "antiquewhite2",        0xeedfcc },
        { "antiquewhite3",        0xcdc0b0 },
        { "antiquewhite4",        0x8b8378 },
        { "aqua",                 0x00ffff },
        { "aquamarine",           0x7fffd4 },
        { "aquamarine1",          0x7fffd4 },
        { "aquamarine2",          0x76eec6 },
        { "aquamarine3",          0x66cdaa },
        { "aquamarine4",          0x458b74 },
        { "azure",                0xf0ffff },
        { "azure1",               0xf0ffff },
        { "azure2",               0xe0eeee },
        { "azure3",               0xc1cdcd },
        { "azure4",               0x838b8b },
        { "beige",                0xf5f5dc },
        { "bisque",               0xffe4c4 },
        { "bisque1",              0xffe4c4 },
        { "bisque2",              0xeed5b7 },
        { "bisque3",              0xcdb79e },
        { "bisque4",              0x8b7d6b },
        { "black",                0x000000 },
        { "blanchedalmond",       0xffebcd },
        { "blue",                 0x0000ff },
        { "blue1",                0x0000ff },
        { "blue2",                0x0000ee },
        { "blue3",                0x0000cd },
        { "blue4",                0x00008B },
        { "blueviolet",           0x8a2be2 },
        { "brown",                0xA52A2A },
        { "brown1",               0xFF4040 },
        { "brown2",               0xEE3B3B },
        { "brown3",               0xCD3333 },
        { "brown4",               0x8B2323 },
        { "burlywood",            0xDEB887 },
        { "burlywood1",           0xFFD39B },
        { "burlywood2",           0xEEC591 },
        { "burlywood3",           0xCDAA7D },
        { "burlywood4",           0x8B7355 },
        { "cadetblue",            0x5F9EA0 },
        { "cadetblue",            0x5f9ea0 },
        { "cadetblue1",           0x98f5ff },
        { "cadetblue2",           0x8ee5ee },
        { "cadetblue3",           0x7ac5cd },
        { "cadetblue4",           0x53868b },
        { "chartreuse",           0x7FFF00 },
        { "chartreuse1",          0x7FFF00 },
        { "chartreuse2",          0x76EE00 },
        { "chartreuse3",          0x66CD00 },
        { "chartreuse4",          0x458B00 },
        { "chocolate",            0xD2691E },
        { "chocolate1",           0xFF7F24 },
        { "chocolate2",           0xEE7621 },
        { "chocolate3",           0xCD661D },
        { "chocolate4",           0x8B4513 },
        { "coral",                0xFF7F50 },
        { "coral1",               0xFF7256 },
        { "coral2",               0xEE6A50 },
        { "coral3",               0xCD5B45 },
        { "coral4",               0x8B3E2F },
        { "cornflowerblue",       0x6495ed },
        { "cornsilk",             0xFFF8DC },
        { "cornsilk1",            0xFFF8DC },
        { "cornsilk2",            0xEEE8CD },
        { "cornsilk3",            0xCDC8B1 },
        { "cornsilk4",            0x8B8878 },
        { "crimson",              0xDC143C },
        { "cyan",                 0x00FFFF },
        { "cyan1",                0x00FFFF },
        { "cyan2",                0x00EEEE },
        { "cyan3",                0x00CDCD },
        { "cyan4",                0x008B8B },
        { "darkblue",             0x00008b },
        { "darkcyan",             0x008b8b },
        { "darkgoldenrod",        0xb8860b },
        { "darkgoldenrod1",       0xffb90f },
        { "darkgoldenrod2",       0xeead0e },
        { "darkgoldenrod3",       0xcd950c },
        { "darkgoldenrod4",       0x8b6508 },
        { "darkgray",             0xa9a9a9 },
        { "darkgreen",            0x006400 },
        { "darkgrey",             0xa9a9a9 },
        { "darkkhaki",            0xbdb76b },
        { "darkmagenta",          0x8b008b },
        { "darkolivegreen",       0x556b2f },
        { "darkolivegreen1",      0xcaff70 },
        { "darkolivegreen2",      0xbcee68 },
        { "darkolivegreen3",      0xa2cd5a },
        { "darkolivegreen4",      0x6e8b3d },
        { "darkorange",           0xff8c00 },
        { "darkorange1",          0xff7f00 },
        { "darkorange2",          0xee7600 },
        { "darkorange3",          0xcd6600 },
        { "darkorange4",          0x8b4500 },
        { "darkorchid",           0x9932cc },
        { "darkorchid1",          0xbf3eff },
        { "darkorchid2",          0xb23aee },
        { "darkorchid3",          0x9a32cd },
        { "darkorchid4",          0x68228b },
        { "darkred",              0x8b0000 },
        { "darksalmon",           0xe9967a },
        { "darkseagreen",         0x8fbc8f },
        { "darkseagreen1",        0xc1ffc1 },
        { "darkseagreen2",        0xb4eeb4 },
        { "darkseagreen3",        0x9bcd9b },
        { "darkseagreen4",        0x698b69 },
        { "darkslateblue",        0x483d8b },
        { "darkslategray",        0x2f4f4f },
        { "darkslategray1",       0x97ffff },
        { "darkslategray2",       0x8deeee },
        { "darkslategray3",       0x79cdcd },
        { "darkslategray4",       0x528b8b },
        { "darkslategrey",        0x2f4f4f },
        { "darkturquoise",        0x00ced1 },
        { "darkviolet",           0x9400D3 },
        { "darkviolet",           0x9400d3 },
        { "deeppink",             0xff1493 },
        { "deeppink1",            0xff1493 },
        { "deeppink2",            0xee1289 },
        { "deeppink3",            0xcd1076 },
        { "deeppink4",            0x8b0a50 },
        { "deepskyblue",          0x00bfff },
        { "deepskyblue1",         0x00bfff },
        { "deepskyblue2",         0x00b2ee },
        { "deepskyblue3",         0x009acd },
        { "deepskyblue4",         0x00688b },
        { "dimgray",              0x696969 },
        { "dimgrey",              0x696969 },
        { "dodgerblue",           0x1e90ff },
        { "dodgerblue1",          0x1e90ff },
        { "dodgerblue2",          0x1c86ee },
        { "dodgerblue3",          0x1874cd },
        { "dodgerblue4",          0x104e8b },
        { "firebrick",            0xB22222 },
        { "firebrick1",           0xFF3030 },
        { "firebrick2",           0xEE2C2C },
        { "firebrick3",           0xCD2626 },
        { "firebrick4",           0x8B1A1A },
        { "floralwhite",          0xfffaf0 },
        { "forestgreen",          0x228b22 },
        { "fractal",              0x808080 },
        { "fuchsia",              0xFF00FF },
        { "gainsboro",            0xDCDCDC },
        { "ghostwhite",           0xf8f8ff },
        { "gold",                 0xFFD700 },
        { "gold1",                0xFFD700 },
        { "gold2",                0xEEC900 },
        { "gold3",                0xCDAD00 },
        { "gold4",                0x8B7500 },
        { "goldenrod",            0xDAA520 },
        { "goldenrod1",           0xFFC125 },
        { "goldenrod2",           0xEEB422 },
        { "goldenrod3",           0xCD9B1D },
        { "goldenrod4",           0x8B6914 },
        { "gray",                 0x7E7E7E },
        { "gray",                 0xBEBEBE },
        { "gray0",                0x000000 },
        { "gray1",                0x030303 },
        { "gray10",               0x1A1A1A },
        { "gray100",              0xFFFFFF },
        { "gray11",               0x1C1C1C },
        { "gray12",               0x1F1F1F },
        { "gray13",               0x212121 },
        { "gray14",               0x242424 },
        { "gray15",               0x262626 },
        { "gray16",               0x292929 },
        { "gray17",               0x2B2B2B },
        { "gray18",               0x2E2E2E },
        { "gray19",               0x303030 },
        { "gray2",                0x050505 },
        { "gray20",               0x333333 },
        { "gray21",               0x363636 },
        { "gray22",               0x383838 },
        { "gray23",               0x3B3B3B },
        { "gray24",               0x3D3D3D },
        { "gray25",               0x404040 },
        { "gray26",               0x424242 },
        { "gray27",               0x454545 },
        { "gray28",               0x474747 },
        { "gray29",               0x4A4A4A },
        { "gray3",                0x080808 },
        { "gray30",               0x4D4D4D },
        { "gray31",               0x4F4F4F },
        { "gray32",               0x525252 },
        { "gray33",               0x545454 },
        { "gray34",               0x575757 },
        { "gray35",               0x595959 },
        { "gray36",               0x5C5C5C },
        { "gray37",               0x5E5E5E },
        { "gray38",               0x616161 },
        { "gray39",               0x636363 },
        { "gray4",                0x0A0A0A },
        { "gray40",               0x666666 },
        { "gray41",               0x696969 },
        { "gray42",               0x6B6B6B },
        { "gray43",               0x6E6E6E },
        { "gray44",               0x707070 },
        { "gray45",               0x737373 },
        { "gray46",               0x757575 },
        { "gray47",               0x787878 },
        { "gray48",               0x7A7A7A },
        { "gray49",               0x7D7D7D },
        { "gray5",                0x0D0D0D },
        { "gray50",               0x7F7F7F },
        { "gray51",               0x828282 },
        { "gray52",               0x858585 },
        { "gray53",               0x878787 },
        { "gray54",               0x8A8A8A },
        { "gray55",               0x8C8C8C },
        { "gray56",               0x8F8F8F },
        { "gray57",               0x919191 },
        { "gray58",               0x949494 },
        { "gray59",               0x969696 },
        { "gray6",                0x0F0F0F },
        { "gray60",               0x999999 },
        { "gray61",               0x9C9C9C },
        { "gray62",               0x9E9E9E },
        { "gray63",               0xA1A1A1 },
        { "gray64",               0xA3A3A3 },
        { "gray65",               0xA6A6A6 },
        { "gray66",               0xA8A8A8 },
        { "gray67",               0xABABAB },
        { "gray68",               0xADADAD },
        { "gray69",               0xB0B0B0 },
        { "gray7",                0x121212 },
        { "gray70",               0xB3B3B3 },
        { "gray71",               0xB5B5B5 },
        { "gray72",               0xB8B8B8 },
        { "gray73",               0xBABABA },
        { "gray74",               0xBDBDBD },
        { "gray75",               0xBFBFBF },
        { "gray76",               0xC2C2C2 },
        { "gray77",               0xC4C4C4 },
        { "gray78",               0xC7C7C7 },
        { "gray79",               0xC9C9C9 },
        { "gray8",                0x141414 },
        { "gray80",               0xCCCCCC },
        { "gray81",               0xCFCFCF },
        { "gray82",               0xD1D1D1 },
        { "gray83",               0xD4D4D4 },
        { "gray84",               0xD6D6D6 },
        { "gray85",               0xD9D9D9 },
        { "gray86",               0xDBDBDB },
        { "gray87",               0xDEDEDE },
        { "gray88",               0xE0E0E0 },
        { "gray89",               0xE3E3E3 },
        { "gray9",                0x171717 },
        { "gray90",               0xE5E5E5 },
        { "gray91",               0xE8E8E8 },
        { "gray92",               0xEBEBEB },
        { "gray93",               0xEDEDED },
        { "gray94",               0xF0F0F0 },
        { "gray95",               0xF2F2F2 },
        { "gray96",               0xF5F5F5 },
        { "gray97",               0xF7F7F7 },
        { "gray98",               0xFAFAFA },
        { "gray99",               0xFCFCFC },
        { "green",                0x008000 },
        { "green",                0x00FF00 },
        { "green1",               0x00FF00 },
        { "green2",               0x00EE00 },
        { "green3",               0x00CD00 },
        { "green4",               0x008B00 },
        { "greenyellow",          0xadff2f },
        { "grey",                 0xBEBEBE },
        { "grey0",                0x000000 },
        { "grey1",                0x030303 },
        { "grey10",               0x1A1A1A },
        { "grey100",              0xFFFFFF },
        { "grey11",               0x1C1C1C },
        { "grey12",               0x1F1F1F },
        { "grey13",               0x212121 },
        { "grey14",               0x242424 },
        { "grey15",               0x262626 },
        { "grey16",               0x292929 },
        { "grey17",               0x2B2B2B },
        { "grey18",               0x2E2E2E },
        { "grey19",               0x303030 },
        { "grey2",                0x050505 },
        { "grey20",               0x333333 },
        { "grey21",               0x363636 },
        { "grey22",               0x383838 },
        { "grey23",               0x3B3B3B },
        { "grey24",               0x3D3D3D },
        { "grey25",               0x404040 },
        { "grey26",               0x424242 },
        { "grey27",               0x454545 },
        { "grey28",               0x474747 },
        { "grey29",               0x4A4A4A },
        { "grey3",                0x080808 },
        { "grey30",               0x4D4D4D },
        { "grey31",               0x4F4F4F },
        { "grey32",               0x525252 },
        { "grey33",               0x545454 },
        { "grey34",               0x575757 },
        { "grey35",               0x595959 },
        { "grey36",               0x5C5C5C },
        { "grey37",               0x5E5E5E },
        { "grey38",               0x616161 },
        { "grey39",               0x636363 },
        { "grey4",                0x0A0A0A },
        { "grey40",               0x666666 },
        { "grey41",               0x696969 },
        { "grey42",               0x6B6B6B },
        { "grey43",               0x6E6E6E },
        { "grey44",               0x707070 },
        { "grey45",               0x737373 },
        { "grey46",               0x757575 },
        { "grey47",               0x787878 },
        { "grey48",               0x7A7A7A },
        { "grey49",               0x7D7D7D },
        { "grey5",                0x0D0D0D },
        { "grey50",               0x7F7F7F },
        { "grey51",               0x828282 },
        { "grey52",               0x858585 },
        { "grey53",               0x878787 },
        { "grey54",               0x8A8A8A },
        { "grey55",               0x8C8C8C },
        { "grey56",               0x8F8F8F },
        { "grey57",               0x919191 },
        { "grey58",               0x949494 },
        { "grey59",               0x969696 },
        { "grey6",                0x0F0F0F },
        { "grey60",               0x999999 },
        { "grey61",               0x9C9C9C },
        { "grey62",               0x9E9E9E },
        { "grey63",               0xA1A1A1 },
        { "grey64",               0xA3A3A3 },
        { "grey65",               0xA6A6A6 },
        { "grey66",               0xA8A8A8 },
        { "grey67",               0xABABAB },
        { "grey68",               0xADADAD },
        { "grey69",               0xB0B0B0 },
        { "grey7",                0x121212 },
        { "grey70",               0xB3B3B3 },
        { "grey71",               0xB5B5B5 },
        { "grey72",               0xB8B8B8 },
        { "grey73",               0xBABABA },
        { "grey74",               0xBDBDBD },
        { "grey75",               0xBFBFBF },
        { "grey76",               0xC2C2C2 },
        { "grey77",               0xC4C4C4 },
        { "grey78",               0xC7C7C7 },
        { "grey79",               0xC9C9C9 },
        { "grey8",                0x141414 },
        { "grey80",               0xCCCCCC },
        { "grey81",               0xCFCFCF },
        { "grey82",               0xD1D1D1 },
        { "grey83",               0xD4D4D4 },
        { "grey84",               0xD6D6D6 },
        { "grey85",               0xD9D9D9 },
        { "grey86",               0xDBDBDB },
        { "grey87",               0xDEDEDE },
        { "grey88",               0xE0E0E0 },
        { "grey89",               0xE3E3E3 },
        { "grey9",                0x171717 },
        { "grey90",               0xE5E5E5 },
        { "grey91",               0xE8E8E8 },
        { "grey92",               0xEBEBEB },
        { "grey93",               0xEDEDED },
        { "grey94",               0xF0F0F0 },
        { "grey95",               0xF2F2F2 },
        { "grey96",               0xF5F5F5 },
        { "grey97",               0xF7F7F7 },
        { "grey98",               0xFAFAFA },
        { "grey99",               0xFCFCFC },
        { "honeydew",             0xF0FFF0 },
        { "honeydew1",            0xF0FFF0 },
        { "honeydew2",            0xE0EEE0 },
        { "honeydew3",            0xC1CDC1 },
        { "honeydew4",            0x838B83 },
        { "hotpink",              0xff69b4 },
        { "hotpink1",             0xff6eb4 },
        { "hotpink2",             0xee6aa7 },
        { "hotpink3",             0xcd6090 },
        { "hotpink4",             0x8b3a62 },
        { "indianred",            0xcd5c5c },
        { "indianred1",           0xff6a6a },
        { "indianred2",           0xee6363 },
        { "indianred3",           0xcd5555 },
        { "indianred4",           0x8b3a3a },
        { "indigo",               0x4B0082 },
        { "ivory",                0xFFFFF0 },
        { "ivory1",               0xFFFFF0 },
        { "ivory2",               0xEEEEE0 },
        { "ivory3",               0xCDCDC1 },
        { "ivory4",               0x8B8B83 },
        { "khaki",                0xF0E68C },
        { "khaki1",               0xFFF68F },
        { "khaki2",               0xEEE685 },
        { "khaki3",               0xCDC673 },
        { "khaki4",               0x8B864E },
        { "lavender",             0xE6E6FA },
        { "lavenderblush",        0xfff0f5 },
        { "lavenderblush1",       0xfff0f5 },
        { "lavenderblush2",       0xeee0e5 },
        { "lavenderblush3",       0xcdc1c5 },
        { "lavenderblush4",       0x8b8386 },
        { "lawngreen",            0x7cfc00 },
        { "lemonchiffon",         0xfffacd },
        { "lemonchiffon1",        0xfffacd },
        { "lemonchiffon2",        0xeee9bf },
        { "lemonchiffon3",        0xcdc9a5 },
        { "lemonchiffon4",        0x8b8970 },
        { "lightblue",            0xadd8e6 },
        { "lightblue1",           0xbfefff },
        { "lightblue2",           0xb2dfee },
        { "lightblue3",           0x9ac0cd },
        { "lightblue4",           0x68838b },
        { "lightcoral",           0xf08080 },
        { "lightcyan",            0xe0ffff },
        { "lightcyan1",           0xe0ffff },
        { "lightcyan2",           0xd1eeee },
        { "lightcyan3",           0xb4cdcd },
        { "lightcyan4",           0x7a8b8b },
        { "lightgoldenrod",       0xeedd82 },
        { "lightgoldenrod1",      0xffec8b },
        { "lightgoldenrod2",      0xeedc82 },
        { "lightgoldenrod3",      0xcdbe70 },
        { "lightgoldenrod4",      0x8b814c },
        { "lightgoldenrodyellow", 0xfafad2 },
        { "lightgray",            0xd3d3d3 },
        { "lightgreen",           0x90ee90 },
        { "lightgrey",            0xd3d3d3 },
        { "lightpink",            0xffb6c1 },
        { "lightpink1",           0xffaeb9 },
        { "lightpink2",           0xeea2ad },
        { "lightpink3",           0xcd8c95 },
        { "lightpink4",           0x8b5f65 },
        { "lightsalmon",          0xffa07a },
        { "lightsalmon1",         0xffa07a },
        { "lightsalmon2",         0xee9572 },
        { "lightsalmon3",         0xcd8162 },
        { "lightsalmon4",         0x8b5742 },
        { "lightseagreen",        0x20b2aa },
        { "lightskyblue",         0x87cefa },
        { "lightskyblue1",        0xb0e2ff },
        { "lightskyblue2",        0xa4d3ee },
        { "lightskyblue3",        0x8db6cd },
        { "lightskyblue4",        0x607b8b },
        { "lightslateblue",       0x8470ff },
        { "lightslategray",       0x778899 },
        { "lightslategrey",       0x778899 },
        { "lightsteelblue",       0xb0c4de },
        { "lightsteelblue1",      0xcae1ff },
        { "lightsteelblue2",      0xbcd2ee },
        { "lightsteelblue3",      0xa2b5cd },
        { "lightsteelblue4",      0x6e7b8b },
        { "lightyellow",          0xffffe0 },
        { "lightyellow1",         0xffffe0 },
        { "lightyellow2",         0xeeeed1 },
        { "lightyellow3",         0xcdcdb4 },
        { "lightyellow4",         0x8b8b7a },
        { "lime",                 0x00FF00 },
        { "limegreen",            0x32cd32 },
        { "linen",                0xFAF0E6 },
        { "magenta",              0xFF00FF },
        { "magenta1",             0xFF00FF },
        { "magenta2",             0xEE00EE },
        { "magenta3",             0xCD00CD },
        { "magenta4",             0x8B008B },
        { "maroon",               0x800000 },
        { "maroon",               0xB03060 },
        { "maroon1",              0xFF34B3 },
        { "maroon2",              0xEE30A7 },
        { "maroon3",              0xCD2990 },
        { "maroon4",              0x8B1C62 },
        { "mediumaquamarine",     0x66cdaa },
        { "mediumblue",           0x0000cd },
        { "mediumforestgreen",    0x32814b },
        { "mediumgoldenrod",      0xd1c166 },
        { "mediumorchid",         0xba55d3 },
        { "mediumorchid1",        0xe066ff },
        { "mediumorchid2",        0xd15fee },
        { "mediumorchid3",        0xb452cd },
        { "mediumorchid4",        0x7a378b },
        { "mediumpurple",         0x9370db },
        { "mediumpurple1",        0xab82ff },
        { "mediumpurple2",        0x9f79ee },
        { "mediumpurple3",        0x8968cd },
        { "mediumpurple4",        0x5d478b },
        { "mediumseagreen",       0x3cb371 },
        { "mediumslateblue",      0x7b68ee },
        { "mediumspringgreen",    0x00fa9a },
        { "mediumturquoise",      0x48d1cc },
        { "mediumvioletred",      0xc71585 },
        { "midnightblue",         0x191970 },
        { "mintcream",            0xf5fffa },
        { "mistyrose",            0xffe4e1 },
        { "mistyrose1",           0xffe4e1 },
        { "mistyrose2",           0xeed5d2 },
        { "mistyrose3",           0xcdb7b5 },
        { "mistyrose4",           0x8b7d7b },
        { "moccasin",             0xFFE4B5 },
        { "navajowhite",          0xffdead },
        { "navajowhite1",         0xffdead },
        { "navajowhite2",         0xeecfa1 },
        { "navajowhite3",         0xcdb38b },
        { "navajowhite4",         0x8b795e },
        { "navy",                 0x000080 },
        { "navyblue",             0x000080 },
        { "none",                 0x0000FF },
        { "oldlace",              0xfdf5e6 },
        { "olive",                0x808000 },
        { "olivedrab",            0x6b8e23 },
        { "olivedrab1",           0xc0ff3e },
        { "olivedrab2",           0xb3ee3a },
        { "olivedrab3",           0x9acd32 },
        { "olivedrab4",           0x698b22 },
        { "opaque",               0x000000 },
        { "orange",               0xFFA500 },
        { "orange1",              0xFFA500 },
        { "orange2",              0xEE9A00 },
        { "orange3",              0xCD8500 },
        { "orange4",              0x8B5A00 },
        { "orangered",            0xff4500 },
        { "orangered1",           0xff4500 },
        { "orangered2",           0xee4000 },
        { "orangered3",           0xcd3700 },
        { "orangered4",           0x8b2500 },
        { "orchid",               0xDA70D6 },
        { "orchid1",              0xFF83FA },
        { "orchid2",              0xEE7AE9 },
        { "orchid3",              0xCD69C9 },
        { "orchid4",              0x8B4789 },
        { "palegoldenrod",        0xeee8aa },
        { "palegreen",            0x98fb98 },
        { "palegreen1",           0x9aff9a },
        { "palegreen2",           0x90ee90 },
        { "palegreen3",           0x7ccd7c },
        { "palegreen4",           0x548b54 },
        { "paleturquoise",        0xafeeee },
        { "paleturquoise1",       0xbbffff },
        { "paleturquoise2",       0xaeeeee },
        { "paleturquoise3",       0x96cdcd },
        { "paleturquoise4",       0x668b8b },
        { "palevioletred",        0xdb7093 },
        { "palevioletred1",       0xff82ab },
        { "palevioletred2",       0xee799f },
        { "palevioletred3",       0xcd6889 },
        { "palevioletred4",       0x8b475d },
        { "papayawhip",           0xffefd5 },
        { "peachpuff",            0xffdab9 },
        { "peachpuff1",           0xffdab9 },
        { "peachpuff2",           0xeecbad },
        { "peachpuff3",           0xcdaf95 },
        { "peachpuff4",           0x8b7765 },
        { "peru",                 0xCD853F },
        { "pink",                 0xFFC0CB },
        { "pink1",                0xFFB5C5 },
        { "pink2",                0xEEA9B8 },
        { "pink3",                0xCD919E },
        { "pink4",                0x8B636C },
        { "plum",                 0xDDA0DD },
        { "plum1",                0xFFBBFF },
        { "plum2",                0xEEAEEE },
        { "plum3",                0xCD96CD },
        { "plum4",                0x8B668B },
        { "powderblue",           0xb0e0e6 },
        { "purple",               0x800080 },
        { "purple",               0xA020F0 },
        { "purple1",              0x9B30FF },
        { "purple2",              0x912CEE },
        { "purple3",              0x7D26CD },
        { "purple4",              0x551A8B },
        { "red",                  0xFF0000 },
        { "red1",                 0xFF0000 },
        { "red2",                 0xEE0000 },
        { "red3",                 0xCD0000 },
        { "red4",                 0x8B0000 },
        { "rosybrown",            0xbc8f8f },
        { "rosybrown1",           0xffc1c1 },
        { "rosybrown2",           0xeeb4b4 },
        { "rosybrown3",           0xcd9b9b },
        { "rosybrown4",           0x8b6969 },
        { "royalblue",            0x4169e1 },
        { "royalblue1",           0x4876ff },
        { "royalblue2",           0x436eee },
        { "royalblue3",           0x3a5fcd },
        { "royalblue4",           0x27408b },
        { "saddlebrown",          0x8b4513 },
        { "salmon",               0xFA8072 },
        { "salmon1",              0xFF8C69 },
        { "salmon2",              0xEE8262 },
        { "salmon3",              0xCD7054 },
        { "salmon4",              0x8B4C39 },
        { "sandybrown",           0xf4a460 },
        { "seagreen",             0x2e8b57 },
        { "seagreen1",            0x54ff9f },
        { "seagreen2",            0x4eee94 },
        { "seagreen3",            0x43cd80 },
        { "seagreen4",            0x2e8b57 },
        { "seashell",             0xFFF5EE },
        { "seashell1",            0xFFF5EE },
        { "seashell2",            0xEEE5DE },
        { "seashell3",            0xCDC5BF },
        { "seashell4",            0x8B8682 },
        { "sienna",               0xA0522D },
        { "sienna1",              0xFF8247 },
        { "sienna2",              0xEE7942 },
        { "sienna3",              0xCD6839 },
        { "sienna4",              0x8B4726 },
        { "silver",               0xC0C0C0 },
        { "skyblue",              0x87ceeb },
        { "skyblue1",             0x87ceff },
        { "skyblue2",             0x7ec0ee },
        { "skyblue3",             0x6ca6cd },
        { "skyblue4",             0x4a708b },
        { "slateblue",            0x6a5acd },
        { "slateblue1",           0x836fff },
        { "slateblue2",           0x7a67ee },
        { "slateblue3",           0x6959cd },
        { "slateblue4",           0x473c8b },
        { "slategray",            0x708090 },
        { "slategray1",           0xc6e2ff },
        { "slategray2",           0xb9d3ee },
        { "slategray3",           0x9fb6cd },
        { "slategray4",           0x6c7b8b },
        { "slategrey",            0x708090 },
        { "snow",                 0xFFFAFA },
        { "snow1",                0xFFFAFA },
        { "snow2",                0xEEE9E9 },
        { "snow3",                0xCDC9C9 },
        { "snow4",                0x8B8989 },
        { "springgreen",          0x00ff7f },
        { "springgreen1",         0x00ff7f },
        { "springgreen2",         0x00ee76 },
        { "springgreen3",         0x00cd66 },
        { "springgreen4",         0x008b45 },
        { "steelblue",            0x4682b4 },
        { "steelblue1",           0x63b8ff },
        { "steelblue2",           0x5cacee },
        { "steelblue3",           0x4f94cd },
        { "steelblue4",           0x36648b },
        { "tan",                  0xD2B48C },
        { "tan1",                 0xFFA54F },
        { "tan2",                 0xEE9A49 },
        { "tan3",                 0xCD853F },
        { "tan4",                 0x8B5A2B },
        { "teal",                 0x008080 },
        { "thistle",              0xD8BFD8 },
        { "thistle1",             0xFFE1FF },
        { "thistle2",             0xEED2EE },
        { "thistle3",             0xCDB5CD },
        { "thistle4",             0x8B7B8B },
        { "tomato",               0xFF6347 },
        { "tomato1",              0xFF6347 },
        { "tomato2",              0xEE5C42 },
        { "tomato3",              0xCD4F39 },
        { "tomato4",              0x8B3626 },
        { "transparent",          0x0000FF },
        { "turquoise",            0x40E0D0 },
        { "turquoise1",           0x00F5FF },
        { "turquoise2",           0x00E5EE },
        { "turquoise3",           0x00C5CD },
        { "turquoise4",           0x00868B },
        { "violet",               0xEE82EE },
        { "violetred",            0xd02090 },
        { "violetred1",           0xff3e96 },
        { "violetred2",           0xee3a8c },
        { "violetred3",           0xcd3278 },
        { "violetred4",           0x8b2252 },
        { "wheat",                0xF5DEB3 },
        { "wheat1",               0xFFE7BA },
        { "wheat2",               0xEED8AE },
        { "wheat3",               0xCDBA96 },
        { "wheat4",               0x8B7E66 },
        { "white",                0xFFFFFF },
        { "whitesmoke",           0xf5f5f5 },
        { "yellow",               0xFFFF00 },
        { "yellow1",              0xFFFF00 },
        { "yellow2",              0xEEEE00 },
        { "yellow3",              0xCDCD00 },
        { "yellow4",              0x8B8B00 },
        { "yellowgreen",          0x9acd32 },
#endif /* EXTENDED_XPM_COLORS */
        {"none",                  0xFFFFFF}
    };

    if (spec[0] == '#') {
        char buf[7];
        switch(speclen) {
        case 4:
            buf[0] = buf[1] = spec[1];
            buf[2] = buf[3] = spec[2];
            buf[4] = buf[5] = spec[3];
            break;
        case 7:
            SDL_memcpy(buf, spec + 1, 6);
            break;
        case 13:
            buf[0] = spec[1];
            buf[1] = spec[2];
            buf[2] = spec[5];
            buf[3] = spec[6];
            buf[4] = spec[9];
            buf[5] = spec[10];
            break;
        }
        buf[6] = '\0';
        *rgb = (Uint32)SDL_strtol(buf, NULL, 16);
        return 1;
    } else {
        int i;
        for (i = 0; i < SDL_arraysize(known); i++) {
            if (SDL_strncasecmp(known[i].name, spec, speclen) == 0) {
                *rgb = known[i].rgb;
                return 1;
            }
        }
        return 0;
    }
}

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

static char *linebuf;
static int buflen;
static char *error;

/*
 * Read next line from the source.
 * If len > 0, it's assumed to be at least len chars (for efficiency).
 * Return NULL and set error upon EOF or parse error.
 */
static char *get_next_line(char ***lines, SDL_RWops *src, int len)
{
    char *linebufnew;

    if (lines) {
        return *(*lines)++;
    } else {
        char c;
        int n;
        do {
            if (SDL_RWread(src, &c, 1, 1) <= 0) {
                error = "Premature end of data";
                return NULL;
            }
        } while (c != '"');
        if (len) {
            len += 4;   /* "\",\n\0" */
            if (len > buflen){
                buflen = len;
                linebufnew = (char *)SDL_realloc(linebuf, buflen);
                if (!linebufnew) {
                    SDL_free(linebuf);
                    error = "Out of memory";
                    return NULL;
                }
                linebuf = linebufnew;
            }
            if (SDL_RWread(src, linebuf, len - 1, 1) <= 0) {
                error = "Premature end of data";
                return NULL;
            }
            n = len - 2;
        } else {
            n = 0;
            do {
                if (n >= buflen - 1) {
                    if (buflen == 0)
                        buflen = 16;
                    buflen *= 2;
                    linebufnew = (char *)SDL_realloc(linebuf, buflen);
                    if (!linebufnew) {
                        SDL_free(linebuf);
                        error = "Out of memory";
                        return NULL;
                    }
                    linebuf = linebufnew;
                }
                if (SDL_RWread(src, linebuf + n, 1, 1) <= 0) {
                    error = "Premature end of data";
                    return NULL;
                }
            } while (linebuf[n++] != '"');
            n--;
        }
        linebuf[n] = '\0';
        return linebuf;
    }
}

#define SKIPSPACE(p)                \
do {                        \
    while (SDL_isspace((unsigned char)*(p))) \
          ++(p);                \
} while (0)

#define SKIPNONSPACE(p)                 \
do {                            \
    while (!SDL_isspace((unsigned char)*(p)) && *p)  \
          ++(p);                    \
} while (0)

/* read XPM from either array or RWops */
static SDL_Surface *load_xpm(char **xpm, SDL_RWops *src)
{
    Sint64 start = 0;
    SDL_Surface *image = NULL;
    int index;
    int x, y;
    int w, h, ncolors, cpp;
    int indexed;
    Uint8 *dst;
    struct color_hash *colors = NULL;
    SDL_Color *im_colors = NULL;
    char *keystrings = NULL, *nextkey;
    char *line;
    char ***xpmlines = NULL;
    int pixels_len;

    error = NULL;
    linebuf = NULL;
    buflen = 0;

    if (src)
        start = SDL_RWtell(src);

    if (xpm)
        xpmlines = &xpm;

    line = get_next_line(xpmlines, src, 0);
    if (!line)
        goto done;
    /*
     * The header string of an XPMv3 image has the format
     *
     * <width> <height> <ncolors> <cpp> [ <hotspot_x> <hotspot_y> ]
     *
     * where the hotspot coords are intended for mouse cursors.
     * Right now we don't use the hotspots but it should be handled
     * one day.
     */
    if (SDL_sscanf(line, "%d %d %d %d", &w, &h, &ncolors, &cpp) != 4
       || w <= 0 || h <= 0 || ncolors <= 0 || cpp <= 0) {
        error = "Invalid format description";
        goto done;
    }

    /* Check for allocation overflow */
    if ((size_t)(ncolors * cpp)/cpp != ncolors) {
        error = "Invalid color specification";
        goto done;
    }
    keystrings = (char *)SDL_malloc(ncolors * cpp);
    if (!keystrings) {
        error = "Out of memory";
        goto done;
    }
    nextkey = keystrings;

    /* Create the new surface */
    if (ncolors <= 256) {
        indexed = 1;
        image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8,
                         0, 0, 0, 0);
        im_colors = image->format->palette->colors;
        image->format->palette->ncolors = ncolors;
    } else {
        indexed = 0;
        image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
                         0xff0000, 0x00ff00, 0x0000ff, 0);
    }
    if (!image) {
        /* Hmm, some SDL error (out of memory?) */
        goto done;
    }

    /* Read the colors */
    colors = create_colorhash(ncolors);
    if (!colors) {
        error = "Out of memory";
        goto done;
    }
    for (index = 0; index < ncolors; ++index ) {
        char *p;
        line = get_next_line(xpmlines, src, 0);
        if (!line)
            goto done;

        p = line + cpp + 1;

        /* parse a colour definition */
        for (;;) {
            char nametype;
            char *colname;
            Uint32 rgb, pixel;

            SKIPSPACE(p);
            if (!*p) {
                error = "colour parse error";
                goto done;
            }
            nametype = *p;
            SKIPNONSPACE(p);
            SKIPSPACE(p);
            colname = p;
            SKIPNONSPACE(p);
            if (nametype == 's')
                continue;      /* skip symbolic colour names */

            if (!color_to_rgb(colname, (int)(p - colname), &rgb))
                continue;

            SDL_memcpy(nextkey, line, cpp);
            if (indexed) {
                SDL_Color *c = im_colors + index;
                c->r = (Uint8)(rgb >> 16);
                c->g = (Uint8)(rgb >> 8);
                c->b = (Uint8)(rgb);
                pixel = index;
            } else {
                pixel = rgb;
            }
            add_colorhash(colors, nextkey, cpp, pixel);
            nextkey += cpp;
            if (rgb == 0xffffffff)
                SDL_SetColorKey(image, SDL_TRUE, pixel);
            break;
        }
    }

    /* Read the pixels */
    pixels_len = w * cpp;
    dst = (Uint8 *)image->pixels;
    for (y = 0; y < h; y++) {
        line = get_next_line(xpmlines, src, pixels_len);
        if (!line)
            goto done;

        if (indexed) {
            /* optimization for some common cases */
            if (cpp == 1)
                for (x = 0; x < w; x++)
                    dst[x] = (Uint8)QUICK_COLORHASH(colors,
                                 line + x);
            else
                for (x = 0; x < w; x++)
                    dst[x] = (Uint8)get_colorhash(colors,
                                   line + x * cpp,
                                   cpp);
        } else {
            for (x = 0; x < w; x++)
                ((Uint32*)dst)[x] = get_colorhash(colors,
                                line + x * cpp,
                                  cpp);
        }
        dst += image->pitch;
    }

done:
    if (error) {
        if ( src )
            SDL_RWseek(src, start, RW_SEEK_SET);
        if ( image ) {
            SDL_FreeSurface(image);
            image = NULL;
        }
        IMG_SetError("%s", error);
    }
    if (keystrings)
        SDL_free(keystrings);
    free_colorhash(colors);
    if (linebuf)
        SDL_free(linebuf);
    return(image);
}

/* Load a XPM type image from an RWops datasource */
SDL_Surface *IMG_LoadXPM_RW(SDL_RWops *src)
{
    if ( !src ) {
        /* The error message has been set in SDL_RWFromFile */
        return NULL;
    }
    return load_xpm(NULL, src);
}

SDL_Surface *IMG_ReadXPMFromArray(char **xpm)
{
    if (!xpm) {
        IMG_SetError("array is NULL");
        return NULL;
    }
    return load_xpm(xpm, NULL);
}

#else  /* not LOAD_XPM */

/* See if an image is contained in a data source */
int IMG_isXPM(SDL_RWops *src)
{
    return(0);
}


/* Load a XPM type image from an SDL datasource */
SDL_Surface *IMG_LoadXPM_RW(SDL_RWops *src)
{
    return(NULL);
}

SDL_Surface *IMG_ReadXPMFromArray(char **xpm)
{
    return NULL;
}
#endif /* not LOAD_XPM */
