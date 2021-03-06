/*
 * Convert VGA text mode memory dump into ANSI escape sequences
 * compatible with xterm256.
 *
 * Copyright (c) 2018 Leandro A. F. Pereira <leandro@tia.mat.br>
 * Licensed under the terms of LGPLv3:
 *      https://www.gnu.org/licenses/lgpl-3.0.en.html
 */

#include <stdio.h>

enum colors {
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN,
    LIGHTGRAY, DARKGRAY, BRIGHTBLUE, BRIGHTGREEN,
    BRIGHTCYAN, BRIGHTRED, BRIGHTMAGENTA, YELLOW,
    BRIGHTWHITE
};

static const int vga_to_xterm256[] = {
    [BLACK] = 0,
    [BLUE] = 19,
    [GREEN] = 34,
    [CYAN] = 37,
    [RED] = 124,
    [MAGENTA] = 127,
    [BROWN] = 130,
    [LIGHTGRAY] = 145,
    [DARKGRAY] = 59,
    [BRIGHTBLUE] = 63,
    [BRIGHTGREEN] = 83,
    [BRIGHTCYAN] = 87,
    [BRIGHTRED] = 203,
    [BRIGHTMAGENTA] = 207,
    [YELLOW] = 227,
    [BRIGHTWHITE] = 15
};

static char* cp850_to_utf8[256] = {
    [0] = "\x0",
    [1] = "\x1",
    [2] = "\x2",
    [3] = "\x3",
    [4] = "\x4",
    [5] = "\x5",
    [6] = "\x6",
    [7] = "\x7",
    [8] = "\x8",
    [9] = "\x9",
    [10] = "\xa",
    [11] = "\xb",
    [12] = "\xc",
    [13] = "\xd",
    [14] = "\xe",
    [15] = "\xf",
    [16] = "\x10",
    [17] = "\x11",
    [18] = "\u2195",
    [19] = "\x13",
    [20] = "\x14",
    [21] = "\x15",
    [22] = "\x16",
    [23] = "\x17",
    [24] = "\u2191",
    [25] = "\u2193",
    [26] = "\u2192",
    [27] = "\u2190",
    [28] = "\x1c",
    [29] = "\x1d",
    [30] = "\u25b2",
    [31] = "\u25bc",
    [32] = "\x20",
    [33] = "\x21",
    [34] = "\x22",
    [35] = "\x23",
    [36] = "\x24",
    [37] = "\x25",
    [38] = "\x26",
    [39] = "\x27",
    [40] = "\x28",
    [41] = "\x29",
    [42] = "\x2a",
    [43] = "\x2b",
    [44] = "\x2c",
    [45] = "\x2d",
    [46] = "\x2e",
    [47] = "\x2f",
    [48] = "\x30",
    [49] = "\x31",
    [50] = "\x32",
    [51] = "\x33",
    [52] = "\x34",
    [53] = "\x35",
    [54] = "\x36",
    [55] = "\x37",
    [56] = "\x38",
    [57] = "\x39",
    [58] = "\x3a",
    [59] = "\x3b",
    [60] = "\x3c",
    [61] = "\x3d",
    [62] = "\x3e",
    [63] = "\x3f",
    [64] = "\x40",
    [65] = "\x41",
    [66] = "\x42",
    [67] = "\x43",
    [68] = "\x44",
    [69] = "\x45",
    [70] = "\x46",
    [71] = "\x47",
    [72] = "\x48",
    [73] = "\x49",
    [74] = "\x4a",
    [75] = "\x4b",
    [76] = "\x4c",
    [77] = "\x4d",
    [78] = "\x4e",
    [79] = "\x4f",
    [80] = "\x50",
    [81] = "\x51",
    [82] = "\x52",
    [83] = "\x53",
    [84] = "\x54",
    [85] = "\x55",
    [86] = "\x56",
    [87] = "\x57",
    [88] = "\x58",
    [89] = "\x59",
    [90] = "\x5a",
    [91] = "\x5b",
    [92] = "\x5c",
    [93] = "\x5d",
    [94] = "\x5e",
    [95] = "\x5f",
    [96] = "\x60",
    [97] = "\x61",
    [98] = "\x62",
    [99] = "\x63",
    [100] = "\x64",
    [101] = "\x65",
    [102] = "\x66",
    [103] = "\x67",
    [104] = "\x68",
    [105] = "\x69",
    [106] = "\x6a",
    [107] = "\x6b",
    [108] = "\x6c",
    [109] = "\x6d",
    [110] = "\x6e",
    [111] = "\x6f",
    [112] = "\x70",
    [113] = "\x71",
    [114] = "\x72",
    [115] = "\x73",
    [116] = "\x74",
    [117] = "\x75",
    [118] = "\x76",
    [119] = "\x77",
    [120] = "\x78",
    [121] = "\x79",
    [122] = "\x7a",
    [123] = "\x7b",
    [124] = "\x7c",
    [125] = "\x7d",
    [126] = "\x7e",
    [127] = "\x7f",
    [128] = "\u00C7", // Ç
    [129] = "\u00FC", // ü
    [130] = "\u00E9", // é
    [131] = "\u00E2", // â
    [132] = "\u00E4", // ä
    [133] = "\u00E0", // à
    [134] = "\u00E5", // å
    [135] = "\u00E7", // ç
    [136] = "\u00EA", // ê
    [137] = "\u00EB", // ë
    [138] = "\u00E8", // è
    [139] = "\u00EF", // ï
    [140] = "\u00EE", // î
    [141] = "\u00EC", // ì
    [142] = "\u00C4", // Ä
    [143] = "\u00C5", // Å
    [144] = "\u00C9", // É
    [145] = "\u00E6", // æ
    [146] = "\u00C6", // Æ
    [147] = "\u00F4", // ô
    [148] = "\u00F6", // ö
    [149] = "\u00F2", // ò
    [150] = "\u00FB", // û
    [151] = "\u00F9", // ù
    [152] = "\u00FF", // ÿ
    [153] = "\u00D6", // Ö
    [154] = "\u00DC", // Ü
    [155] = "\u00F8", // ø
    [156] = "\u00A3", // £
    [157] = "\u00D8", // Ø
    [158] = "\u00D7", // ×
    [159] = "\u0192", // ƒ
    [160] = "\u00E1", // á
    [161] = "\u00ED", // í
    [162] = "\u00F3", // ó
    [163] = "\u00FA", // ú
    [164] = "\u00F1", // ñ
    [165] = "\u00D1", // Ñ
    [166] = "\u00AA", // ª
    [167] = "\u00BA", // º
    [168] = "\u00BF", // ¿
    [169] = "\u00AE", // ®
    [170] = "\u00AC", // ¬
    [171] = "\u00BD", // ½
    [172] = "\u00BC", // ¼
    [173] = "\u00A1", // ¡
    [174] = "\u00AB", // «
    [175] = "\u00BB", // »
    [176] = "\u2591", // ░
    [177] = "\u2592", // ▒
    [178] = "\u2593", // ▓
    [179] = "\u2502", // │
    [180] = "\u2524", // ┤
    [181] = "\u00C1", // Á
    [182] = "\u00C2", // Â
    [183] = "\u00C0", // À
    [184] = "\u00A9", // ©
    [185] = "\u2563", // ╣
    [186] = "\u2551", // ║
    [187] = "\u2557", // ╗
    [188] = "\u255D", // ╝
    [189] = "\u00A2", // ¢
    [190] = "\u00A5", // ¥
    [191] = "\u2510", // ┐
    [192] = "\u2514", // └
    [193] = "\u2534", // ┴
    [194] = "\u252C", // ┬
    [195] = "\u251C", // ├
    [196] = "\u2500", // ─
    [197] = "\u253C", // ┼
    [198] = "\u00E3", // ã
    [199] = "\u00C3", // Ã
    [200] = "\u255A", // ╚
    [201] = "\u2554", // ╔
    [202] = "\u2569", // ╩
    [203] = "\u2566", // ╦
    [204] = "\u2560", // ╠
    [205] = "\u2550", // ═
    [206] = "\u256C", // ╬
    [207] = "\u00A4", // ¤
    [208] = "\u00F0", // ð
    [209] = "\u00D0", // Ð
    [210] = "\u00CA", // Ê
    [211] = "\u00CB", // Ë
    [212] = "\u00C8", // È
    [213] = "\u0131", // ı
    [214] = "\u00CD", // Í
    [215] = "\u00CE", // Î
    [216] = "\u00CF", // Ï
    [217] = "\u2518", // ┘
    [218] = "\u250C", // ┌
    [219] = "\u2588", // █
    [220] = "\u2584", // ▄
    [221] = "\u00A6", // ¦
    [222] = "\u00CC", // Ì
    [223] = "\u2580", // ▀
    [224] = "\u00D3", // Ó
    [225] = "\u00DF", // ß
    [226] = "\u00D4", // Ô
    [227] = "\u00D2", // Ò
    [228] = "\u00F5", // õ
    [229] = "\u00D5", // Õ
    [230] = "\u00B5", // µ
    [231] = "\u00FE", // þ
    [232] = "\u00DE", // Þ
    [233] = "\u00DA", // Ú
    [234] = "\u00DB", // Û
    [235] = "\u00D9", // Ù
    [236] = "\u00FD", // ý
    [237] = "\u00DD", // Ý
    [238] = "\u00AF", // ¯
    [239] = "\u00B4", // ´
    [240] = "\u00AD", // SHY
    [241] = "\u00B1", // ±
    [242] = "\u2017", // ‗
    [243] = "\u00BE", // ¾
    [244] = "\u00B6", // ¶
    [245] = "\u00A7", // §
    [246] = "\u00F7", // ÷
    [247] = "\u00B8", // ¸
    [248] = "\u00B0", // °
    [249] = "\u00A8", // ¨
    [250] = "\u00B7", // ·
    [251] = "\u00B9", // ¹
    [252] = "\u00B3", // ³
    [253] = "\u00B2", // ²
    [254] = "\u25A0", // ■
    [255] = "\u00A0", // NBSP
};

static void to_xterm256(unsigned char attribute, int *fg, int *bg)
{
    *bg = vga_to_xterm256[attribute >> 4];
    *fg = vga_to_xterm256[attribute & 0x0f];
}

static const char *to_utf8(unsigned char character)
{
    return cp850_to_utf8[character];
}

static void convert_to_ansi(FILE *dump)
{
    int chars;
    int prev_bg = 0;
    int prev_fg = 0;

    for (chars = 0; chars < 80 * 24; chars++) {
        unsigned char character, attribute;
        int fg, bg;

        if (chars > 0 && chars % 80 == 0)
            putchar('\n');

        character = fgetc(dump);
        attribute = fgetc(dump);

        to_xterm256(attribute, &fg, &bg);

        if (prev_bg != bg) {
            prev_bg = bg;
            printf("\033[48;5;%dm", bg);
        }
        if (prev_fg != fg) {
            prev_fg = fg;
            printf("\033[38;5;%dm", fg);
        }

        printf("%s", to_utf8(character));
    }
}

int main(int argc, char *argv[])
{
    FILE *memdump;

    if (argc < 2)
        memdump = fopen("dump1.bin", "rbe");
    else
        memdump = fopen(argv[1], "rbe");
    if (!memdump)
        return 1;

    convert_to_ansi(memdump);

    fclose(memdump);
}
