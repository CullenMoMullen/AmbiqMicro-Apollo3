////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//
//! \file    gfx_bitblt.c
//! \brief   Contains runtime code for the Graphics subsystem.
//!
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Includes and external references
////////////////////////////////////////////////////////////////////////////////
#include "freertos_tetris.h"
#include "types.h"

#define MAX_BITMAP_SZ (320 * 4)

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
gfx_Bitmap_t *gfx_bmp_CreateBitmap(unsigned char uBitmapTypeIdentifier, uint16_t uWidth, uint16_t uHeight)
{
    gfx_format_GetDataSize_t *pGetDataSize;
    gfx_Bitmap_t *pBmp = NULL;
    uint32_t uSize;
    pGetDataSize = gfx_format_GetDataSize(uBitmapTypeIdentifier);
    uSize = sizeof(gfx_Bitmap_t) + pGetDataSize (uWidth,uHeight);
    pBmp = (gfx_Bitmap_t*)malloc(uSize);
    if(pBmp)
    {
        pBmp->uType = uBitmapTypeIdentifier;
        pBmp->uHeight= uHeight;
        pBmp->uWidth = uWidth;
    }
    return pBmp;
}


////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
bool gfx_bmp_GetPixel(gfx_Color_t *pPalette, gfx_Bitmap_t *pSrc, int x , int y, gfx_Color_t *pRGB)
{
    if(x <(int)pSrc->uWidth && y < (int)pSrc->uHeight)
    {
        gfx_format_GetPixel_t *pGetPixel = gfx_format_GetPixel(pSrc);
        assert(pGetPixel);
        pGetPixel(pPalette, pSrc, (uint16_t)x, (uint16_t)y, pRGB);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
bool gfx_bmp_PutPixel(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, int x , int y, gfx_Color_t cRGB)
{
    if(x <(int)pDest->uWidth && y < (int)pDest->uHeight)
    {
        gfx_format_PutPixel_t *pPutPixel  = gfx_format_PutPixel(pDest);
        assert(pPutPixel);
        pPutPixel(pPalette, pDest,(uint16_t)x,(uint16_t)y,cRGB);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx_format.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_bitblt_Generic(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, gfx_Rect_t Rect, gfx_Bitmap_t *pSrc, int SourceXoffset, int SourceYoffset)
{
    gfx_Color_t cRGB;
    int x, y;
    gfx_format_PutPixel_t *pPutPixel  = gfx_format_PutPixel(pDest);
    gfx_format_GetPixel_t *pGetPixel = gfx_format_GetPixel(pSrc);

    assert(pPutPixel);
    assert(pGetPixel);

    //! \todo:  optimize:   pre-calculate what the range and offsets will be so no comparisons happen during the blit
    //! \todo:  optimize:   perhaps make a 'get multiple pixels' so the penalty of function calling will be lessoned.
    for(y=0;y<Rect.height;y++)
    {
        if(y+SourceYoffset >= 0 && y+SourceYoffset < (int)pSrc->uHeight)
        {
            for(x=0;x<Rect.width;x++)
            {
                if(x+SourceXoffset >= 0 && x+SourceXoffset < (int)pSrc->uWidth)
                {
                    pGetPixel(pPalette,pSrc,x+SourceXoffset,y+SourceYoffset,&cRGB);
                    if(cRGB & 0xff000000)
                    {
                        pPutPixel(pPalette,pDest,x+Rect.x,y+Rect.y,cRGB);
                    }
                }
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief This function copies a bitmap onto another bitmap, bounded by the ClipRect.
//!
//! \param[in] pPalette Pointer to the color palette
//! \param[in] pDest                A pointer to the destination bitmap
//! \param[in] pSrc                 A pointer to the source bitmap
//! \param[in] ClipRect             The rectangle to draw within
//! \param[in] x                    The X position (relative to the upper left corner of the destination bitmap)
//!                                     to draw the source bitmap.
//! \param[in] y                    The Y position (relative to the upper left corner of the destination bitmap)
//!                                     to draw the source bitmap.
//! \note This function is an internal function and (probably) should not be called from the outside.
////////////////////////////////////////////////////////////////////////////////
void gfx_bitblt_Simple(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, gfx_Rect_t ClipRect, int x, int y, gfx_Bitmap_t *pSrc)
{
    void (*pBitBlt)(gfx_Color_t *pPalette, gfx_Bitmap_t*,gfx_Rect_t,
                                        gfx_Bitmap_t*,int,int);
    int SourceXOffset=0,SourceYOffset=0;
    //four easy early escapes because destination coordinates are
    //entirely outside of the destination rect
    if(x> ClipRect.x + ClipRect.width-1)
        return;
    if(y> ClipRect.y + ClipRect.height-1)
        return;
    if((int)(x + pSrc->uWidth-1) < ClipRect.x)
        return;
    if((int)(y + pSrc->uHeight-1)< ClipRect.y)
        return;

    // The following 4 if blocks will make sure that the clipping rectangle lies completely within the destination rect.
    // It is possible that this is already guranteed, however only becuase of the User Interface Manager.  To make this
    // function useable elsewhere, it is good to have this code.  Consider removing to save space if you find it
    // unnecessary.

    //This should fix the right hand edge
    if(ClipRect.x + ClipRect.width > (int)(pDest->uWidth))
    {
        ClipRect.width = pDest->uWidth - ClipRect.x;
    }

    //and the left hand edge.
    if(ClipRect.x < 0)
    {
        ClipRect.width += ClipRect.x;   //decrease the width of the clipping rect
        ClipRect.x = 0; //move the ClipRect left edge
    }

    //This should fix the bottom edge
    if(ClipRect.y + ClipRect.height> (int)(pDest->uHeight))
    {
        ClipRect.height= pDest->uHeight- ClipRect.y;
    }

    //and the top edge
    if(ClipRect.y < 0)
    {
        ClipRect.height += ClipRect.y;   //decrease the width of the clipping rect
        ClipRect.y = 0; //move the ClipRect left edge
    }

    //     (so the bounds checking can be removed from the optimized bitblt functions)
    //This should fix the right hand edge
    if(ClipRect.x + ClipRect.width > (int)(x + pSrc->uWidth))
    {
        ClipRect.width = x+pSrc->uWidth - ClipRect.x;
    }

    //and the left hand edge.
    if(ClipRect.x>x)
    {
        SourceXOffset = ClipRect.x-x;
    }
    else
    {
        ClipRect.width-= (x-ClipRect.x);
        ClipRect.x = x;
    }

    //This should fix the right bottom edge
    if(ClipRect.y + ClipRect.height> (int)(y + pSrc->uHeight))
    {
        ClipRect.height= y+pSrc->uHeight- ClipRect.y;
    }

    //and the top edge
    if(ClipRect.y>y)
    {
        SourceYOffset = ClipRect.y-y;
    }
    else
    {
        ClipRect.height -= (y-ClipRect.y);
        ClipRect.y = y;
    }

    pBitBlt = gfx_format_BitBlt(pSrc,pDest);
    pBitBlt(pPalette,pDest, ClipRect,pSrc,SourceXOffset, SourceYOffset);

}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_bmp_DrawBitmapByAddr(gfx_DeviceContext_t *pDC, gfx_Bitmap_t *pSrc, int x, int y )
{
    assert(pDC);
    assert(pSrc);
    //each entry in the clipping chain is a valid area to draw in
    if(pDC && pSrc)
    {
        gfx_RectNode_t *pClipNode = pDC->pActiveClipRegion;
        while(pClipNode)
        {   //cycle through each node and draw what's within it.
            gfx_bitblt_Simple(pDC->pPalette,pDC->pBitmap, pClipNode->Rect, x+pDC->i16xOffset, y + pDC->i16yOffset, pSrc);
            pClipNode = pClipNode->pNext;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Used to release the memory used by a bitmap back to the pool.
//!
//! \param[in] pBitmap Pointer to the gfx_Bitmap_t to release
////////////////////////////////////////////////////////////////////////////////
void gfx_bmp_DeleteBitmap(gfx_Bitmap_t* pBitmap)
{
    SystemHalt(1);
}

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}

