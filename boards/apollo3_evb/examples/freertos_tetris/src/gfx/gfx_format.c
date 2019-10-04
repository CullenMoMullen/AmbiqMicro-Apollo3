////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//
//! \file    gfx_format.c
//! \brief   Contains the setup code to define support for different bitmap formats.
//! \version 1.0
//! \date    04-October 2019
//!
//! \internal
//!
//! The tables in this file define support for specific bitmap formats and acceleration.
//!
////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "freertos_tetris.h"


//! \brief Internal function to search the supported format list
//!
//! \internal
//!
//! The other functions below use this common function to return a pointer to the
//! format specifying structure.
//!
//! \param[in] uFormatCode the format code to find the supporting support structure for.
//! \return a pointer to the \ref _gfx_BitmapFormat "gfx_BitmapFormat_t" for the bitmap
//!                         type in question.
const gfx_BitmapFormat_t *gfx_format_GetFormat(uint8_t uFormatCode)
{
    const gfx_BitmapFormat_t *pFormat = g_gfx_BitmapSupportTable;
    while(pFormat->uUniqueIdentifier != BITMAP_TYPE_COUNT)
    {
        if(pFormat->uUniqueIdentifier == uFormatCode)
            break;
        pFormat ++;
    }
    if(pFormat->uUniqueIdentifier == BITMAP_TYPE_COUNT)
        pFormat = NULL;
    assert(pFormat);
    return pFormat;
}

////////////////////////////////////////////////////////////////////////////////
// For information about this function please see gfx_format.h
////////////////////////////////////////////////////////////////////////////////
gfx_format_GetPixel_t    *gfx_format_GetPixel(gfx_Bitmap_t* pBmp)
{
    gfx_format_GetPixel_t *pGetPixel = gfx_format_GetFormat(pBmp->uType)->GetPixelFunction;

    assert(pGetPixel);
    return pGetPixel;
}

////////////////////////////////////////////////////////////////////////////////
// For information about this function please see gfx_format.h
////////////////////////////////////////////////////////////////////////////////
gfx_format_PutPixel_t    *gfx_format_PutPixel(gfx_Bitmap_t* pBmp)
{
    gfx_format_PutPixel_t *pPutPixel = gfx_format_GetFormat(pBmp->uType)->PutPixelFunction;

    assert(pPutPixel);
    return pPutPixel;
}

////////////////////////////////////////////////////////////////////////////////
// For information about this function please see gfx_format.h
////////////////////////////////////////////////////////////////////////////////
gfx_format_GetDataSize_t *gfx_format_GetDataSize(uint8_t uFormatCode)
{
    gfx_format_GetDataSize_t *pGetDataSize = gfx_format_GetFormat(uFormatCode)->GetBitmapDataSize;

    assert(pGetDataSize);
    return pGetDataSize;
}

////////////////////////////////////////////////////////////////////////////////
// For information about this function please see gfx_format.h
////////////////////////////////////////////////////////////////////////////////
gfx_format_LineDraw_t    *gfx_format_GetLineDraw(gfx_Bitmap_t* pBmp)
{

    gfx_format_LineDraw_t *pLineDraw=gfx_format_GetFormat(pBmp->uType)->DrawLineFunction;
    assert(pLineDraw);
    return pLineDraw;
}

////////////////////////////////////////////////////////////////////////////////
// For information about this function please see gfx_format.h
////////////////////////////////////////////////////////////////////////////////
gfx_format_GetBitBlt_t   *gfx_format_BitBlt(gfx_Bitmap_t*pSource, gfx_Bitmap_t*pDest)
{
    const gfx_format_BitBltEntry_t *pEntry = g_gfx_BitBltFunctions;
    gfx_format_GetBitBlt_t *pBitBlt = gfx_bitblt_Generic;

    while(pEntry->pBitBltFunction)
    {
        if((pEntry->uSourceIdentifier==pSource->uType) &&
           (pEntry->uDestinationIdentifier== pDest->uType))
        {
            pBitBlt = pEntry->pBitBltFunction;
            break;
        }
        pEntry++;
    }
    return pBitBlt;
}

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}
