////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//
//! \file    gfx_internal.h
//! \brief   Contains internal header data for the graphics library.
//! \version 2.0
//! \date    04-October 2019
//!
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFX_INTERNAL_H
#define __GFX_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

////! Global structure for the GFX library
//typedef struct _gfx_Globals
//{
//    gfx_Font_t *pSystemFont;            //!< Font used when no font is specified
//    util_lru_Cache_t *pBitmapCache;     //!< Bitmap Cache
//    gfx_ClipNode_t *pClipNodes;         //!< Pointer to the available clip nodes
//    gfx_RectNode_t *pRectNodes;         //!< Pointer to the available rect nodes
//} gfx_Globals_t;

extern gfx_Globals_t g_gfx_Globals;

//Functions pointer tables choosing the right function
extern void (*const g_BitBltFunctions[BITMAP_TYPE_COUNT][BITMAP_TYPE_COUNT])(gfx_Color_t *pPalette, gfx_Bitmap_t*,gfx_Rect_t,gfx_Bitmap_t*,int,int);
extern void (*const g_GetPixelFunctions[])(gfx_Color_t *pPalette, gfx_Bitmap_t *, uint16_t, uint16_t, gfx_Color_t *);
extern void (*const g_PutPixelFunctions[])(gfx_Color_t *pPalette, gfx_Bitmap_t *, uint16_t, uint16_t, gfx_Color_t );

gfx_RectNode_t *gfx_clip_Get(void);
void gfx_clip_Release(gfx_RectNode_t *pNode);
gfx_RectNode_t *gfx_rect_Subtract(gfx_RectNode_t *pRectNode, gfx_Rect_t *pSubtractRect);

void gfx_DrawLineSimple(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, gfx_Rect_t *pClipRect, int x0, int y0,
                        int x1, int y1, gfx_Color_t color);

//void gfx_blendline_16bpp_565(uint16_t *pSource, uint16_t *pDest, uint16_t uLineWidth, uint16_t alphaValue);
//void gfx_blendline_chroma_16bpp_565(uint16_t *pSource, uint16_t *pDest, uint16_t uLineWidth, uint16_t alphaValue, uint16_t uChromaRGB);


#ifdef __cplusplus
}
#endif


#endif

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}

