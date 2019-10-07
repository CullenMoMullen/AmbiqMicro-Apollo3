////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//!
//! \file    gfx_devicecontext.c
//! \brief   Contains functionality to implement the device context oriented functions for the Graphics subsystem.
//! \version 1.0
//! \date    04-October 2019
//!
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes and external references
////////////////////////////////////////////////////////////////////////////////
#include "freertos_tetris.h"
#include "types.h"
#include "error.h"

//By default only foreground and background color
gfx_Color_t  gfx_1BPP_DefaultPalette[2] = {    //!< Default palette.
    COLOR_BLACK,    //Draw Color (Pixel on)
    COLOR_WHITE     //Background Color (Pixel Off)
};

static BOOL bInitialized = FALSE;

gfx_DeviceContext_t deviceContext = {0};

gfx_DeviceContext_t* gfx_2d_DeviceContextGet(void)
{
    return &deviceContext;
}



uint32_t gfx_2d_DeviceContextInit(gfx_Bitmap_t* pFrameBuffer, gfx_Color_t *pPalette,
                                    uint16_t u16RectNodes, uint16_t u16ClipNodes){


    uint32_t retVal = gfx_Initialize(u16RectNodes, u16ClipNodes);
    if( SUCCESS == retVal ){
        deviceContext.bDirty = 0;
        deviceContext.i16xOffset = 0;
        deviceContext.i16yOffset = 0;
        deviceContext.pBitmap = pFrameBuffer;
        deviceContext.u16Height = pFrameBuffer->uHeight;
        deviceContext.u16Width = pFrameBuffer->uWidth;
        deviceContext.pPalette = pPalette ? pPalette : gfx_1BPP_DefaultPalette;
        deviceContext.u32MaxFrameSizeInBytes = gfx_format_GetDataSize(pFrameBuffer->uType)(pFrameBuffer->uWidth, pFrameBuffer->uHeight);

        bInitialized = TRUE;
        //as we call more functions with the pDC we can update the active clipping region
    }

    return retVal;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_SetDrawColor(gfx_DeviceContext_t *pDC, gfx_Color_t color)
{
    gfx_Color_t OldColor=pDC->pPalette[PALETTE_FG_COLOR_IDX];
    pDC->pPalette[PALETTE_FG_COLOR_IDX] = color;
    return OldColor;
}
////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_SetBackgroundColor(gfx_DeviceContext_t *pDC, gfx_Color_t color)
{
    gfx_Color_t OldColor=pDC->pPalette[PALETTE_BG_COLOR_IDX];
    pDC->pPalette[PALETTE_BG_COLOR_IDX] = color;
    return OldColor;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_org_Offset(gfx_DeviceContext_t *pDC, int x, int y)
{
    pDC->i16xOffset += x;
    pDC->i16yOffset += y;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_GetSystemDrawColor(void)
{
    return gfx_1BPP_DefaultPalette[PALETTE_FG_COLOR_IDX];
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_GetSystemBackgroundColor(void)
{
    return gfx_1BPP_DefaultPalette[PALETTE_BG_COLOR_IDX];
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t* gfx_2d_SetColorPalette(gfx_DeviceContext_t *pDC, gfx_Color_t *pPalette)
{
    gfx_Color_t *oldPalette = pDC->pPalette;
    pDC->pPalette = pPalette;
    return oldPalette;
}
////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}
