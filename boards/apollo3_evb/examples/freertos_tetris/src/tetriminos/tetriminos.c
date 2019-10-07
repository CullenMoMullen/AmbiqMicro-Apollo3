//*****************************************************************************
//
//! @file tetriminos.c
//!
//! If enabled, debug messages will be sent over ITM.
//
//*****************************************************************************

//*****************************************************************************
// The tetriminos are in 1BPP Vertical format
//*****************************************************************************
#include "freertos_tetris.h"

/*
const unsigned char oMask1 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 8,
0xFF, 0xD5, 0xAB, 0xD5, 0xAB, 0xD5, 0xAB, 0xFF,
};
*/

tetrimino_t oMask[NUM_O_ROTATIONS] = {
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x60,0x60,0x00
        },
    }
};



/*
const unsigned char iMask1 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
16, 4,

3896

};

const unsigned char iMask2 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
4, 16,
0xFF, 0x55, 0xAB, 0xFF, 0xFF, 0xD5, 0xAA, 0xFF,
};
*/

tetrimino_t iMasks[NUM_I_ROTATIONS] = {
    {
        BITMAP_TYPE_1BPP_VERTICAL, 4, 4,
        {
            0x40,0x40,0x40,0x40
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,4,4, {0x00,0x00,0xF0,0x00},
    }
};


/*
const unsigned char sMask1 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12, 8,
0xF0, 0xB0, 0xD0, 0xB0, 0xDF, 0xAB, 0xD5, 0xFB, 0x0D, 0x0B, 0x0D, 0x0F,
};

const unsigned char sMask2 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0xFF, 0xD5, 0xAB, 0xDF, 0xB0, 0x50, 0xB0, 0xF0, 0x00, 0x00, 0x00, 0x00,
};
*/


tetrimino_t sMasks[NUM_S_ROTATIONS] = {
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x20,0x60,0x40
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x00,0xC0,0x60
        },
    }
};


/*
const unsigned char zMask1 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12, 8,
0x0F, 0x0D, 0x0B, 0x0D, 0xFB, 0xD5, 0xAB, 0xDF, 0xB0, 0xD0, 0xB0, 0xF0,
};

const unsigned char zMask2 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0xF0, 0xB0, 0x50, 0xB0, 0xDF, 0xAB, 0xD5, 0xFF, 0x0F, 0x0A, 0x0D, 0x0F,
};
*/

tetrimino_t zMasks[NUM_Z_ROTATIONS] =
{
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x40,0x60,0x20
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x00,0x60,0xC0
        },
    }
};

/*
const unsigned char lMask1 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12,8,
0xF0, 0xB0, 0xD0, 0xB0, 0xD0, 0xB0, 0xD0, 0xB0, 0xDF, 0xAB, 0xD5, 0xFF,
};

const unsigned char lMask2 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0xFF, 0x55, 0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0D, 0x0A, 0x0D,
};

const unsigned char lMask3 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12,8,
0xFF, 0xAB, 0xD5, 0xFB, 0x0D, 0x0B, 0x0D, 0x0B, 0x0D, 0x0B, 0x0D, 0x0F,
};

const unsigned char lMask4 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0x0F, 0x0D, 0x0B, 0x0D, 0xFB, 0x55, 0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00,
};
*/

tetrimino_t lMasks[NUM_L_ROTATIONS] = {
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x40,0x40,0xC0
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x00,0xE0,0x20
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x60,0x40,0x40
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x80,0xE0,0x00
        },
    }
};

/*
const unsigned char jMask1 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12, 8,
0xFF, 0xD5, 0xAB, 0xDF, 0xB0, 0xD0, 0xB0, 0xD0, 0xB0, 0xD0, 0xB0, 0xF0,
};

const unsigned char jMask2 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0xFF, 0xAB, 0x55, 0xFB, 0x0D, 0x0B, 0x0D, 0x0F, 0x0F, 0x0A, 0x0D, 0x0F,
};

const unsigned char jMask3 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12, 8,
0x0F, 0x0D, 0x0B, 0x0D, 0x0B, 0x0D, 0x0B, 0x0D, 0xFB, 0xD5, 0xAB, 0xFF,
};

const unsigned char jMask4 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0x00, 0x00, 0x00, 0x00, 0xFF, 0xAB, 0x55, 0xFF, 0x0F, 0x0B, 0x0D, 0x0B,
};
*/

tetrimino_t jMasks[NUM_J_ROTATIONS] = {
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0xC0,0x40,0x40,0x00
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0xE0,0x80,0x00
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x40,0x40,0x60,0x00
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x20,0xE0,0x00,0x00
        },
    }
};


/*
const unsigned char tMask1 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12, 8,
0xF0, 0xD0, 0xB0, 0xD0, 0xBF, 0xD5, 0xAB, 0xDF, 0xB0, 0xD0, 0xB0, 0xF0,
};

const unsigned char tMask2 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0xFF, 0xAB, 0x55, 0xBF, 0xD0, 0xB0, 0xD0, 0xF0, 0x0F, 0x0A, 0x0D, 0x0F,
};

const unsigned char tMask3 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
12, 8,
0x0F, 0x0D, 0x0B, 0x0D, 0xFB, 0xD5, 0xAB, 0xFD, 0x0B, 0x0D, 0x0B, 0x0F,
};

const unsigned char tMask4 [] = {
BITMAP_TYPE_1BPP_VERTICAL,
8, 12,
0xF0, 0xB0, 0xD0, 0xB0, 0xDF, 0xAB, 0x55, 0xFF, 0x00, 0x00, 0x00, 0x00,
};
*/

tetrimino_t tMasks[NUM_T_ROTATIONS] = {
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x40,0xC0,0x40
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x00,0xE0,0x40
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x40,0x60,0x40
        },
    },
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        4,4,
        {
            0x00,0x40,0xE0,0x00
        },
    }
};
