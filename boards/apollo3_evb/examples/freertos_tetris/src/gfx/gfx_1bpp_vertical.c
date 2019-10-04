#include "types.h"
#include "freertos_tetris.h"


////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx_format.h.
////////////////////////////////////////////////////////////////////////////////
uint32_t gfx_GetDataSize_1BPP_Vertical(uint16_t u16x, uint16_t u16y)
{
    return ((u16y+7)/8) * u16x;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx_format.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_PutPixel_1BPP_Vertical(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap, uint16_t x, uint16_t y, gfx_Color_t cRGB)
{
    uint8_t *pData = (uint8_t*)pBitmap->pData;
    uint8_t uByte = (1<<(y&0x07));

    pData = &pData[((y/8)*pBitmap->uWidth) + x];

    if((cRGB & 0x00FFFFFF) == (pPalette[1] & 0x00FFFFFF) ||
      ((cRGB & 0x00FFFFFF) == 0x00FFFFFF))//if the color is the background color or WHITE,compare RGB color only
        *pData &= ~uByte;
    else
        *pData |= uByte;

}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx_format.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_GetPixel_1BPP_Vertical(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap, uint16_t x, uint16_t y, gfx_Color_t *pRGB)
{
    uint8_t *pData = (uint8_t*)pBitmap->pData;
    uint8_t uByte = (1<<(y&0x07));

    pData = &pData[((y/8)*pBitmap->uWidth) + x];

    if(*pData & uByte)  // black dot
        *pRGB = pPalette[1];
    else    // white dot
        *pRGB = pPalette[0];
}


////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}
