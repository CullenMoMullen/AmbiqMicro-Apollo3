////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//
//! \file    gfx_primatives.c
//! \brief   Contains functionality that implements simple line drawing and rectangle operations.
////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "freertos_tetris.h"
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a line, pre-clipped, in a given color to a given bitmap.
//! 
//! This function draws a line, pre-clipped, onto any supported bitmap.
//! 
//! \param[in] pPalette  Pointer to the color palette.
//! \param[in] pDest     gfx_DeviceContext_t to operate on to check within.
//! \param[in] x0        x of point 0
//! \param[in] y0        y of point 0
//! \param[in] x1        x of point 1
//! \param[in] y1        y of point 1
//! \param[in] color     Color of the line
//!
//! \todo This function may be excess functionality than required.  The typical usage of line drawing
//!       in our system is horizontal or vertical lines, which are much simpler to clip.
//! \todo optimize:   the two branches (whether x or y is the incrementer) can be collapesed into one using
//!                   pointers to the original data.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_DrawClippedLineGeneric(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, int x0, int y0, int x1, int y1, gfx_Color_t color)
{
    bool bInvert = false;
    int32_t dx, dy;
    int32_t D;
    gfx_format_PutPixel_t *pPutPixel= gfx_format_PutPixel(pDest);

    assert(pPutPixel);

    if(y0==y1)
    {//its a horizontal line
        if(x0 > x1)
        {
            uint16_t temp = x0;
            x0 = x1;
            x1 = temp;
        }
        for(;x0<=x1;x0++)
        {
            pPutPixel(pPalette,pDest,x0,y0,color);
        }
    }
    else if (x0==x1)
    {//a vertical line
        if(y0 > y1)
        {
            uint16_t temp = y0;
            y0 = y1;
            y1 = temp;
        }
        for(;y0<=y1;y0++)
        {
            pPutPixel(pPalette,pDest,x0,y0,color);
        }
    }
    else
    {
        dx = x1-x0;
        dy = y1-y0;
        if(abs(dx)> abs(dy))
        {//x is our incrementer
            //we're going to always make the line draw from left to right.
            if(x0>x1)
            {
                int32_t iTemp = x0;
                x0=x1;
                x1 = iTemp;
                iTemp = y0;
                y0=y1;
                y1 = iTemp;
                dx = x1-x0;
                dy = y1-y0;
            }
            if(dy < 0)
            {
                dy = -dy;
                bInvert = true;
            }
            D = 2*dy - dx;
            for(;x0<=x1;x0++)
            {
                pPutPixel(pPalette,pDest,x0,y0,color);//this can be dangerous.  The function being called does not
                                            //do any clipping.  If we're outside the bitmap, things will get
                                            //corrupted.
                if(D <= 0)
                {
                    D+= 2*dy;
                }
                else
                {
                    D+= 2*(dy-dx);
                    if(!bInvert)
                        y0++;
                    else
                        y0--;
                }
            }
        }
        else
        {//y is our incrementer
            //we're always going to draw from top to bottom.
            if(y1<y0)
            {
                int32_t iTemp = y1;
                y1= y0;
                y0 = iTemp;
                iTemp = x1;
                x1= x0;
                x0 = iTemp;
                dx = x1-x0;
                dy = y1-y0;
            }
            if(dx < 0)
            {
                dx = -dx;
                bInvert = true;
            }
            D = 2*dx - dy;

            for(;y0<=y1;y0++)
            {
                pPutPixel(pPalette,pDest,x0,y0,color);
                if(D <= 0)
                {
                    D+= 2*dx;
                }
                else
                {
                    D+= 2*(dx-dy);
                    if(!bInvert)
                        x0++;
                    else
                        x0--;
                }
            }
        }
    }
}

#define LEFT_OF_WINDOW  0x1
#define RIGHT_OF_WINDOW 0x2
#define BELOW_WINDOW    0x4
#define ABOVE_WINDOW    0x8

////////////////////////////////////////////////////////////////////////////////
//! \internal
//!
//! \brief Determines which region the point is in
//! \param[in] pClipRect    The rectangle we're interested in
//! \param[in] x            x coord of the point
//! \param[in] y            y coord of the point
//!
//! \todo This function may be excess functionality than required.  The typical usage of line drawing
//!       in our system is horizontal or vertical lines, which are much simpler to clip.
////////////////////////////////////////////////////////////////////////////////
uint8_t gfx_GetPointsRegionCode(gfx_Rect_t *pClipRect, int32_t x,int32_t y)
{
    uint8_t u8Code = 0;
    if(x<pClipRect->x)
    {
        u8Code |= LEFT_OF_WINDOW;
    }
    else  if(x>=pClipRect->x+pClipRect->width)
    {
        u8Code |= RIGHT_OF_WINDOW;
    }
    if(y<pClipRect->y)
    {
        u8Code |= ABOVE_WINDOW;
    }
    else if(y>=pClipRect->y + pClipRect->height)
    {
        u8Code |= BELOW_WINDOW;
    }
    return u8Code;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a line onto a bitmap, clipped by a rectangle.
//!
//! \param[in] pPalette  Pointer to the color palette.
//! \param[in] pDest        The destination bitmap
//! \param[in] pClipRect    The clipping rectangle
//! \param[in] x0           x of point 0
//! \param[in] y0           y of point 0
//! \param[in] x1           x of point 1
//! \param[in] y1           y of point 1
//! \param[in] color        The color of the line
//!
//! \todo This function may be excess functionality than required.  The typical usage of line drawing
//!       in our system is horizontal or vertical lines, which are much simpler to clip.
////////////////////////////////////////////////////////////////////////////////
void gfx_DrawLineSimple(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, gfx_Rect_t *pClipRect, int x0, int y0, int x1, int y1, gfx_Color_t color)
{
    gfx_format_LineDraw_t *pDrawLine = gfx_format_GetLineDraw(pDest);

    uint8_t u8P0Code;
    uint8_t u8P1Code;

    if(x0 > x1)
    {//ensure we're going left->right
        int32_t temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0=y1;
        y1=temp;
    }

    u8P0Code = gfx_GetPointsRegionCode(pClipRect,x0,y0);
    u8P1Code = gfx_GetPointsRegionCode(pClipRect,x1,y1);

    if(u8P0Code | u8P1Code)
    {   //only do this if we've got some line drawing that is outside the clip rect
        if(u8P0Code & u8P1Code)
        {//if this case is true, the line cannot cross the clip ret
            return;
        }
        else
        {
            int32_t dx = x0-x1;
            int32_t dy = y0-y1;

            if(!dx)
            {//its a vertical line
                if(u8P0Code & ABOVE_WINDOW)
                {
                    y0 = pClipRect->y;
                } else if(u8P0Code & BELOW_WINDOW)
                {
                    y0 = pClipRect->y + pClipRect->height-1;
                }
                if(u8P1Code & ABOVE_WINDOW)
                {
                    y1 = pClipRect->y;
                } else if(u8P1Code & BELOW_WINDOW)
                {
                    y1 = pClipRect->y + pClipRect->height-1;
                }
            } 
            else if(!dy)
            {//horizontal line.  (only need to check if x0 < and x1 >, because we've arranged them that way)
                if(u8P0Code & LEFT_OF_WINDOW)
                    x0 = pClipRect->x;
                if(u8P1Code & RIGHT_OF_WINDOW)
                    x1 = pClipRect->x + pClipRect->width-1;
            }
            else
            {//got a non-orthogonal line
                int32_t Const = y0 - (dy * x0)/dx;

                //point 0
                if (x0 < pClipRect->x)
                {
                    x0 = pClipRect->x;
                    y0 = (dy * x0 / dx) + Const;
                }
                //x0 can't be > pClipRect->x + pClipRect->width -1
                if (y0 < pClipRect->y)
                {
                    y0 = pClipRect->y;
                    x0 = (y0-Const) * dx / dy;
                } 
                else if (y0 >= pClipRect->y + pClipRect->height -1)
                {
                    y0 = pClipRect->y + pClipRect->height-1;
                    x0 = (y0-Const) * dx / dy;
                }
                //make sure our secondary calculations haven't put us outside the rect
                if(x0 < pClipRect->x)
                    return;

                //point 1
                //x1 can't be < pClipRect->x
                if(x1 >= pClipRect->x + pClipRect->width -1)
                {
                    x1 = pClipRect->x + pClipRect->width -1;
                    y1 = (dy * x1 / dx) + Const;
                }
                if (y1 < pClipRect->y)
                {
                    y1 = pClipRect->y;
                    x1 = (y1-Const) * dx / dy;
                } 
                else if (y1 >= pClipRect->y + pClipRect->height-1)
                {
                    y1 = pClipRect->y + pClipRect->height-1;
                    x1 = (y1-Const) * dx / dy;
                }
                //make sure our secondary calculations haven't put us outside the rect
                if(x1 >= pClipRect->x + pClipRect->width)
                    return;
            }
        }
    }
    pDrawLine(pPalette,pDest,x0,y0,x1,y1,color);
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_DrawLine(gfx_DeviceContext_t *pDC, int x0, int y0, int x1, int y1, gfx_Color_t color)
{
    gfx_RectNode_t *pClipNode = pDC->pActiveClipRegion;
    x0 += pDC->i16xOffset;
    x1 += pDC->i16xOffset;
    y0 += pDC->i16yOffset;
    y1 += pDC->i16yOffset;
    //each entry in the clipping chain is a valid area to draw in
    while(pClipNode)
    {
        gfx_DrawLineSimple(pDC->pPalette,pDC->pBitmap, &pClipNode->Rect,x0, y0,x1,y1,color);
        pClipNode = pClipNode->pNext;
    }
}


////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_FillRect(gfx_DeviceContext_t *pDC, gfx_Rect_t *pRect, gfx_Color_t color)
{
    int32_t y;
    if(pRect->width > 0){
        for(y=pRect->y; y<pRect->y+pRect->height;y++)
        {
            gfx_2d_DrawLine(pDC,pRect->x, y, pRect->x + pRect->width-1, y, color);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_RectangleByPoints(gfx_DeviceContext_t *pDC,int left, int top, int right, int bottom, gfx_Color_t color)
{
    gfx_2d_DrawLine(pDC,left    , top   , right , top   , color);
    gfx_2d_DrawLine(pDC,right   , top   , right , bottom, color);
    gfx_2d_DrawLine(pDC,right   , bottom, left  , bottom, color);
    gfx_2d_DrawLine(pDC,left    , bottom, left  , top   , color);

}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_Rectangle(gfx_DeviceContext_t *pDC, gfx_Rect_t *pRect, gfx_Color_t color)
{
    gfx_2d_RectangleByPoints(pDC,pRect->x, pRect->y,pRect->x+pRect->width-1,pRect->y+pRect->height-1,color);
}


////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_GradientRectangleByPoints(gfx_DeviceContext_t *pDC, 
                                        int left, int top,
                                        int right, int bottom,
                                        gfx_Color_t StartColor, gfx_Color_t EndColor)
{
    int32_t iRed;
    int32_t iGreen;
    int32_t iBlue;

    int32_t iCount = (bottom+1) - top;

    iRed    = GET_RED  (StartColor)<<8;
    iGreen  = GET_GREEN(StartColor)<<8;
    iBlue   = GET_BLUE (StartColor)<<8;



    if(iCount > 0)
    {
        int32_t iRedStep;
        int32_t iBlueStep;
        int32_t iGreenStep;

        int32_t iRedEnd   = GET_RED(EndColor)<<8;
        int32_t iGreenEnd = GET_GREEN(EndColor)<<8;
        int32_t iBlueEnd  = GET_BLUE(EndColor)<<8;


        iRedStep    = (iRedEnd-iRed  )/(iCount);
        iGreenStep  = (iGreenEnd-iGreen)/(iCount);
        iBlueStep   = (iBlueEnd-iBlue)/(iCount);

        do
        {
            gfx_2d_DrawLine(pDC,left,top,right,top, RGB(iRed>>8,iGreen>>8,iBlue>>8));
            top ++;
            iRed    += iRedStep;
            iGreen  += iGreenStep;
            iBlue   += iBlueStep;
        }   while(iCount-->0);
    }
}


//! \brief Draws a gradient rectangle onto the current context, clipped by the active clipping region.
//!
//! The difference between this implementation of the gradient fill and the regular one is that this one starts at the outside
//! and shades towards the center.
//! 
//! \param[in] pDC          The target gfx_DeviceContext_t
//! \param[in] left         Left edge of the rectangle
//! \param[in] top          Top edge of the rectangle
//! \param[in] right        Right edge of the rectangle
//! \param[in] bottom       Bottom edge of the rectangle
//! \param[in] Startcolor   The color at the outside of the rectangle.
//! \param[in] EndColor     The color at the middle of the rectangle.
void gfx_2d_GradientInwards(gfx_DeviceContext_t *pDC, 
                            int left, int top,
                            int right, int bottom,
                            gfx_Color_t StartColor, gfx_Color_t EndColor)
{

    int32_t iRed;
    int32_t iGreen;
    int32_t iBlue;

    int32_t iCount = ((bottom+1) - top)/2;  //we will start on the outside and move in

    iRed    = GET_RED  (StartColor)<<8;
    iGreen  = GET_GREEN(StartColor)<<8;
    iBlue   = GET_BLUE (StartColor)<<8;

    if(iCount > 0){
        int32_t iRedStep;
        int32_t iBlueStep;
        int32_t iGreenStep;

        int32_t iRedEnd   = GET_RED(EndColor)<<8;
        int32_t iGreenEnd = GET_GREEN(EndColor)<<8;
        int32_t iBlueEnd  = GET_BLUE(EndColor)<<8;


        iRedStep    = (iRedEnd-iRed  )/(iCount);
        iGreenStep  = (iGreenEnd-iGreen)/(iCount);
        iBlueStep   = (iBlueEnd-iBlue)/(iCount);    

        do
        {
            gfx_2d_DrawLine(pDC,left,top,right,top, RGB(iRed>>8,iGreen>>8,iBlue>>8));
            gfx_2d_DrawLine(pDC,left,bottom,right,bottom, RGB(iRed>>8,iGreen>>8,iBlue>>8));
            top ++;
            bottom--;
            iRed    += iRedStep;
            iGreen  += iGreenStep;
            iBlue   += iBlueStep;
        }   while(iCount-->0);
    }
}



////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}

