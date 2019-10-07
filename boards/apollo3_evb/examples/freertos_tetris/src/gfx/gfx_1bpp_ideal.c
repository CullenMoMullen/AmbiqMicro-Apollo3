////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//
//! \file    gfx_1bpp_ideal.c
//! \brief   Contains Ideal 1 bit per pixel routines.
//! \version 1.0
//! \date    04-October 2019
//!
////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "freertos_tetris.h"

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx_format.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_PutPixel_1BPP_Ideal(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap, uint16_t x, uint16_t y, gfx_Color_t cRGB)
{
    unsigned char *pData = (unsigned char*)pBitmap->pData;
    unsigned char uByte = 1<<((8-(x%8))-1);


    pData = &pData[y*((pBitmap->uWidth+7)/8)+ ((x)/8)];
    
    if((cRGB & 0x00FFFFFF) == (pPalette[PALETTE_BG_COLOR_IDX] & 0x00FFFFFF))//if the color is the background color,compare RGB color only
        *pData &= ~uByte;
    else
        *pData |= uByte;

}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx_format.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_GetPixel_1BPP_Ideal(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap, uint16_t x, uint16_t y, gfx_Color_t *pRGB)
{
    unsigned char *pData = (unsigned char*)pBitmap->pData;
    unsigned char uByte = 1<<((8-(x%8))-1);

    pData = &pData[y*((pBitmap->uWidth+7)/8)+ ((x)/8)];

    if(*pData & uByte)
        *pRGB = pPalette[PALETTE_FG_COLOR_IDX];
    else
        *pRGB = pPalette[PALETTE_BG_COLOR_IDX];
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx_format.h.
////////////////////////////////////////////////////////////////////////////////
uint32_t gfx_GetDataSize_1BPP_Ideal(uint16_t width, uint16_t height)
{
    return ((width+7)/8) * height;
}

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}

