//*****************************************************************************
//
//! @file tetriminos.c
//!
//! If enabled, debug messages will be sent over ITM.
//
//*****************************************************************************

#ifndef __TETRIMINOS_H__
#define __TETRIMINOS_H__

//*****************************************************************************
// The tetriminos are in 1BPP Vertical format
//*****************************************************************************
#define NUM_O_ROTATIONS (1)
#define NUM_I_ROTATIONS (2)
#define NUM_Z_ROTATIONS (2)
#define NUM_S_ROTATIONS (2)
#define NUM_L_ROTATIONS (4)
#define NUM_J_ROTATIONS (4)
#define NUM_T_ROTATIONS (4)

typedef struct __tetrimino_t__ {
    //! \brief Type of bitmap this header is
    unsigned int uType      : 8;
    //! \brief Width of bitmap (in pixels)
    unsigned int uWidth     : 12;
    //! \brief Height of bitmap (in pixels)
    unsigned int uHeight    : 12;
    //! \brief Data of the bitmap
    unsigned char     pData[4];
} tetrimino_t;

extern tetrimino_t jMasks[];
extern tetrimino_t lMasks[];
extern tetrimino_t iMasks[];
extern tetrimino_t tMasks[];
extern tetrimino_t zMasks[];
extern tetrimino_t sMasks[];
extern tetrimino_t oMasks[];

#endif
